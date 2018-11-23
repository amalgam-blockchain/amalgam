#include <appbase/application.hpp>

#include <amalgam/plugins/database_api/database_api.hpp>
#include <amalgam/plugins/database_api/database_api_plugin.hpp>

#include <amalgam/protocol/get_config.hpp>
#include <amalgam/protocol/exceptions.hpp>
#include <amalgam/protocol/transaction_util.hpp>

#include <amalgam/utilities/git_revision.hpp>

#include <fc/git_revision.hpp>

namespace amalgam { namespace plugins { namespace database_api {

class database_api_impl
{
   public:
      database_api_impl();
      ~database_api_impl();

      DECLARE_API_IMPL
      (
         (get_block_header)
         (get_block)
         (get_ops_in_block)
         (get_transaction)
         (get_config)
         (get_version)
         (get_dynamic_global_properties)
         (get_chain_properties)
         (get_witness_schedule)
         (get_reserve_ratio)
         (get_hardfork_properties)
         (get_current_price_feed)
         (get_feed_history)
         (list_witnesses)
         (find_witnesses)
         (list_witness_votes)
         (get_witness_votes_by_account)
         (get_witness_votes_by_witness)
         (get_witnesses_by_vote)
         (get_witness_count)
         (get_active_witnesses)
         (list_accounts)
         (find_accounts)
         (get_account_count)
         (get_account_history)
         (get_account_bandwidth)
         (list_owner_histories)
         (find_owner_histories)
         (list_account_recovery_requests)
         (find_account_recovery_requests)
         (list_change_recovery_account_requests)
         (find_change_recovery_account_requests)
         (list_escrows)
         (find_escrows)
         (get_escrow)
         (list_withdraw_vesting_routes)
         (find_withdraw_vesting_routes)
         (list_savings_withdrawals)
         (find_savings_withdrawals_from)
         (find_savings_withdrawals_to)
         (list_vesting_delegations)
         (find_vesting_delegations)
         (list_vesting_delegation_expirations)
         (find_vesting_delegation_expirations)
         (list_abd_conversion_requests)
         (find_abd_conversion_requests)
         (list_decline_voting_rights_requests)
         (find_decline_voting_rights_requests)
         (list_limit_orders)
         (find_limit_orders)
         (get_transaction_hex)
         (get_required_signatures)
         (get_potential_signatures)
         (verify_authority)
         (verify_account_authority)
         (verify_signatures)
      )

      template< typename ResultType >
      static ResultType on_push_default( const ResultType& r ) { return r; }

      template< typename IndexType, typename OrderType, typename ValueType, typename ResultType, typename OnPush >
      void iterate_results( ValueType start, vector< ResultType >& result, uint32_t limit, OnPush&& on_push = &database_api_impl::on_push_default< ResultType > )
      {
         const auto& idx = _db.get_index< IndexType, OrderType >();
         auto itr = idx.lower_bound( start );
         auto end = idx.end();

         while( result.size() < limit && itr != end )
         {
            result.push_back( on_push( *itr ) );
            ++itr;
         }
      }

      chain::database& _db;
};

//////////////////////////////////////////////////////////////////////
//                                                                  //
// Constructors                                                     //
//                                                                  //
//////////////////////////////////////////////////////////////////////

database_api::database_api()
   : my( new database_api_impl() )
{
   JSON_RPC_REGISTER_API( AMALGAM_DATABASE_API_PLUGIN_NAME );
}

database_api::~database_api() {}

database_api_impl::database_api_impl()
   : _db( appbase::app().get_plugin< amalgam::plugins::chain::chain_plugin >().db() ) {}

database_api_impl::~database_api_impl() {}

//////////////////////////////////////////////////////////////////////
//                                                                  //
// Blocks and transactions                                          //
//                                                                  //
//////////////////////////////////////////////////////////////////////

DEFINE_API_IMPL( database_api_impl, get_block_header )
{
   auto result = _db.fetch_block_by_number( args.block_num );
   if( result )
      return *result;
   return {};
}

DEFINE_API_IMPL( database_api_impl, get_block )
{
   auto result = _db.fetch_block_by_number( args.block_num );
   if( result )
      return *result;
   return {};
}

DEFINE_API_IMPL( database_api_impl, get_ops_in_block )
{
   FC_ASSERT( appbase::app().find_plugin< amalgam::plugins::account_history::account_history_plugin >() != nullptr,
      "This function works only if account_history plugin is enabled" );
   return _db.with_read_lock( [&]()
   {
      const auto& idx = _db.get_index< chain::operation_index, chain::by_location >();
      auto itr = idx.lower_bound( args.block_num );

      get_ops_in_block_return result;

      while( itr != idx.end() && itr->block == args.block_num )
      {
         api_operation_object temp = *itr;
         if( !args.only_virtual || is_virtual_operation( temp.op ) )
            result.emplace( std::move( temp ) );
         ++itr;
      }

      return result;
   });
}

DEFINE_API_IMPL( database_api_impl, get_transaction )
{
   FC_ASSERT( appbase::app().find_plugin< amalgam::plugins::account_history::account_history_plugin >() != nullptr,
      "This function works only if account_history plugin is enabled" );

#ifdef SKIP_BY_TX_ID
   FC_ASSERT( false, "This node's operator has disabled operation indexing by transaction_id" );
#else

   return _db.with_read_lock( [&]()
   {
      get_transaction_return result;

      const auto& idx = _db.get_index< chain::operation_index, chain::by_transaction_id >();
      auto itr = idx.lower_bound( args.id );
      if( itr != idx.end() && itr->trx_id == args.id )
      {
         auto blk = _db.fetch_block_by_number( itr->block );
         FC_ASSERT( blk.valid() );
         FC_ASSERT( blk->transactions.size() > itr->trx_in_block );
         result = blk->transactions[itr->trx_in_block];
         result.block_num       = itr->block;
         result.transaction_num = itr->trx_in_block;
      }
      else
      {
         FC_ASSERT( false, "Unknown Transaction ${t}", ("t",args.id) );
      }

      return result;
   });
#endif
}

//////////////////////////////////////////////////////////////////////
//                                                                  //
// Globals                                                          //
//                                                                  //
//////////////////////////////////////////////////////////////////////


DEFINE_API_IMPL( database_api_impl, get_config )
{
   return amalgam::protocol::get_config();
}

DEFINE_API_IMPL( database_api_impl, get_version )
{
   return get_version_return
   (
      fc::string( AMALGAM_BLOCKCHAIN_VERSION ),
      fc::string( amalgam::utilities::git_revision_sha ),
      fc::string( fc::git_revision_sha ),
      _db.get_chain_id()
   );
}

DEFINE_API_IMPL( database_api_impl, get_dynamic_global_properties )
{
   return _db.get_dynamic_global_properties();
}

DEFINE_API_IMPL( database_api_impl, get_chain_properties )
{
   return _db.get_witness_schedule_object().median_props;
}

DEFINE_API_IMPL( database_api_impl, get_witness_schedule )
{
   return api_witness_schedule_object( _db.get_witness_schedule_object() );
}

DEFINE_API_IMPL( database_api_impl, get_reserve_ratio )
{
   return _db.get( reserve_ratio_id_type() );
}

DEFINE_API_IMPL( database_api_impl, get_hardfork_properties )
{
   return _db.get_hardfork_property_object();
}

DEFINE_API_IMPL( database_api_impl, get_current_price_feed )
{
   return _db.get_feed_history().current_median_history;
}

DEFINE_API_IMPL( database_api_impl, get_feed_history )
{
   return _db.get_feed_history();
}


//////////////////////////////////////////////////////////////////////
//                                                                  //
// Witnesses                                                        //
//                                                                  //
//////////////////////////////////////////////////////////////////////

DEFINE_API_IMPL( database_api_impl, list_witnesses )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_witnesses_return result;
   result.reserve( args.limit );

   switch( args.order )
   {
      case( by_name ):
      {
         iterate_results< chain::witness_index, chain::by_name >(
            args.start.as< protocol::account_name_type >(),
            result,
            args.limit,
            [&]( const witness_object& w ){ return api_witness_object( w ); } );
         break;
      }
      case( by_vote_name ):
      {
         auto key = args.start.as< std::pair< share_type, account_name_type > >();
         iterate_results< chain::witness_index, chain::by_vote_name >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            [&]( const witness_object& w ){ return api_witness_object( w ); } );
         break;
      }
      case( by_schedule_time ):
      {
         auto key = args.start.as< std::pair< fc::uint128, account_name_type > >();
         auto wit_id = _db.get< chain::witness_object, chain::by_name >( key.second ).id;
         iterate_results< chain::witness_index, chain::by_schedule_time >(
            boost::make_tuple( key.first, wit_id ),
            result,
            args.limit,
            [&]( const witness_object& w ){ return api_witness_object( w ); } );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_witnesses )
{
   FC_ASSERT( args.owners.size() <= DATABASE_API_SINGLE_QUERY_LIMIT );

   find_witnesses_return result;

   for( auto& o : args.owners )
   {
      auto witness = _db.find< chain::witness_object, chain::by_name >( o );

      if( witness != nullptr )
         result.push_back( api_witness_object( *witness ) );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, list_witness_votes )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_witness_votes_return result;
   result.reserve( args.limit );

   switch( args.order )
   {
      case( by_account_witness ):
      {
         auto key = args.start.as< std::pair< account_name_type, account_name_type > >();
         iterate_results< chain::witness_vote_index, chain::by_account_witness >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            [&]( const witness_vote_object& v ){ return api_witness_vote_object( v ); } );
         break;
      }
      case( by_witness_account ):
      {
         auto key = args.start.as< std::pair< account_name_type, account_name_type > >();
         iterate_results< chain::witness_vote_index, chain::by_witness_account >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            [&]( const witness_vote_object& v ){ return api_witness_vote_object( v ); } );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, get_witness_votes_by_account )
{
   get_witness_votes_by_account_return result;

   const auto& vote_idx = _db.get_index< chain::witness_vote_index, chain::by_account_witness >();
   auto itr = vote_idx.lower_bound( boost::make_tuple( args.account, account_name_type() ) );

   while( itr != vote_idx.end() && itr->account == args.account && result.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
   {
      result.push_back( api_witness_vote_object( *itr ) );
      ++itr;
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, get_witness_votes_by_witness )
{
   get_witness_votes_by_witness_return result;

   const auto& vote_idx = _db.get_index< chain::witness_vote_index, chain::by_witness_account >();
   auto itr = vote_idx.lower_bound( boost::make_tuple( args.account, account_name_type() ) );

   while( itr != vote_idx.end() && itr->witness == args.account && result.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
   {
      result.push_back( api_witness_vote_object( *itr ) );
      ++itr;
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, get_witnesses_by_vote )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   account_name_type start_name = args.account;
   vector< fc::variant > start_key;

   if( start_name == account_name_type() )
   {
      start_key.push_back( fc::variant( std::numeric_limits< int64_t >::max() ) );
      start_key.push_back( fc::variant( account_name_type() ) );
   }
   else
   {
      auto start = list_witnesses( { fc::variant( start_name ), 1, by_name } );

      if( start.size() == 0 )
         return get_witnesses_by_vote_return();

      start_key.push_back( fc::variant( start[0].votes ) );
      start_key.push_back( fc::variant( start[0].owner ) );
   }

   auto witnesses = list_witnesses( { fc::variant( start_key ), args.limit, by_vote_name } );

   get_witnesses_by_vote_return result;

   for( auto& w : witnesses )
   {
      result.push_back( api_witness_object( w ) );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, get_witness_count )
{
   return _db.get_index< chain::witness_index >().indices().size();
}

DEFINE_API_IMPL( database_api_impl, get_active_witnesses )
{
   const auto& wso = _db.get_witness_schedule_object();
   size_t n = wso.current_shuffled_witnesses.size();
   get_active_witnesses_return result;
   result.reserve( n );
   for( size_t i=0; i<n; i++ )
      result.push_back( wso.current_shuffled_witnesses[i] );
   return result;
}


//////////////////////////////////////////////////////////////////////
//                                                                  //
// Accounts                                                         //
//                                                                  //
//////////////////////////////////////////////////////////////////////

/* Accounts */

DEFINE_API_IMPL( database_api_impl, list_accounts )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_accounts_return result;
   result.reserve( args.limit );

   switch( args.order )
   {
      case( by_name ):
      {
         iterate_results< chain::account_index, chain::by_name >(
            args.start.as< protocol::account_name_type >(),
            result,
            args.limit,
            [&]( const account_object& a ){ return api_account_object( a, _db ); } );
         break;
      }
      case( by_proxy ):
      {
         auto key = args.start.as< std::pair< account_name_type, account_name_type > >();
         iterate_results< chain::account_index, chain::by_proxy >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            [&]( const account_object& a ){ return api_account_object( a, _db ); } );
         break;
      }
      case( by_next_vesting_withdrawal ):
      {
         auto key = args.start.as< std::pair< fc::time_point_sec, account_name_type > >();
         iterate_results< chain::account_index, chain::by_next_vesting_withdrawal >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            [&]( const account_object& a ){ return api_account_object( a, _db ); } );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_accounts )
{
   find_accounts_return result;
   FC_ASSERT( args.accounts.size() <= DATABASE_API_SINGLE_QUERY_LIMIT );

   for( auto& a : args.accounts )
   {
      auto acct = _db.find< chain::account_object, chain::by_name >( a );
      if( acct != nullptr )
         result.push_back( api_account_object( *acct, _db ) );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, get_account_count )
{
   return _db.get_index< chain::account_index >().indices().size();
}

DEFINE_API_IMPL( database_api_impl, get_account_history )
{
   FC_ASSERT( appbase::app().find_plugin< amalgam::plugins::account_history::account_history_plugin >() != nullptr,
      "This function works only if account_history plugin is enabled" );
   FC_ASSERT( args.limit <= 10000, "limit of ${l} is greater than maximum allowed", ("l",args.limit) );
   FC_ASSERT( args.start >= args.limit, "start must be greater than limit" );

   return _db.with_read_lock( [&]()
   {
      const auto& idx = _db.get_index< chain::account_history_index, chain::by_account >();
      auto itr = idx.lower_bound( boost::make_tuple( args.account, args.start ) );
      uint32_t n = 0;

      get_account_history_return result;
      while( true )
      {
         if( itr == idx.end() )
            break;
         if( itr->account != args.account )
            break;
         if( n >= args.limit )
            break;
         result[ itr->sequence ] = _db.get( itr->op );
         ++itr;
         ++n;
      }

      return result;
   });
}

DEFINE_API_IMPL( database_api_impl, get_account_bandwidth )
{
   get_account_bandwidth_return result;

   auto band = _db.find< witness::account_bandwidth_object, witness::by_account_bandwidth_type >( boost::make_tuple( args.account, args.type ) );
   if( band != nullptr )
      result = *band;

   return result;
}


/* Owner Auth Histories */

DEFINE_API_IMPL( database_api_impl, list_owner_histories )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_owner_histories_return result;
   result.reserve( args.limit );

   auto key = args.start.as< std::pair< account_name_type, fc::time_point_sec > >();
   iterate_results< chain::owner_authority_history_index, chain::by_account >(
      boost::make_tuple( key.first, key.second ),
      result,
      args.limit,
      [&]( const owner_authority_history_object& o ){ return api_owner_authority_history_object( o ); } );

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_owner_histories )
{
   find_owner_histories_return result;

   const auto& hist_idx = _db.get_index< chain::owner_authority_history_index, chain::by_account >();
   auto itr = hist_idx.lower_bound( args.owner );

   while( itr != hist_idx.end() && itr->account == args.owner && result.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
   {
      result.push_back( api_owner_authority_history_object( *itr ) );
      ++itr;
   }

   return result;
}


/* Account Recovery Requests */

DEFINE_API_IMPL( database_api_impl, list_account_recovery_requests )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_account_recovery_requests_return result;
   result.reserve( args.limit );

   switch( args.order )
   {
      case( by_account ):
      {
         iterate_results< chain::account_recovery_request_index, chain::by_account >(
            args.start.as< account_name_type >(),
            result,
            args.limit,
            [&]( const account_recovery_request_object& a ){ return api_account_recovery_request_object( a ); } );
         break;
      }
      case( by_expiration ):
      {
         auto key = args.start.as< std::pair< fc::time_point_sec, account_name_type > >();
         iterate_results< chain::account_recovery_request_index, chain::by_expiration >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            [&]( const account_recovery_request_object& a ){ return api_account_recovery_request_object( a ); } );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_account_recovery_requests )
{
   find_account_recovery_requests_return result;
   FC_ASSERT( args.accounts.size() <= DATABASE_API_SINGLE_QUERY_LIMIT );

   for( auto& a : args.accounts )
   {
      auto request = _db.find< chain::account_recovery_request_object, chain::by_account >( a );

      if( request != nullptr )
         result.push_back( api_account_recovery_request_object( *request ) );
   }

   return result;
}


/* Change Recovery Account Requests */

DEFINE_API_IMPL( database_api_impl, list_change_recovery_account_requests )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_change_recovery_account_requests_return result;
   result.reserve( args.limit );

   switch( args.order )
   {
      case( by_account ):
      {
         iterate_results< chain::change_recovery_account_request_index, chain::by_account >(
            args.start.as< account_name_type >(),
            result,
            args.limit,
            &database_api_impl::on_push_default< change_recovery_account_request_object > );
         break;
      }
      case( by_effective_date ):
      {
         auto key = args.start.as< std::pair< fc::time_point_sec, account_name_type > >();
         iterate_results< chain::change_recovery_account_request_index, chain::by_effective_date >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            &database_api_impl::on_push_default< change_recovery_account_request_object > );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_change_recovery_account_requests )
{
   find_change_recovery_account_requests_return result;
   FC_ASSERT( args.accounts.size() <= DATABASE_API_SINGLE_QUERY_LIMIT );

   for( auto& a : args.accounts )
   {
      auto request = _db.find< chain::change_recovery_account_request_object, chain::by_account >( a );

      if( request != nullptr )
         result.push_back( *request );
   }

   return result;
}


/* Escrows */

DEFINE_API_IMPL( database_api_impl, list_escrows )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_escrows_return result;
   result.reserve( args.limit );

   switch( args.order )
   {
      case( by_from_id ):
      {
         auto key = args.start.as< std::pair< account_name_type, uint32_t > >();
         iterate_results< chain::escrow_index, chain::by_from_id >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            &database_api_impl::on_push_default< escrow_object > );
         break;
      }
      case( by_ratification_deadline ):
      {
         auto key = args.start.as< std::vector< fc::variant > >();
         FC_ASSERT( key.size() == 3, "by_ratification_deadline start requires 3 values. (bool, time_point_sec, escrow_id_type)" );
         iterate_results< chain::escrow_index, chain::by_ratification_deadline >(
            boost::make_tuple( key[0].as< bool >(), key[1].as< fc::time_point_sec >(), key[2].as< escrow_id_type >() ),
            result,
            args.limit,
            &database_api_impl::on_push_default< escrow_object > );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_escrows )
{
   find_escrows_return result;

   const auto& escrow_idx = _db.get_index< chain::escrow_index, chain::by_from_id >();
   auto itr = escrow_idx.lower_bound( args.from );

   while( itr != escrow_idx.end() && itr->from == args.from && result.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
   {
      result.push_back( *itr );
      ++itr;
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, get_escrow )
{
   get_escrow_return result;

   auto escrows = list_escrows( { fc::variant( std::pair< account_name_type, uint32_t >( args.from, args.escrow_id ) ), 1, by_from_id } );

   if( escrows.size()
      && escrows[0].from == args.from
      && escrows[0].escrow_id == args.escrow_id )
   {
      result = escrows[0];
   }

   return result;
}


/* Withdraw Vesting Routes */

DEFINE_API_IMPL( database_api_impl, list_withdraw_vesting_routes )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_withdraw_vesting_routes_return result;
   result.reserve( args.limit );

   switch( args.order )
   {
      case( by_withdraw_route ):
      {
         auto key = args.start.as< std::pair< account_name_type, account_name_type > >();
         iterate_results< chain::withdraw_vesting_route_index, chain::by_withdraw_route >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            &database_api_impl::on_push_default< withdraw_vesting_route_object > );
         break;
      }
      case( by_destination ):
      {
         auto key = args.start.as< std::pair< account_name_type, withdraw_vesting_route_id_type > >();
         iterate_results< chain::withdraw_vesting_route_index, chain::by_destination >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            &database_api_impl::on_push_default< withdraw_vesting_route_object > );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_withdraw_vesting_routes )
{
   find_withdraw_vesting_routes_return result;

   switch( args.order )
   {
      case( by_withdraw_route ):
      {
         const auto& route_idx = _db.get_index< chain::withdraw_vesting_route_index, chain::by_withdraw_route >();
         auto itr = route_idx.lower_bound( args.account );

         while( itr != route_idx.end() && itr->from_account == args.account && result.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
         {
            result.push_back( *itr );
            ++itr;
         }

         break;
      }
      case( by_destination ):
      {
         const auto& route_idx = _db.get_index< chain::withdraw_vesting_route_index, chain::by_destination >();
         auto itr = route_idx.lower_bound( args.account );

         while( itr != route_idx.end() && itr->to_account == args.account && result.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
         {
            result.push_back( *itr );
            ++itr;
         }

         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}


/* Savings Withdrawals */

DEFINE_API_IMPL( database_api_impl, list_savings_withdrawals )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_savings_withdrawals_return result;
   result.reserve( args.limit );

   switch( args.order )
   {
      case( by_from_id ):
      {
         auto key = args.start.as< std::pair< account_name_type, uint32_t > >();
         iterate_results< chain::savings_withdraw_index, chain::by_from_rid >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            [&]( const savings_withdraw_object& w ){ return api_savings_withdraw_object( w ); } );
         break;
      }
      case( by_complete_from_id ):
      {
         auto key = args.start.as< std::vector< fc::variant > >();
         FC_ASSERT( key.size() == 3, "by_complete_from_id start requires 3 values. (time_point_sec, account_name_type, uint32_t)" );
         iterate_results< chain::savings_withdraw_index, chain::by_complete_from_rid >(
            boost::make_tuple( key[0].as< fc::time_point_sec >(), key[1].as< account_name_type >(), key[2].as< uint32_t >() ),
            result,
            args.limit,
            [&]( const savings_withdraw_object& w ){ return api_savings_withdraw_object( w ); } );
         break;
      }
      case( by_to_complete ):
      {
         auto key = args.start.as< std::vector< fc::variant > >();
         FC_ASSERT( key.size() == 3, "by_to_complete start requires 3 values. (account_name_type, time_point_sec, savings_withdraw_id_type" );
         iterate_results< chain::savings_withdraw_index, chain::by_to_complete >(
            boost::make_tuple( key[0].as< account_name_type >(), key[1].as< fc::time_point_sec >(), key[2].as< savings_withdraw_id_type >() ),
            result,
            args.limit,
            [&]( const savings_withdraw_object& w ){ return api_savings_withdraw_object( w ); } );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_savings_withdrawals_from )
{
   find_savings_withdrawals_from_return result;
   const auto& withdraw_idx = _db.get_index< chain::savings_withdraw_index, chain::by_from_rid >();
   auto itr = withdraw_idx.lower_bound( args.account );

   while( itr != withdraw_idx.end() && itr->from == args.account && result.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
   {
      result.push_back( api_savings_withdraw_object( *itr ) );
      ++itr;
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_savings_withdrawals_to )
{
   find_savings_withdrawals_to_return result;
   const auto& withdraw_idx = _db.get_index< chain::savings_withdraw_index, chain::by_to_complete >();
   auto itr = withdraw_idx.lower_bound( args.account );

   while( itr != withdraw_idx.end() && itr->to == args.account && result.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
   {
      result.push_back( api_savings_withdraw_object( *itr ) );
      ++itr;
   }

   return result;
}


/* Vesting Delegations */

DEFINE_API_IMPL( database_api_impl, list_vesting_delegations )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_vesting_delegations_return result;
   result.reserve( args.limit );

   switch( args.order )
   {
      case( by_delegation ):
      {
         auto key = args.start.as< std::pair< account_name_type, account_name_type > >();
         iterate_results< chain::vesting_delegation_index, chain::by_delegation >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            &database_api_impl::on_push_default< api_vesting_delegation_object > );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_vesting_delegations )
{
   find_vesting_delegations_return result;
   const auto& delegation_idx = _db.get_index< chain::vesting_delegation_index, chain::by_delegation >();
   auto itr = delegation_idx.lower_bound( args.account );

   while( itr != delegation_idx.end() && itr->delegator == args.account && result.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
   {
      result.push_back( api_vesting_delegation_object( *itr ) );
      ++itr;
   }

   return result;
}


/* Vesting Delegation Expirations */

DEFINE_API_IMPL( database_api_impl, list_vesting_delegation_expirations )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_vesting_delegation_expirations_return result;
   result.reserve( args.limit );

   switch( args.order )
   {
      case( by_expiration ):
      {
         auto key = args.start.as< std::pair< time_point_sec, vesting_delegation_expiration_id_type > >();
         iterate_results< chain::vesting_delegation_expiration_index, chain::by_expiration >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            &database_api_impl::on_push_default< api_vesting_delegation_expiration_object > );
         break;
      }
      case( by_account_expiration ):
      {
         auto key = args.start.as< std::vector< fc::variant > >();
         FC_ASSERT( key.size() == 3, "by_account_expiration start requires 3 values. (account_name_type, time_point_sec, vesting_delegation_expiration_id_type" );
         iterate_results< chain::vesting_delegation_expiration_index, chain::by_account_expiration >(
            boost::make_tuple( key[0].as< account_name_type >(), key[1].as< time_point_sec >(), key[2].as< vesting_delegation_expiration_id_type >() ),
            result,
            args.limit,
            &database_api_impl::on_push_default< api_vesting_delegation_expiration_object > );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_vesting_delegation_expirations )
{
   find_vesting_delegation_expirations_return result;
   const auto& del_exp_idx = _db.get_index< chain::vesting_delegation_expiration_index, chain::by_account_expiration >();
   auto itr = del_exp_idx.lower_bound( args.account );

   while( itr != del_exp_idx.end() && itr->delegator == args.account && result.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
   {
      result.push_back( *itr );
      ++itr;
   }

   return result;
}


/* ABD Conversion Requests */

DEFINE_API_IMPL( database_api_impl, list_abd_conversion_requests )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_abd_conversion_requests_return result;
   result.reserve( args.limit );

   switch( args.order )
   {
      case( by_conversion_date ):
      {
         auto key = args.start.as< std::pair< time_point_sec, convert_request_id_type > >();
         iterate_results< chain::convert_request_index, chain::by_conversion_date >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            &database_api_impl::on_push_default< api_convert_request_object > );
         break;
      }
      case( by_account ):
      {
         auto key = args.start.as< std::pair< account_name_type, uint32_t > >();
         iterate_results< chain::convert_request_index, chain::by_owner >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            &database_api_impl::on_push_default< api_convert_request_object > );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_abd_conversion_requests )
{
   find_abd_conversion_requests_return result;
   const auto& convert_idx = _db.get_index< chain::convert_request_index, chain::by_owner >();
   auto itr = convert_idx.lower_bound( args.account );

   while( itr != convert_idx.end() && itr->owner == args.account && result.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
   {
      result.push_back( *itr );
      ++itr;
   }

   return result;
}


/* Decline Voting Rights Requests */

DEFINE_API_IMPL( database_api_impl, list_decline_voting_rights_requests )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_decline_voting_rights_requests_return result;
   result.reserve( args.limit );

   switch( args.order )
   {
      case( by_account ):
      {
         iterate_results< chain::decline_voting_rights_request_index, chain::by_account >(
            args.start.as< account_name_type >(),
            result,
            args.limit,
            &database_api_impl::on_push_default< api_decline_voting_rights_request_object > );
         break;
      }
      case( by_effective_date ):
      {
         auto key = args.start.as< std::pair< time_point_sec, account_name_type > >();
         iterate_results< chain::decline_voting_rights_request_index, chain::by_effective_date >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            &database_api_impl::on_push_default< api_decline_voting_rights_request_object > );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_decline_voting_rights_requests )
{
   FC_ASSERT( args.accounts.size() <= DATABASE_API_SINGLE_QUERY_LIMIT );
   find_decline_voting_rights_requests_return result;

   for( auto& a : args.accounts )
   {
      auto request = _db.find< chain::decline_voting_rights_request_object, chain::by_account >( a );

      if( request != nullptr )
         result.push_back( *request );
   }

   return result;
}


//////////////////////////////////////////////////////////////////////
//                                                                  //
// Market                                                           //
//                                                                  //
//////////////////////////////////////////////////////////////////////

/* Limit Orders */

DEFINE_API_IMPL( database_api_impl, list_limit_orders )
{
   FC_ASSERT( args.limit <= DATABASE_API_SINGLE_QUERY_LIMIT );

   list_limit_orders_return result;
   result.reserve( args.limit );

   switch( args.order )
   {
      case( by_price ):
      {
         auto key = args.start.as< std::pair< price, limit_order_id_type > >();
         iterate_results< chain::limit_order_index, chain::by_price >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            &database_api_impl::on_push_default< api_limit_order_object > );
         break;
      }
      case( by_account ):
      {
         auto key = args.start.as< std::pair< account_name_type, uint32_t > >();
         iterate_results< chain::limit_order_index, chain::by_account >(
            boost::make_tuple( key.first, key.second ),
            result,
            args.limit,
            &database_api_impl::on_push_default< api_limit_order_object > );
         break;
      }
      default:
         FC_ASSERT( false, "Unknown or unsupported sort order" );
   }

   return result;
}

DEFINE_API_IMPL( database_api_impl, find_limit_orders )
{
   find_limit_orders_return result;
   const auto& order_idx = _db.get_index< chain::limit_order_index, chain::by_account >();
   auto itr = order_idx.lower_bound( args.account );

   while( itr != order_idx.end() && itr->seller == args.account && result.size() <= DATABASE_API_SINGLE_QUERY_LIMIT )
   {
      result.push_back( *itr );
      ++itr;
   }

   return result;
}


//////////////////////////////////////////////////////////////////////
//                                                                  //
// Authority / Validation                                           //
//                                                                  //
//////////////////////////////////////////////////////////////////////

DEFINE_API_IMPL( database_api_impl, get_transaction_hex )
{
   return get_transaction_hex_return( { fc::to_hex( fc::raw::pack_to_vector( args.trx ) ) } );
}

DEFINE_API_IMPL( database_api_impl, get_required_signatures )
{
   get_required_signatures_return result = args.trx.get_required_signatures( _db.get_chain_id(),
                                                   args.available_keys,
                                                   [&]( string account_name ){ return authority( _db.get< chain::account_authority_object, chain::by_account >( account_name ).active  ); },
                                                   [&]( string account_name ){ return authority( _db.get< chain::account_authority_object, chain::by_account >( account_name ).owner   ); },
                                                   [&]( string account_name ){ return authority( _db.get< chain::account_authority_object, chain::by_account >( account_name ).posting ); },
                                                   AMALGAM_MAX_SIG_CHECK_DEPTH,
                                                   fc::ecc::canonical_signature_type::bip_0062 );

   return result;
}

DEFINE_API_IMPL( database_api_impl, get_potential_signatures )
{
   get_potential_signatures_return result;
   args.trx.get_required_signatures(
      _db.get_chain_id(),
      flat_set< public_key_type >(),
      [&]( account_name_type account_name )
      {
         const auto& auth = _db.get< chain::account_authority_object, chain::by_account >( account_name ).active;
         for( const auto& k : auth.get_keys() )
            result.insert( k );
         return authority( auth );
      },
      [&]( account_name_type account_name )
      {
         const auto& auth = _db.get< chain::account_authority_object, chain::by_account >( account_name ).owner;
         for( const auto& k : auth.get_keys() )
            result.insert( k );
         return authority( auth );
      },
      [&]( account_name_type account_name )
      {
         const auto& auth = _db.get< chain::account_authority_object, chain::by_account >( account_name ).posting;
         for( const auto& k : auth.get_keys() )
            result.insert( k );
         return authority( auth );
      },
      AMALGAM_MAX_SIG_CHECK_DEPTH,
      fc::ecc::canonical_signature_type::bip_0062
   );

   return result;
}

DEFINE_API_IMPL( database_api_impl, verify_authority )
{
   args.trx.verify_authority(_db.get_chain_id(),
                           [&]( string account_name ){ return authority( _db.get< chain::account_authority_object, chain::by_account >( account_name ).active  ); },
                           [&]( string account_name ){ return authority( _db.get< chain::account_authority_object, chain::by_account >( account_name ).owner   ); },
                           [&]( string account_name ){ return authority( _db.get< chain::account_authority_object, chain::by_account >( account_name ).posting ); },
                           AMALGAM_MAX_SIG_CHECK_DEPTH,
                           AMALGAM_MAX_AUTHORITY_MEMBERSHIP,
                           AMALGAM_MAX_SIG_CHECK_ACCOUNTS,
                           fc::ecc::canonical_signature_type::bip_0062 );
   return true;
}

// TODO: This is broken. By the look of is, it has been since BitShares. verify_authority always
// returns false because the TX is not signed.
DEFINE_API_IMPL( database_api_impl, verify_account_authority )
{
   auto account = _db.find< chain::account_object, chain::by_name >( args.account );
   FC_ASSERT( account != nullptr, "no such account" );

   /// reuse trx.verify_authority by creating a dummy transfer
   verify_authority_args vap;
   transfer_operation op;
   op.from = account->name;
   vap.trx.operations.emplace_back( op );

   return verify_authority( vap );
}

DEFINE_API_IMPL( database_api_impl, verify_signatures )
{
   // get_signature_keys can throw for dup sigs. Allow this to throw.
   flat_set< public_key_type > sig_keys;
   for( const auto&  sig : args.signatures )
   {
      AMALGAM_ASSERT(
         sig_keys.insert( fc::ecc::public_key( sig, args.hash ) ).second,
         protocol::tx_duplicate_sig,
         "Duplicate Signature detected" );
   }

   verify_signatures_return result;
   result = true;

   // verify authority throws on failure, catch and return false
   try
   {
      amalgam::protocol::verify_authority< verify_signatures_args >(
         { args },
         sig_keys,
         [this]( const string& name ) { return authority( _db.get< chain::account_authority_object, chain::by_account >( name ).owner ); },
         [this]( const string& name ) { return authority( _db.get< chain::account_authority_object, chain::by_account >( name ).active ); },
         [this]( const string& name ) { return authority( _db.get< chain::account_authority_object, chain::by_account >( name ).posting ); },
         AMALGAM_MAX_SIG_CHECK_DEPTH );
   }
   catch( fc::exception& ) { result = false; }

   return result;
}

DEFINE_LOCKLESS_APIS( database_api,
   (get_ops_in_block)
   (get_transaction)
   (get_account_history)
   (get_config)
   (get_version)
)

DEFINE_READ_APIS( database_api,
   (get_block_header)
   (get_block)
   (get_dynamic_global_properties)
   (get_chain_properties)
   (get_witness_schedule)
   (get_reserve_ratio)
   (get_hardfork_properties)
   (get_current_price_feed)
   (get_feed_history)
   (list_witnesses)
   (find_witnesses)
   (list_witness_votes)
   (get_witness_votes_by_account)
   (get_witness_votes_by_witness)
   (get_witnesses_by_vote)
   (get_witness_count)
   (get_active_witnesses)
   (list_accounts)
   (find_accounts)
   (get_account_count)
   (get_account_bandwidth)
   (list_owner_histories)
   (find_owner_histories)
   (list_account_recovery_requests)
   (find_account_recovery_requests)
   (list_change_recovery_account_requests)
   (find_change_recovery_account_requests)
   (list_escrows)
   (find_escrows)
   (get_escrow)
   (list_withdraw_vesting_routes)
   (find_withdraw_vesting_routes)
   (list_savings_withdrawals)
   (find_savings_withdrawals_from)
   (find_savings_withdrawals_to)
   (list_vesting_delegations)
   (find_vesting_delegations)
   (list_vesting_delegation_expirations)
   (find_vesting_delegation_expirations)
   (list_abd_conversion_requests)
   (find_abd_conversion_requests)
   (list_decline_voting_rights_requests)
   (find_decline_voting_rights_requests)
   (list_limit_orders)
   (find_limit_orders)
   (get_transaction_hex)
   (get_required_signatures)
   (get_potential_signatures)
   (verify_authority)
   (verify_account_authority)
   (verify_signatures)
)

} } } // amalgam::plugins::database_api
