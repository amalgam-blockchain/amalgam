#include <amalgam/plugins/witness/witness_plugin.hpp>
#include <amalgam/plugins/witness/witness_objects.hpp>

#include <amalgam/chain/database_exceptions.hpp>
#include <amalgam/chain/account_object.hpp>
#include <amalgam/chain/witness_objects.hpp>
#include <amalgam/chain/index.hpp>
#include <amalgam/chain/util/impacted.hpp>

#include <amalgam/utilities/key_conversion.hpp>
#include <amalgam/utilities/plugin_utilities.hpp>

#include <fc/io/json.hpp>
#include <fc/macros.hpp>
#include <fc/smart_ref_impl.hpp>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>


#define DISTANCE_CALC_PRECISION (10000)
#define BLOCK_PRODUCING_LAG_TIME (750)
#define BLOCK_PRODUCTION_LOOP_SLEEP_TIME (200000)


namespace amalgam { namespace plugins { namespace witness {

using namespace amalgam::chain;

using std::string;
using std::vector;

namespace bpo = boost::program_options;

void new_chain_banner( const chain::database& db )
{
   std::cerr << "\n"
      "********************************\n"
      "*                              *\n"
      "*   ------- NEW CHAIN ------   *\n"
      "*   -  Welcome to Amalgam! -   *\n"
      "*   ------------------------   *\n"
      "*                              *\n"
      "********************************\n"
      "\n";
   return;
}

namespace detail
{
   using namespace amalgam::chain;


   class witness_plugin_impl
   {
   public:
      witness_plugin_impl( boost::asio::io_service& io ) :
         _timer(io),
         _chain_plugin( appbase::app().get_plugin< amalgam::plugins::chain::chain_plugin >() ),
         _db( appbase::app().get_plugin< amalgam::plugins::chain::chain_plugin >().db() )
         {}

      void on_pre_apply_block( const chain::block_notification& note );
      void on_post_apply_block( const chain::block_notification& note );
      void on_pre_apply_transaction( const chain::transaction_notification& trx );
      void on_pre_apply_operation( const chain::operation_notification& note );
      void on_post_apply_operation( const chain::operation_notification& note );

      void update_account_bandwidth( const chain::account_object& a, uint32_t trx_size, const bandwidth_type type );

      void schedule_production_loop();
      block_production_condition::block_production_condition_enum block_production_loop();
      block_production_condition::block_production_condition_enum maybe_produce_block(fc::mutable_variant_object& capture);

      bool     _production_enabled              = false;
      uint32_t _required_witness_participation  = 33 * AMALGAM_1_PERCENT;
      uint32_t _production_skip_flags           = chain::database::skip_nothing;

      std::map< amalgam::protocol::public_key_type, fc::ecc::private_key > _private_keys;
      std::set< amalgam::protocol::account_name_type >                     _witnesses;
      boost::asio::deadline_timer                                        _timer;

      std::set< amalgam::protocol::account_name_type >                     _dupe_customs;

      plugins::chain::chain_plugin& _chain_plugin;
      chain::database&              _db;
      boost::signals2::connection   _pre_apply_block_conn;
      boost::signals2::connection   _post_apply_block_conn;
      boost::signals2::connection   _pre_apply_transaction_conn;
      boost::signals2::connection   _pre_apply_operation_conn;
      boost::signals2::connection   _post_apply_operation_conn;
   };

   void check_memo( const string& memo, const chain::account_object& account, const account_authority_object& auth )
   {
      vector< public_key_type > keys;

      try
      {
         // Check if memo is a private key
         keys.push_back( fc::ecc::extended_private_key::from_base58( memo ).get_public_key() );
      }
      catch( fc::parse_error_exception& ) {}
      catch( fc::assert_exception& ) {}

      // Get possible keys if memo was an account password
      string owner_seed = account.name + "owner" + memo;
      auto owner_secret = fc::sha256::hash( owner_seed.c_str(), owner_seed.size() );
      keys.push_back( fc::ecc::private_key::regenerate( owner_secret ).get_public_key() );

      string active_seed = account.name + "active" + memo;
      auto active_secret = fc::sha256::hash( active_seed.c_str(), active_seed.size() );
      keys.push_back( fc::ecc::private_key::regenerate( active_secret ).get_public_key() );

      string posting_seed = account.name + "posting" + memo;
      auto posting_secret = fc::sha256::hash( posting_seed.c_str(), posting_seed.size() );
      keys.push_back( fc::ecc::private_key::regenerate( posting_secret ).get_public_key() );

      // Check keys against public keys in authorities
      for( auto& key_weight_pair : auth.owner.key_auths )
      {
         for( auto& key : keys )
            AMALGAM_ASSERT( key_weight_pair.first != key,  plugin_exception,
               "Detected private owner key in memo field. You should change your owner keys." );
      }

      for( auto& key_weight_pair : auth.active.key_auths )
      {
         for( auto& key : keys )
            AMALGAM_ASSERT( key_weight_pair.first != key,  plugin_exception,
               "Detected private active key in memo field. You should change your active keys." );
      }

      for( auto& key_weight_pair : auth.posting.key_auths )
      {
         for( auto& key : keys )
            AMALGAM_ASSERT( key_weight_pair.first != key,  plugin_exception,
               "Detected private posting key in memo field. You should change your posting keys." );
      }

      const auto& memo_key = account.memo_key;
      for( auto& key : keys )
         AMALGAM_ASSERT( memo_key != key,  plugin_exception,
            "Detected private memo key in memo field. You should change your memo key." );
   }

   struct operation_visitor
   {
      operation_visitor( const chain::database& db ) : _db( db ) {}

      const chain::database& _db;

      typedef void result_type;

      template< typename T >
      void operator()( const T& )const {}

      void operator()( const transfer_operation& o )const
      {
         if( o.memo.length() > 0 )
            check_memo( o.memo,
                        _db.get< chain::account_object, chain::by_name >( o.from ),
                        _db.get< account_authority_object, chain::by_account >( o.from ) );
      }

      void operator()( const transfer_to_savings_operation& o )const
      {
         if( o.memo.length() > 0 )
            check_memo( o.memo,
                        _db.get< chain::account_object, chain::by_name >( o.from ),
                        _db.get< account_authority_object, chain::by_account >( o.from ) );
      }

      void operator()( const transfer_from_savings_operation& o )const
      {
         if( o.memo.length() > 0 )
            check_memo( o.memo,
                        _db.get< chain::account_object, chain::by_name >( o.from ),
                        _db.get< account_authority_object, chain::by_account >( o.from ) );
      }
   };

   void witness_plugin_impl::on_pre_apply_block( const chain::block_notification& b )
   {
      _dupe_customs.clear();
   }

   void witness_plugin_impl::on_pre_apply_transaction( const chain::transaction_notification& note )
   {
      const signed_transaction& trx = note.transaction;
      flat_set< account_name_type > required; vector<authority> other;
      trx.get_required_authorities( required, required, required, other );

      auto trx_size = fc::raw::pack_size(trx);

      for( const auto& auth : required )
      {
         const auto& acnt = _db.get_account( auth );

         update_account_bandwidth( acnt, trx_size, bandwidth_type::forum );

         for( const auto& op : trx.operations )
         {
            if( is_market_operation( op ) )
            {
               update_account_bandwidth( acnt, trx_size * 10, bandwidth_type::market );
               break;
            }
         }
      }
   }

   void witness_plugin_impl::on_pre_apply_operation( const chain::operation_notification& note )
   {
      if( _db.is_producing() )
      {
         note.op.visit( operation_visitor( _db ) );
      }
   }

   void witness_plugin_impl::on_post_apply_operation( const chain::operation_notification& note )
   {
      switch( note.op.which() )
      {
         case operation::tag< custom_operation >::value:
         case operation::tag< custom_json_operation >::value:
         case operation::tag< custom_binary_operation >::value:
         {
            flat_set< account_name_type > impacted;
            app::operation_get_impacted_accounts( note.op, impacted );

            for( auto& account : impacted )
               if( _db.is_producing() )
                  AMALGAM_ASSERT( _dupe_customs.insert( account ).second, plugin_exception,
                     "Account ${a} already submitted a custom json operation this block.",
                     ("a", account) );
         }
            break;
         default:
            break;
      }
   }

   void witness_plugin_impl::on_post_apply_block( const block_notification& note )
   { try {
      const signed_block& b = note.block;
      int64_t max_block_size = _db.get_dynamic_global_properties().maximum_block_size;

      auto reserve_ratio_ptr = _db.find( reserve_ratio_id_type() );

      int32_t block_size = int32_t( fc::raw::pack_size( b ) );
      if( BOOST_UNLIKELY( reserve_ratio_ptr == nullptr ) )
      {
         _db.create< reserve_ratio_object >( [&]( reserve_ratio_object& r )
         {
            r.average_block_size = 0;
            r.current_reserve_ratio = AMALGAM_MAX_RESERVE_RATIO * RESERVE_RATIO_PRECISION;
            r.max_virtual_bandwidth = ( static_cast<uint128_t>( AMALGAM_MAX_BLOCK_SIZE) * AMALGAM_MAX_RESERVE_RATIO
                                       * AMALGAM_BANDWIDTH_PRECISION * AMALGAM_BANDWIDTH_AVERAGE_WINDOW_SECONDS )
                                       / AMALGAM_BLOCK_INTERVAL;
         });
         reserve_ratio_ptr = &_db.get( reserve_ratio_id_type() );
      }
      else
      {
         _db.modify( *reserve_ratio_ptr, [&]( reserve_ratio_object& r )
         {
            r.average_block_size = ( 99 * r.average_block_size + block_size ) / 100;

            /**
            * About once per minute the average network use is consulted and used to
            * adjust the reserve ratio. Anything above 25% usage reduces the reserve
            * ratio proportional to the distance from 25%. If usage is at 50% then
            * the reserve ratio will half. Likewise, if it is at 12% it will increase by 50%.
            *
            * If the reserve ratio is consistently low, then it is probably time to increase
            * the capcacity of the network.
            *
            * This algorithm is designed to react quickly to observations significantly
            * different from past observed behavior and make small adjustments when
            * behavior is within expected norms.
            */
            if( _db.head_block_num() % 20 == 0 )
            {
               int64_t distance = ( ( r.average_block_size - ( max_block_size / 4 ) ) * DISTANCE_CALC_PRECISION )
                  / ( max_block_size / 4 );
               auto old_reserve_ratio = r.current_reserve_ratio;

               if( distance > 0 )
               {
                  r.current_reserve_ratio -= ( r.current_reserve_ratio * distance ) / ( distance + DISTANCE_CALC_PRECISION );

                  // We do not want the reserve ratio to drop below 1
                  if( r.current_reserve_ratio < RESERVE_RATIO_PRECISION )
                     r.current_reserve_ratio = RESERVE_RATIO_PRECISION;
               }
               else
               {
                  // By default, we should always slowly increase the reserve ratio.
                  r.current_reserve_ratio += std::max( RESERVE_RATIO_MIN_INCREMENT, ( r.current_reserve_ratio * distance ) / ( distance - DISTANCE_CALC_PRECISION ) );

                  if( r.current_reserve_ratio > AMALGAM_MAX_RESERVE_RATIO * RESERVE_RATIO_PRECISION )
                     r.current_reserve_ratio = AMALGAM_MAX_RESERVE_RATIO * RESERVE_RATIO_PRECISION;
               }

               if( old_reserve_ratio != r.current_reserve_ratio )
               {
                  ilog( "Reserve ratio updated from ${old} to ${new}. Block: ${blocknum}",
                     ("old", old_reserve_ratio)
                     ("new", r.current_reserve_ratio)
                     ("blocknum", _db.head_block_num()) );
               }

               r.max_virtual_bandwidth = ( uint128_t( max_block_size ) * uint128_t( r.current_reserve_ratio )
                                          * uint128_t( AMALGAM_BANDWIDTH_PRECISION * AMALGAM_BANDWIDTH_AVERAGE_WINDOW_SECONDS ) )
                                          / ( AMALGAM_BLOCK_INTERVAL * RESERVE_RATIO_PRECISION );
            }
         });
      }

      _dupe_customs.clear();

   } FC_LOG_AND_RETHROW() }

   void witness_plugin_impl::update_account_bandwidth( const chain::account_object& a, uint32_t trx_size, const bandwidth_type type )
   {
      const auto& props = _db.get_dynamic_global_properties();
      bool has_bandwidth = true;

      if( props.total_vesting_shares.amount > 0 )
      {
         auto band = _db.find< account_bandwidth_object, by_account_bandwidth_type >( boost::make_tuple( a.name, type ) );

         if( band == nullptr )
         {
            band = &_db.create< account_bandwidth_object >( [&]( account_bandwidth_object& b )
            {
               b.account = a.name;
               b.type = type;
            });
         }

         share_type new_bandwidth;
         share_type trx_bandwidth = trx_size * AMALGAM_BANDWIDTH_PRECISION;
         auto delta_time = ( _db.head_block_time() - band->last_bandwidth_update ).to_seconds();

         if( delta_time > AMALGAM_BANDWIDTH_AVERAGE_WINDOW_SECONDS )
            new_bandwidth = 0;
         else
            new_bandwidth = ( ( ( AMALGAM_BANDWIDTH_AVERAGE_WINDOW_SECONDS - delta_time ) * fc::uint128( band->average_bandwidth.value ) )
               / AMALGAM_BANDWIDTH_AVERAGE_WINDOW_SECONDS ).to_uint64();

         new_bandwidth += trx_bandwidth;

         _db.modify( *band, [&]( account_bandwidth_object& b )
         {
            b.average_bandwidth = new_bandwidth;
            b.lifetime_bandwidth += trx_bandwidth;
            b.last_bandwidth_update = _db.head_block_time();
         });

         fc::uint128 account_vshares( _db.get_effective_vesting_shares(a, VESTS_SYMBOL).amount.value );
         fc::uint128 total_vshares( props.total_vesting_shares.amount.value );
         fc::uint128 account_average_bandwidth( band->average_bandwidth.value );
         fc::uint128 max_virtual_bandwidth( _db.get( reserve_ratio_id_type() ).max_virtual_bandwidth );

         has_bandwidth = ( account_vshares * max_virtual_bandwidth ) > ( account_average_bandwidth * total_vshares );

         if( _db.is_producing() )
         {
            AMALGAM_ASSERT( has_bandwidth,  plugin_exception,
               "Account: ${account} bandwidth limit exceeded. Please wait to transact or power up AMALGAM.",
               ("account", a.name)
               ("account_vshares", account_vshares)
               ("account_average_bandwidth", account_average_bandwidth)
               ("max_virtual_bandwidth", max_virtual_bandwidth)
               ("total_vesting_shares", total_vshares) );
         }
      }
   }

   void witness_plugin_impl::schedule_production_loop() {
      // Sleep for 200ms, before checking the block production
      fc::time_point now = fc::time_point::now();
      int64_t time_to_sleep = BLOCK_PRODUCTION_LOOP_SLEEP_TIME - (now.time_since_epoch().count() % BLOCK_PRODUCTION_LOOP_SLEEP_TIME);
      if (time_to_sleep < 50000) // we must sleep for at least 50ms
          time_to_sleep += BLOCK_PRODUCTION_LOOP_SLEEP_TIME;

      _timer.expires_from_now( boost::posix_time::microseconds( time_to_sleep ) );
      _timer.async_wait( boost::bind( &witness_plugin_impl::block_production_loop, this ) );
   }

   block_production_condition::block_production_condition_enum witness_plugin_impl::block_production_loop()
   {
      if( fc::time_point::now() < fc::time_point(AMALGAM_GENESIS_TIME) )
      {
         wlog( "waiting until genesis time to produce block: ${t}", ("t",AMALGAM_GENESIS_TIME) );
         schedule_production_loop();
         return block_production_condition::wait_for_genesis;
      }

      block_production_condition::block_production_condition_enum result;
      fc::mutable_variant_object capture;
      try
      {
         result = maybe_produce_block(capture);
      }
      catch( const fc::canceled_exception& )
      {
         //We're trying to exit. Go ahead and let this one out.
         throw;
      }
      catch( const chain::unknown_hardfork_exception& e )
      {
         // Hit a hardfork that the current node know nothing about, stop production and inform user
         elog( "${e}\nNode may be out of date...", ("e", e.to_detail_string()) );
         throw;
      }
      catch( const fc::exception& e )
      {
         elog("Got exception while generating block:\n${e}", ("e", e.to_detail_string()));
         result = block_production_condition::exception_producing_block;
      }

      switch(result)
      {
         case block_production_condition::produced:
            ilog("Generated block #${n} with timestamp ${t} at time ${c}", (capture));
            break;
         case block_production_condition::not_synced:
   //         ilog("Not producing block because production is disabled until we receive a recent block (see: --enable-stale-production)");
            break;
         case block_production_condition::not_my_turn:
   //         ilog("Not producing block because it isn't my turn");
            break;
         case block_production_condition::not_time_yet:
   //         ilog("Not producing block because slot has not yet arrived");
            break;
         case block_production_condition::no_private_key:
            ilog("Not producing block because I don't have the private key for ${scheduled_key}", (capture) );
            break;
         case block_production_condition::low_participation:
            elog("Not producing block because node appears to be on a minority fork with only ${pct}% witness participation", (capture) );
            break;
         case block_production_condition::lag:
            elog("Not producing block because node didn't wake up within ${t}ms of the slot time.", ("t", BLOCK_PRODUCING_LAG_TIME));
            break;
         case block_production_condition::consecutive:
            elog("Not producing block because the last block was generated by the same witness.\nThis node is probably disconnected from the network so block production has been disabled.\nDisable this check with --allow-consecutive option.");
            break;
         case block_production_condition::exception_producing_block:
            elog( "exception producing block" );
            break;
         case block_production_condition::wait_for_genesis:
            break;
      }

      schedule_production_loop();
      return result;
   }

   block_production_condition::block_production_condition_enum witness_plugin_impl::maybe_produce_block(fc::mutable_variant_object& capture)
   {
      fc::time_point now_fine = fc::time_point::now();
      fc::time_point_sec now = now_fine + fc::microseconds( 500000 );

      // If the next block production opportunity is in the present or future, we're synced.
      if( !_production_enabled )
      {
         if( _db.get_slot_time(1) >= now )
            _production_enabled = true;
         else
            return block_production_condition::not_synced;
      }

      // is anyone scheduled to produce now or one second in the future?
      uint32_t slot = _db.get_slot_at_time( now );
      if( slot == 0 )
      {
         capture("next_time", _db.get_slot_time(1));
         return block_production_condition::not_time_yet;
      }

      //
      // this assert should not fail, because now <= db.head_block_time()
      // should have resulted in slot == 0.
      //
      // if this assert triggers, there is a serious bug in get_slot_at_time()
      // which would result in allowing a later block to have a timestamp
      // less than or equal to the previous block
      //
      assert( now > _db.head_block_time() );

      chain::account_name_type scheduled_witness = _db.get_scheduled_witness( slot );
      // we must control the witness scheduled to produce the next block.
      if( _witnesses.find( scheduled_witness ) == _witnesses.end() )
      {
         capture("scheduled_witness", scheduled_witness);
         return block_production_condition::not_my_turn;
      }

      fc::time_point_sec scheduled_time = _db.get_slot_time( slot );
      chain::public_key_type scheduled_key = _db.get< chain::witness_object, chain::by_name >(scheduled_witness).signing_key;
      auto private_key_itr = _private_keys.find( scheduled_key );

      if( private_key_itr == _private_keys.end() )
      {
         capture("scheduled_witness", scheduled_witness);
         capture("scheduled_key", scheduled_key);
         return block_production_condition::no_private_key;
      }

      uint32_t prate = _db.witness_participation_rate();
      if( prate < _required_witness_participation )
      {
         capture("pct", uint32_t(100*uint64_t(prate) / AMALGAM_1_PERCENT));
         return block_production_condition::low_participation;
      }

      if( llabs((scheduled_time - now).count()) > fc::milliseconds( BLOCK_PRODUCING_LAG_TIME ).count() )
      {
         capture("scheduled_time", scheduled_time)("now", now);
         return block_production_condition::lag;
      }

      auto block = _chain_plugin.generate_block(
         scheduled_time,
         scheduled_witness,
         private_key_itr->second,
         _production_skip_flags
         );
      capture("n", block.block_num())("t", block.timestamp)("c", now);

      appbase::app().get_plugin< amalgam::plugins::p2p::p2p_plugin >().broadcast_block( block );
      return block_production_condition::produced;
   }

} // detail


witness_plugin::witness_plugin() {}
witness_plugin::~witness_plugin() {}

void witness_plugin::set_program_options(
   boost::program_options::options_description& cli,
   boost::program_options::options_description& cfg)
{
   string witness_id_example = "creator";
   cfg.add_options()
         ("enable-stale-production", bpo::value<bool>()->default_value( false ), "Enable block production, even if the chain is stale.")
         ("required-participation", bpo::value< uint32_t >()->default_value( 33 ), "Percent of witnesses (0-99) that must be participating in order to produce blocks")
         ("witness,w", bpo::value<vector<string>>()->composing()->multitoken(),
            ("name of witness controlled by this node (e.g. " + witness_id_example + " )" ).c_str() )
         ("private-key", bpo::value<vector<string>>()->composing()->multitoken(), "WIF PRIVATE KEY to be used by one or more witnesses" )
         ;
   cli.add_options()
         ("enable-stale-production", bpo::bool_switch()->default_value( false ), "Enable block production, even if the chain is stale.")
         ;
}

void witness_plugin::plugin_initialize(const boost::program_options::variables_map& options)
{ try {
   ilog( "Initializing witness plugin" );
   my = std::make_unique< detail::witness_plugin_impl >( appbase::app().get_io_service() );

   AMALGAM_LOAD_VALUE_SET( options, "witness", my->_witnesses, amalgam::protocol::account_name_type )

   if( options.count("private-key") )
   {
      const std::vector<std::string> keys = options["private-key"].as<std::vector<std::string>>();
      for (const std::string& wif_key : keys )
      {
         fc::optional<fc::ecc::private_key> private_key = amalgam::utilities::wif_to_key(wif_key);
         FC_ASSERT( private_key.valid(), "unable to parse private key" );
         my->_private_keys[private_key->get_public_key()] = *private_key;
      }
   }

   my->_production_enabled = options.at( "enable-stale-production" ).as< bool >();

   if( options.count( "required-participation" ) )
   {
      my->_required_witness_participation = AMALGAM_1_PERCENT * options.at( "required-participation" ).as< uint32_t >();
   }

   my->_pre_apply_block_conn = my->_db.add_post_apply_block_handler(
      [&]( const chain::block_notification& note ){ my->on_pre_apply_block( note ); }, *this, 0 );
   my->_post_apply_block_conn = my->_db.add_post_apply_block_handler(
      [&]( const chain::block_notification& note ){ my->on_post_apply_block( note ); }, *this, 0 );
   my->_pre_apply_transaction_conn = my->_db.add_pre_apply_transaction_handler(
      [&]( const chain::transaction_notification& note ){ my->on_pre_apply_transaction( note ); }, *this, 0 );
   my->_pre_apply_operation_conn = my->_db.add_pre_apply_operation_handler(
      [&]( const chain::operation_notification& note ){ my->on_pre_apply_operation( note ); }, *this, 0);
   my->_post_apply_operation_conn = my->_db.add_pre_apply_operation_handler(
      [&]( const chain::operation_notification& note ){ my->on_post_apply_operation( note ); }, *this, 0);

   add_plugin_index< account_bandwidth_index >( my->_db );
   add_plugin_index< reserve_ratio_index     >( my->_db );

   if( my->_witnesses.size() && my->_private_keys.size() )
      my->_chain_plugin.set_write_lock_hold_time( -1 );
} FC_LOG_AND_RETHROW() }

void witness_plugin::plugin_startup()
{ try {
   ilog("witness plugin:  plugin_startup() begin" );
   chain::database& d = appbase::app().get_plugin< amalgam::plugins::chain::chain_plugin >().db();

   if( !my->_witnesses.empty() )
   {
      ilog( "Launching block production for ${n} witnesses.", ("n", my->_witnesses.size()) );
      appbase::app().get_plugin< amalgam::plugins::p2p::p2p_plugin >().set_block_production( true );
      if( my->_production_enabled )
      {
         if( d.head_block_num() == 0 )
            new_chain_banner( d );
         my->_production_skip_flags |= chain::database::skip_undo_history_check;
      }
      my->schedule_production_loop();
   } else
      elog("No witnesses configured! Please add witness IDs and private keys to configuration.");
   ilog("witness plugin:  plugin_startup() end");
} FC_CAPTURE_AND_RETHROW() }

void witness_plugin::plugin_shutdown()
{
   try
   {
      chain::util::disconnect_signal( my->_pre_apply_block_conn );
      chain::util::disconnect_signal( my->_post_apply_block_conn );
      chain::util::disconnect_signal( my->_pre_apply_transaction_conn );
      chain::util::disconnect_signal( my->_pre_apply_operation_conn );
      chain::util::disconnect_signal( my->_post_apply_operation_conn );

      my->_timer.cancel();
   }
   catch(fc::exception& e)
   {
      edump( (e.to_detail_string()) );
   }
}

} } } // amalgam::plugins::witness
