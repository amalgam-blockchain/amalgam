#include <amalgam/app/api_context.hpp>
#include <amalgam/app/application.hpp>
#include <amalgam/app/database_api.hpp>

#include <amalgam/protocol/get_config.hpp>

#include <amalgam/chain/util/reward.hpp>

#include <fc/bloom_filter.hpp>
#include <fc/smart_ref_impl.hpp>
#include <fc/crypto/hex.hpp>

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>


#include <cctype>

#include <cfenv>
#include <iostream>

namespace amalgam { namespace app {

class database_api_impl;


class database_api_impl : public std::enable_shared_from_this<database_api_impl>
{
   public:
      database_api_impl( const amalgam::app::api_context& ctx  );
      ~database_api_impl();

      // Blocks and transactions
      optional<block_header> get_block_header(uint32_t block_num)const;
      optional<signed_block_api_obj> get_block(uint32_t block_num)const;
      vector<applied_operation> get_ops_in_block(uint32_t block_num, bool only_virtual)const;

      // Globals
      fc::variant_object get_config()const;
      dynamic_global_property_api_obj get_dynamic_global_properties()const;

      // Accounts
      vector< extended_account > get_accounts( vector< string > names )const;
      vector<optional<account_api_obj>> lookup_account_names(const vector<string>& account_names)const;
      set<string> lookup_accounts(const string& lower_bound_name, uint32_t limit)const;
      uint64_t get_account_count()const;

      // Witnesses
      vector<optional<witness_api_obj>> get_witnesses(const vector<witness_id_type>& witness_ids)const;
      fc::optional<witness_api_obj> get_witness_by_account( string account_name )const;
      set<account_name_type> lookup_witness_accounts(const string& lower_bound_name, uint32_t limit)const;
      uint64_t get_witness_count()const;

      // Market
      order_book get_order_book( uint32_t limit )const;
      vector< liquidity_balance > get_liquidity_queue( string start_account, uint32_t limit )const;

      // Authority / validation
      std::string get_transaction_hex(const signed_transaction& trx)const;
      set<public_key_type> get_required_signatures( const signed_transaction& trx, const flat_set<public_key_type>& available_keys )const;
      set<public_key_type> get_potential_signatures( const signed_transaction& trx )const;
      bool verify_authority( const signed_transaction& trx )const;
      bool verify_account_authority( const string& name_or_id, const flat_set<public_key_type>& signers )const;
      verify_signatures_return verify_signatures( const verify_signatures_args& args )const;

      amalgam::chain::database&                _db;

      bool _disable_get_block = false;
};

applied_operation::applied_operation() {}

applied_operation::applied_operation( const operation_object& op_obj )
 : trx_id( op_obj.trx_id ),
   block( op_obj.block ),
   trx_in_block( op_obj.trx_in_block ),
   op_in_trx( op_obj.op_in_trx ),
   virtual_op( op_obj.virtual_op ),
   timestamp( op_obj.timestamp )
{
   //fc::raw::unpack( op_obj.serialized_op, op );     // g++ refuses to compile this as ambiguous
   op = fc::raw::unpack< operation >( op_obj.serialized_op );
}

void find_accounts( set<string>& accounts, const discussion& d ) {
   accounts.insert( d.author );
}

//////////////////////////////////////////////////////////////////////
//                                                                  //
// Constructors                                                     //
//                                                                  //
//////////////////////////////////////////////////////////////////////

database_api::database_api( const amalgam::app::api_context& ctx )
   : my( new database_api_impl( ctx ) ) {}

database_api::~database_api() {}

database_api_impl::database_api_impl( const amalgam::app::api_context& ctx )
   : _db( *ctx.app.chain_database() )
{
   wlog("creating database api ${x}", ("x",int64_t(this)) );

   _disable_get_block = ctx.app._disable_get_block;
}

database_api_impl::~database_api_impl()
{
   elog("freeing database api ${x}", ("x",int64_t(this)) );
}

void database_api::on_api_startup() {}

//////////////////////////////////////////////////////////////////////
//                                                                  //
// Blocks and transactions                                          //
//                                                                  //
//////////////////////////////////////////////////////////////////////

optional<block_header> database_api::get_block_header(uint32_t block_num)const
{
   FC_ASSERT( !my->_disable_get_block, "get_block_header is disabled on this node." );

   return my->_db.with_read_lock( [&]()
   {
      return my->get_block_header( block_num );
   });
}

optional<block_header> database_api_impl::get_block_header(uint32_t block_num) const
{
   auto result = _db.fetch_block_by_number(block_num);
   if(result)
      return *result;
   return {};
}

optional<signed_block_api_obj> database_api::get_block(uint32_t block_num)const
{
   FC_ASSERT( !my->_disable_get_block, "get_block is disabled on this node." );

   return my->_db.with_read_lock( [&]()
   {
      return my->get_block( block_num );
   });
}

optional<signed_block_api_obj> database_api_impl::get_block(uint32_t block_num)const
{
   return _db.fetch_block_by_number(block_num);
}

vector<applied_operation> database_api::get_ops_in_block(uint32_t block_num, bool only_virtual)const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->get_ops_in_block( block_num, only_virtual );
   });
}

vector<applied_operation> database_api_impl::get_ops_in_block(uint32_t block_num, bool only_virtual)const
{
   const auto& idx = _db.get_index< operation_index >().indices().get< by_location >();
   auto itr = idx.lower_bound( block_num );
   vector<applied_operation> result;
   applied_operation temp;
   while( itr != idx.end() && itr->block == block_num )
   {
      temp = *itr;
      if( !only_virtual || is_virtual_operation(temp.op) )
         result.push_back(temp);
      ++itr;
   }
   return result;
}


//////////////////////////////////////////////////////////////////////
//                                                                  //
// Globals                                                          //
//                                                                  //
//////////////////////////////////////////////////////////////////////=

fc::variant_object database_api::get_config()const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->get_config();
   });
}

fc::variant_object database_api_impl::get_config()const
{
   return amalgam::protocol::get_config();
}

dynamic_global_property_api_obj database_api::get_dynamic_global_properties()const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->get_dynamic_global_properties();
   });
}

chain_properties database_api::get_chain_properties()const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->_db.get_witness_schedule_object().median_props;
   });
}

feed_history_api_obj database_api::get_feed_history()const
{
   return my->_db.with_read_lock( [&]()
   {
      return feed_history_api_obj( my->_db.get_feed_history() );
   });
}

price database_api::get_current_median_history_price()const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->_db.get_feed_history().current_median_history;
   });
}

dynamic_global_property_api_obj database_api_impl::get_dynamic_global_properties()const
{
   return dynamic_global_property_api_obj( _db.get( dynamic_global_property_id_type() ), _db );
}

witness_schedule_api_obj database_api::get_witness_schedule()const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->_db.get(witness_schedule_id_type());
   });
}

hardfork_version database_api::get_hardfork_version()const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->_db.get(hardfork_property_id_type()).current_hardfork_version;
   });
}

scheduled_hardfork database_api::get_next_scheduled_hardfork() const
{
   return my->_db.with_read_lock( [&]()
   {
      scheduled_hardfork shf;
      const auto& hpo = my->_db.get(hardfork_property_id_type());
      shf.hf_version = hpo.next_hardfork;
      shf.live_time = hpo.next_hardfork_time;
      return shf;
   });
}

reward_fund_api_obj database_api::get_reward_fund( string name )const
{
   return my->_db.with_read_lock( [&]()
   {
      auto fund = my->_db.find< reward_fund_object, by_name >( name );
      FC_ASSERT( fund != nullptr, "Invalid reward fund name" );

      return *fund;
   });
}

//////////////////////////////////////////////////////////////////////
//                                                                  //
// Accounts                                                         //
//                                                                  //
//////////////////////////////////////////////////////////////////////

vector< extended_account > database_api::get_accounts( vector< string > names )const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->get_accounts( names );
   });
}

vector< extended_account > database_api_impl::get_accounts( vector< string > names )const
{
   const auto& idx  = _db.get_index< account_index >().indices().get< by_name >();
   const auto& vidx = _db.get_index< witness_vote_index >().indices().get< by_account_witness >();
   vector< extended_account > results;

   for( auto name: names )
   {
      auto itr = idx.find( name );
      if ( itr != idx.end() )
      {
         results.push_back( extended_account( *itr, _db ) );

         auto vitr = vidx.lower_bound( boost::make_tuple( itr->id, witness_id_type() ) );
         while( vitr != vidx.end() && vitr->account == itr->id ) {
            results.back().witness_votes.insert(_db.get(vitr->witness).owner);
            ++vitr;
         }
      }
   }

   return results;
}

vector<optional<account_api_obj>> database_api::lookup_account_names(const vector<string>& account_names)const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->lookup_account_names( account_names );
   });
}

vector<optional<account_api_obj>> database_api_impl::lookup_account_names(const vector<string>& account_names)const
{
   vector<optional<account_api_obj> > result;
   result.reserve(account_names.size());

   for( auto& name : account_names )
   {
      auto itr = _db.find< account_object, by_name >( name );

      if( itr )
      {
         result.push_back( account_api_obj( *itr, _db ) );
      }
      else
      {
         result.push_back( optional< account_api_obj>() );
      }
   }

   return result;
}

set<string> database_api::lookup_accounts(const string& lower_bound_name, uint32_t limit)const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->lookup_accounts( lower_bound_name, limit );
   });
}

set<string> database_api_impl::lookup_accounts(const string& lower_bound_name, uint32_t limit)const
{
   FC_ASSERT( limit <= 1000 );
   const auto& accounts_by_name = _db.get_index<account_index>().indices().get<by_name>();
   set<string> result;

   for( auto itr = accounts_by_name.lower_bound(lower_bound_name);
        limit-- && itr != accounts_by_name.end();
        ++itr )
   {
      result.insert(itr->name);
   }

   return result;
}

uint64_t database_api::get_account_count()const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->get_account_count();
   });
}

uint64_t database_api_impl::get_account_count()const
{
   return _db.get_index<account_index>().indices().size();
}

vector< owner_authority_history_api_obj > database_api::get_owner_history( string account )const
{
   return my->_db.with_read_lock( [&]()
   {
      vector< owner_authority_history_api_obj > results;

      const auto& hist_idx = my->_db.get_index< owner_authority_history_index >().indices().get< by_account >();
      auto itr = hist_idx.lower_bound( account );

      while( itr != hist_idx.end() && itr->account == account )
      {
         results.push_back( owner_authority_history_api_obj( *itr ) );
         ++itr;
      }

      return results;
   });
}

optional< account_recovery_request_api_obj > database_api::get_recovery_request( string account )const
{
   return my->_db.with_read_lock( [&]()
   {
      optional< account_recovery_request_api_obj > result;

      const auto& rec_idx = my->_db.get_index< account_recovery_request_index >().indices().get< by_account >();
      auto req = rec_idx.find( account );

      if( req != rec_idx.end() )
         result = account_recovery_request_api_obj( *req );

      return result;
   });
}

optional< escrow_api_obj > database_api::get_escrow( string from, uint32_t escrow_id )const
{
   return my->_db.with_read_lock( [&]()
   {
      optional< escrow_api_obj > result;

      try
      {
         result = my->_db.get_escrow( from, escrow_id );
      }
      catch ( ... ) {}

      return result;
   });
}

vector< withdraw_route > database_api::get_withdraw_routes( string account, withdraw_route_type type )const
{
   return my->_db.with_read_lock( [&]()
   {
      vector< withdraw_route > result;

      const auto& acc = my->_db.get_account( account );

      if( type == outgoing || type == all )
      {
         const auto& by_route = my->_db.get_index< withdraw_vesting_route_index >().indices().get< by_withdraw_route >();
         auto route = by_route.lower_bound( acc.id );

         while( route != by_route.end() && route->from_account == acc.id )
         {
            withdraw_route r;
            r.from_account = account;
            r.to_account = my->_db.get( route->to_account ).name;
            r.percent = route->percent;
            r.auto_vest = route->auto_vest;

            result.push_back( r );

            ++route;
         }
      }

      if( type == incoming || type == all )
      {
         const auto& by_dest = my->_db.get_index< withdraw_vesting_route_index >().indices().get< by_destination >();
         auto route = by_dest.lower_bound( acc.id );

         while( route != by_dest.end() && route->to_account == acc.id )
         {
            withdraw_route r;
            r.from_account = my->_db.get( route->from_account ).name;
            r.to_account = account;
            r.percent = route->percent;
            r.auto_vest = route->auto_vest;

            result.push_back( r );

            ++route;
         }
      }

      return result;
   });
}

optional< account_bandwidth_api_obj > database_api::get_account_bandwidth( string account, witness::bandwidth_type type )const
{
   optional< account_bandwidth_api_obj > result;

   if( my->_db.has_index< witness::account_bandwidth_index >() )
   {
      auto band = my->_db.find< witness::account_bandwidth_object, witness::by_account_bandwidth_type >( boost::make_tuple( account, type ) );
      if( band != nullptr )
         result = *band;
   }

   return result;
}

//////////////////////////////////////////////////////////////////////
//                                                                  //
// Witnesses                                                        //
//                                                                  //
//////////////////////////////////////////////////////////////////////

vector<optional<witness_api_obj>> database_api::get_witnesses(const vector<witness_id_type>& witness_ids)const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->get_witnesses( witness_ids );
   });
}

vector<optional<witness_api_obj>> database_api_impl::get_witnesses(const vector<witness_id_type>& witness_ids)const
{
   vector<optional<witness_api_obj>> result; result.reserve(witness_ids.size());
   std::transform(witness_ids.begin(), witness_ids.end(), std::back_inserter(result),
                  [this](witness_id_type id) -> optional<witness_api_obj> {
      if(auto o = _db.find(id))
         return *o;
      return {};
   });
   return result;
}

fc::optional<witness_api_obj> database_api::get_witness_by_account( string account_name ) const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->get_witness_by_account( account_name );
   });
}

vector< witness_api_obj > database_api::get_witnesses_by_vote( string from, uint32_t limit )const
{
   FC_ASSERT(limit <= 100);
   return my->_db.with_read_lock( [&]()
   {
      vector<witness_api_obj> result;
      result.reserve(limit);

      const auto& name_idx = my->_db.get_index< witness_index >().indices().get< by_name >();
      const auto& vote_idx = my->_db.get_index< witness_index >().indices().get< by_vote_name >();

      auto itr = vote_idx.begin();
      if( from.size() ) {
         auto nameitr = name_idx.find( from );
         FC_ASSERT( nameitr != name_idx.end(), "invalid witness name ${n}", ("n",from) );
         itr = vote_idx.iterator_to( *nameitr );
      }

      while( itr != vote_idx.end()  &&
            result.size() < limit &&
            itr->votes > 0 )
      {
         result.push_back( witness_api_obj( *itr ) );
         ++itr;
      }
      return result;
   });
}

fc::optional<witness_api_obj> database_api_impl::get_witness_by_account( string account_name ) const
{
   const auto& idx = _db.get_index< witness_index >().indices().get< by_name >();
   auto itr = idx.find( account_name );
   if( itr != idx.end() )
      return witness_api_obj( *itr );
   return {};
}

set< account_name_type > database_api::lookup_witness_accounts( const string& lower_bound_name, uint32_t limit ) const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->lookup_witness_accounts( lower_bound_name, limit );
   });
}

set< account_name_type > database_api_impl::lookup_witness_accounts( const string& lower_bound_name, uint32_t limit ) const
{
   FC_ASSERT( limit <= 1000 );
   const auto& witnesses_by_id = _db.get_index< witness_index >().indices().get< by_id >();

   // get all the names and look them all up, sort them, then figure out what
   // records to return.  This could be optimized, but we expect the
   // number of witnesses to be few and the frequency of calls to be rare
   set< account_name_type > witnesses_by_account_name;
   for ( const witness_api_obj& witness : witnesses_by_id )
      if ( witness.owner >= lower_bound_name ) // we can ignore anything below lower_bound_name
         witnesses_by_account_name.insert( witness.owner );

   auto end_iter = witnesses_by_account_name.begin();
   while ( end_iter != witnesses_by_account_name.end() && limit-- )
       ++end_iter;
   witnesses_by_account_name.erase( end_iter, witnesses_by_account_name.end() );
   return witnesses_by_account_name;
}

uint64_t database_api::get_witness_count()const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->get_witness_count();
   });
}

uint64_t database_api_impl::get_witness_count()const
{
   return _db.get_index<witness_index>().indices().size();
}

//////////////////////////////////////////////////////////////////////
//                                                                  //
// Market                                                           //
//                                                                  //
//////////////////////////////////////////////////////////////////////

order_book database_api::get_order_book( uint32_t limit )const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->get_order_book( limit );
   });
}

vector<extended_limit_order> database_api::get_open_orders( string owner )const
{
   return my->_db.with_read_lock( [&]()
   {
      vector<extended_limit_order> result;
      const auto& idx = my->_db.get_index<limit_order_index>().indices().get<by_account>();
      auto itr = idx.lower_bound( owner );
      while( itr != idx.end() && itr->seller == owner ) {
         result.push_back( *itr );

         if( itr->sell_price.base.symbol == AMALGAM_SYMBOL )
            result.back().real_price = (~result.back().sell_price).to_real();
         else
            result.back().real_price = (result.back().sell_price).to_real();
         ++itr;
      }
      return result;
   });
}

order_book database_api_impl::get_order_book( uint32_t limit )const
{
   FC_ASSERT( limit <= 1000 );
   order_book result;

   auto max_sell = price::max( ABD_SYMBOL, AMALGAM_SYMBOL );
   auto max_buy = price::max( AMALGAM_SYMBOL, ABD_SYMBOL );

   const auto& limit_price_idx = _db.get_index<limit_order_index>().indices().get<by_price>();
   auto sell_itr = limit_price_idx.lower_bound(max_sell);
   auto buy_itr  = limit_price_idx.lower_bound(max_buy);
   auto end = limit_price_idx.end();

   while(  sell_itr != end && sell_itr->sell_price.base.symbol == ABD_SYMBOL && result.bids.size() < limit )
   {
      auto itr = sell_itr;
      order cur;
      cur.order_price = itr->sell_price;
      cur.real_price  = (cur.order_price).to_real();
      cur.abd = itr->for_sale;
      cur.amalgam = ( asset( itr->for_sale, ABD_SYMBOL ) * cur.order_price ).amount;
      cur.created = itr->created;
      result.bids.push_back( cur );
      ++sell_itr;
   }
   while(  buy_itr != end && buy_itr->sell_price.base.symbol == AMALGAM_SYMBOL && result.asks.size() < limit )
   {
      auto itr = buy_itr;
      order cur;
      cur.order_price = itr->sell_price;
      cur.real_price  = (~cur.order_price).to_real();
      cur.amalgam   = itr->for_sale;
      cur.abd     = ( asset( itr->for_sale, AMALGAM_SYMBOL ) * cur.order_price ).amount;
      cur.created = itr->created;
      result.asks.push_back( cur );
      ++buy_itr;
   }


   return result;
}

vector< liquidity_balance > database_api::get_liquidity_queue( string start_account, uint32_t limit )const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->get_liquidity_queue( start_account, limit );
   });
}

vector< liquidity_balance > database_api_impl::get_liquidity_queue( string start_account, uint32_t limit )const
{
   FC_ASSERT( limit <= 1000 );

   const auto& liq_idx = _db.get_index< liquidity_reward_balance_index >().indices().get< by_volume_weight >();
   auto itr = liq_idx.begin();
   vector< liquidity_balance > result;

   result.reserve( limit );

   if( start_account.length() )
   {
      const auto& liq_by_acc = _db.get_index< liquidity_reward_balance_index >().indices().get< by_owner >();
      auto acc = liq_by_acc.find( _db.get_account( start_account ).id );

      if( acc != liq_by_acc.end() )
      {
         itr = liq_idx.find( boost::make_tuple( acc->weight, acc->owner ) );
      }
      else
      {
         itr = liq_idx.end();
      }
   }

   while( itr != liq_idx.end() && result.size() < limit )
   {
      liquidity_balance bal;
      bal.account = _db.get(itr->owner).name;
      bal.weight = itr->weight;
      result.push_back( bal );

      ++itr;
   }

   return result;
}

//////////////////////////////////////////////////////////////////////
//                                                                  //
// Authority / validation                                           //
//                                                                  //
//////////////////////////////////////////////////////////////////////

std::string database_api::get_transaction_hex(const signed_transaction& trx)const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->get_transaction_hex( trx );
   });
}

std::string database_api_impl::get_transaction_hex(const signed_transaction& trx)const
{
   return fc::to_hex(fc::raw::pack(trx));
}

set<public_key_type> database_api::get_required_signatures( const signed_transaction& trx, const flat_set<public_key_type>& available_keys )const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->get_required_signatures( trx, available_keys );
   });
}

set<public_key_type> database_api_impl::get_required_signatures( const signed_transaction& trx, const flat_set<public_key_type>& available_keys )const
{
   auto result = trx.get_required_signatures( AMALGAM_CHAIN_ID,
                                              available_keys,
                                              [&]( string account_name ){ return authority( _db.get< account_authority_object, by_account >( account_name ).active  ); },
                                              [&]( string account_name ){ return authority( _db.get< account_authority_object, by_account >( account_name ).owner   ); },
                                              [&]( string account_name ){ return authority( _db.get< account_authority_object, by_account >( account_name ).posting ); },
                                              AMALGAM_MAX_SIG_CHECK_DEPTH );
   return result;
}

set<public_key_type> database_api::get_potential_signatures( const signed_transaction& trx )const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->get_potential_signatures( trx );
   });
}

set<public_key_type> database_api_impl::get_potential_signatures( const signed_transaction& trx )const
{
   set<public_key_type> result;
   trx.get_required_signatures(
      AMALGAM_CHAIN_ID,
      flat_set<public_key_type>(),
      [&]( account_name_type account_name )
      {
         const auto& auth = _db.get< account_authority_object, by_account >(account_name).active;
         for( const auto& k : auth.get_keys() )
            result.insert(k);
         return authority( auth );
      },
      [&]( account_name_type account_name )
      {
         const auto& auth = _db.get< account_authority_object, by_account >(account_name).owner;
         for( const auto& k : auth.get_keys() )
            result.insert(k);
         return authority( auth );
      },
      [&]( account_name_type account_name )
      {
         const auto& auth = _db.get< account_authority_object, by_account >(account_name).posting;
         for( const auto& k : auth.get_keys() )
            result.insert(k);
         return authority( auth );
      },
      AMALGAM_MAX_SIG_CHECK_DEPTH
   );
   return result;
}

bool database_api::verify_authority( const signed_transaction& trx ) const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->verify_authority( trx );
   });
}

bool database_api_impl::verify_authority( const signed_transaction& trx )const
{
   trx.verify_authority( AMALGAM_CHAIN_ID,
                         [&]( string account_name ){ return authority( _db.get< account_authority_object, by_account >( account_name ).active  ); },
                         [&]( string account_name ){ return authority( _db.get< account_authority_object, by_account >( account_name ).owner   ); },
                         [&]( string account_name ){ return authority( _db.get< account_authority_object, by_account >( account_name ).posting ); },
                         AMALGAM_MAX_SIG_CHECK_DEPTH );
   return true;
}

bool database_api::verify_account_authority( const string& name_or_id, const flat_set<public_key_type>& signers )const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->verify_account_authority( name_or_id, signers );
   });
}

bool database_api_impl::verify_account_authority( const string& name, const flat_set<public_key_type>& keys )const
{
   FC_ASSERT( name.size() > 0);
   auto account = _db.find< account_object, by_name >( name );
   FC_ASSERT( account, "no such account" );

   /// reuse trx.verify_authority by creating a dummy transfer
   signed_transaction trx;
   transfer_operation op;
   op.from = account->name;
   trx.operations.emplace_back(op);

   return verify_authority( trx );
}

verify_signatures_return database_api_impl::verify_signatures( const verify_signatures_args& args )const
{
   // get_signature_keys can throw for dup sigs. Allow this to throw.
   flat_set< public_key_type > sig_keys;
   for( const auto&  sig : args.signatures )
   {
      AMALGAM_ASSERT(
         sig_keys.insert( fc::ecc::public_key(sig,args.hash) ).second,
         tx_duplicate_sig,
         "Duplicate Signature detected" );
   }

   verify_signatures_return result;
   result.valid = true;

   // verify authority throws on failure, catch and return false
   try
   {
      amalgam::protocol::verify_authority(
         [&args]( flat_set< account_name_type >& required_active,
                  flat_set< account_name_type >& required_owner,
                  flat_set< account_name_type >& required_posting,
                  vector< authority >& )
         {
            switch( args.auth_level )
            {
               case authority::owner:
                  std::copy( args.accounts.begin(), args.accounts.end(), required_owner.end() );
                  break;
               case authority::active:
                  std::copy( args.accounts.begin(), args.accounts.end(), required_active.end() );
                  break;
               case authority::posting:
                  std::copy( args.accounts.begin(), args.accounts.end(), required_posting.end() );
                  break;
               case authority::key:
               default:
                  FC_ASSERT( false, "verify_signatures only supports owner, active, and posting auths" );
            }
         },
         sig_keys,
         [this]( const string& name ) { return authority( _db.get< account_authority_object, by_account >( name ).owner ); },
         [this]( const string& name ) { return authority( _db.get< account_authority_object, by_account >( name ).active ); },
         [this]( const string& name ) { return authority( _db.get< account_authority_object, by_account >( name ).posting ); },
         AMALGAM_MAX_SIG_CHECK_DEPTH );
   }
   catch( fc::exception& ) { result.valid = false; }

   return result;
}

verify_signatures_return database_api::verify_signatures( const verify_signatures_args& args )const
{
   return my->_db.with_read_lock( [&]()
   {
      return my->verify_signatures( args );
   });
}

vector<convert_request_api_obj> database_api::get_conversion_requests( const string& account )const
{
   return my->_db.with_read_lock( [&]()
   {
      const auto& idx = my->_db.get_index< convert_request_index >().indices().get< by_owner >();
      vector< convert_request_api_obj > result;
      auto itr = idx.lower_bound(account);
      while( itr != idx.end() && itr->owner == account ) {
         result.push_back(*itr);
         ++itr;
      }
      return result;
   });
}

discussion database_api::get_content( string permlink )const
{
   return my->_db.with_read_lock( [&]()
   {
      const auto& by_permlink_idx = my->_db.get_index< comment_index >().indices().get< by_permlink >();
      auto itr = by_permlink_idx.find( permlink );
      if( itr != by_permlink_idx.end() )
      {
         discussion result(*itr);
         set_pending_payout(result);
         result.active_votes = get_active_votes( permlink );
         return result;
      }
      return discussion();
   });
}

vector<vote_state> database_api::get_active_votes( string permlink )const
{
   return my->_db.with_read_lock( [&]()
   {
      vector<vote_state> result;
      const auto& comment = my->_db.get_comment( permlink );
      const auto& idx = my->_db.get_index<comment_vote_index>().indices().get< by_comment_voter >();
      comment_id_type cid(comment.id);
      auto itr = idx.lower_bound( cid );
      while( itr != idx.end() && itr->comment == cid )
      {
         const auto& vo = my->_db.get(itr->voter);
         vote_state vstate;
         vstate.voter = vo.name;
         vstate.weight = itr->weight;
         vstate.rshares = itr->rshares;
         vstate.percent = itr->vote_percent;
         vstate.time = itr->last_update;

         result.push_back(vstate);
         ++itr;
      }
      return result;
   });
}

vector<account_vote> database_api::get_account_votes( string voter )const
{
   return my->_db.with_read_lock( [&]()
   {
      vector<account_vote> result;

      const auto& voter_acnt = my->_db.get_account(voter);
      const auto& idx = my->_db.get_index<comment_vote_index>().indices().get< by_voter_comment >();

      account_id_type aid(voter_acnt.id);
      auto itr = idx.lower_bound( aid );
      auto end = idx.upper_bound( aid );
      while( itr != end )
      {
         const auto& vo = my->_db.get(itr->comment);
         account_vote avote;
         avote.authorperm = vo.author + "/" + to_string( vo.permlink );
         avote.weight = itr->weight;
         avote.rshares = itr->rshares;
         avote.percent = itr->vote_percent;
         avote.time = itr->last_update;
         result.push_back(avote);
         ++itr;
      }
      return result;
   });
}

u256 to256( const fc::uint128& t )
{
   u256 result( t.high_bits() );
   result <<= 65;
   result += t.low_bits();
   return result;
}

void database_api::set_pending_payout( discussion& d )const
{
   const auto& hist = my->_db.get_feed_history();
   const auto& rf = my->_db.get_reward_fund( my->_db.get_comment( d.permlink ) );

   asset pot = rf.reward_balance;

   if( !hist.current_median_history.is_null() ) pot = pot * hist.current_median_history;

   u256 total_r2 = to256( rf.recent_claims );

   if( total_r2 > 0 )
   {
      uint128_t vshares = d.net_rshares.value > 0 ? amalgam::chain::util::evaluate_reward_curve( d.net_rshares.value, rf.author_reward_curve, rf.content_constant ) : 0;

      u256 r2 = to256(vshares);
      r2 = ( r2 * d.reward_weight ) / AMALGAM_100_PERCENT;
      r2 *= pot.amount.value;
      r2 /= total_r2;

      d.pending_payout_value = asset( static_cast<uint64_t>(r2), pot.symbol );
   }

   if( d.parent_author != AMALGAM_ROOT_POST_PARENT )
      d.cashout_time = my->_db.calculate_discussion_payout_time( my->_db.get< comment_object >( d.id ) );
}

vector<discussion> database_api::get_content_replies( string permlink )const
{
   return my->_db.with_read_lock( [&]()
   {
      const auto& by_parent_idx = my->_db.get_index< comment_index >().indices().get< by_parent >();
      auto itr = by_parent_idx.find( permlink );
      vector<discussion> result;
      while( itr != by_parent_idx.end() && to_string( itr->parent_permlink ) == permlink )
      {
         result.push_back( discussion( *itr ) );
         set_pending_payout( result.back() );
         ++itr;
      }
      return result;
   });
}

map< uint32_t, applied_operation > database_api::get_account_history( string account, uint64_t from, uint32_t limit )const
{
   FC_ASSERT(limit <= 10000, "Limit of ${l} is greater than maximum allowed", ("l", limit));
   FC_ASSERT(from >= limit, "From must be greater than limit");
   return my->_db.with_read_lock( [&]()
   {
      const auto& idx = my->_db.get_index<account_history_index>().indices().get<by_account>();
      auto itr = idx.lower_bound( boost::make_tuple( account, from ) );
      uint32_t n = 0;

      map<uint32_t, applied_operation> result;
      while( true )
      {
         if( itr == idx.end() )
            break;
         if( itr->account != account )
            break;
         if( n >= limit )
            break;
         result[itr->sequence] = my->_db.get(itr->op);
         ++itr;
         ++n;
      }
      return result;
   });
}

vector<account_name_type> database_api::get_miner_queue()const
{
   return my->_db.with_read_lock( [&]()
   {
      vector<account_name_type> result;
      const auto& pow_idx = my->_db.get_index<witness_index>().indices().get<by_pow>();

      auto itr = pow_idx.upper_bound(0);
      while( itr != pow_idx.end() ) {
         if( itr->pow_worker )
            result.push_back( itr->owner );
         ++itr;
      }
      return result;
   });
}

vector< account_name_type > database_api::get_active_witnesses()const
{
   return my->_db.with_read_lock( [&]()
   {
      const auto& wso = my->_db.get_witness_schedule_object();
      size_t n = wso.current_shuffled_witnesses.size();
      vector< account_name_type > result;
      result.reserve( n );
      for( size_t i=0; i<n; i++ )
         result.push_back( wso.current_shuffled_witnesses[i] );
      return result;
   });
}

vector< savings_withdraw_api_obj > database_api::get_savings_withdraw_from( string account )const
{
   return my->_db.with_read_lock( [&]()
   {
      vector<savings_withdraw_api_obj> result;

      const auto& from_rid_idx = my->_db.get_index< savings_withdraw_index >().indices().get< by_from_rid >();
      auto itr = from_rid_idx.lower_bound( account );
      while( itr != from_rid_idx.end() && itr->from == account ) {
         result.push_back( savings_withdraw_api_obj( *itr ) );
         ++itr;
      }
      return result;
   });
}
vector< savings_withdraw_api_obj > database_api::get_savings_withdraw_to( string account )const
{
   return my->_db.with_read_lock( [&]()
   {
      vector<savings_withdraw_api_obj> result;

      const auto& to_complete_idx = my->_db.get_index< savings_withdraw_index >().indices().get< by_to_complete >();
      auto itr = to_complete_idx.lower_bound( account );
      while( itr != to_complete_idx.end() && itr->to == account ) {
         result.push_back( savings_withdraw_api_obj( *itr ) );
         ++itr;
      }
      return result;
   });
}

vector< vesting_delegation_api_obj > database_api::get_vesting_delegations( string account, string from, uint32_t limit )const
{
   FC_ASSERT( limit <= 1000 );

   return my->_db.with_read_lock( [&]()
   {
      vector< vesting_delegation_api_obj > result;
      result.reserve( limit );

      const auto& delegation_idx = my->_db.get_index< vesting_delegation_index, by_delegation >();
      auto itr = delegation_idx.lower_bound( boost::make_tuple( account, from ) );
      while( result.size() < limit && itr != delegation_idx.end() && itr->delegator == account )
      {
         result.push_back( *itr );
         ++itr;
      }

      return result;
   });
}

vector< vesting_delegation_expiration_api_obj > database_api::get_expiring_vesting_delegations( string account, time_point_sec from, uint32_t limit )const
{
   FC_ASSERT( limit <= 1000 );

   return my->_db.with_read_lock( [&]()
   {
      vector< vesting_delegation_expiration_api_obj > result;
      result.reserve( limit );

      const auto& exp_idx = my->_db.get_index< vesting_delegation_expiration_index, by_account_expiration >();
      auto itr = exp_idx.lower_bound( boost::make_tuple( account, from ) );
      while( result.size() < limit && itr != exp_idx.end() && itr->delegator == account )
      {
         result.push_back( *itr );
         ++itr;
      }

      return result;
   });
}

annotated_signed_transaction database_api::get_transaction( transaction_id_type id )const
{
#ifdef SKIP_BY_TX_ID
   FC_ASSERT( false, "This node's operator has disabled operation indexing by transaction_id" );
#else
   return my->_db.with_read_lock( [&](){
      const auto& idx = my->_db.get_index<operation_index>().indices().get<by_transaction_id>();
      auto itr = idx.lower_bound( id );
      if( itr != idx.end() && itr->trx_id == id ) {
         auto blk = my->_db.fetch_block_by_number( itr->block );
         FC_ASSERT( blk.valid() );
         FC_ASSERT( blk->transactions.size() > itr->trx_in_block );
         annotated_signed_transaction result = blk->transactions[itr->trx_in_block];
         result.block_num       = itr->block;
         result.transaction_num = itr->trx_in_block;
         return result;
      }
      FC_ASSERT( false, "Unknown Transaction ${t}", ("t",id));
   });
#endif
}

} } // amalgam::app
