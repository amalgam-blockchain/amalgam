#include <amalgam/protocol/authority.hpp>

#include <amalgam/chain/util/impacted.hpp>

#include <fc/utility.hpp>

namespace amalgam { namespace app {

using namespace fc;
using namespace amalgam::protocol;

// TODO:  Review all of these, especially no-ops
struct get_impacted_account_visitor
{
   flat_set<account_name_type>& _impacted;
   get_impacted_account_visitor( flat_set<account_name_type>& impact ):_impacted( impact ) {}
   typedef void result_type;

   template<typename T>
   void operator()( const T& op )
   {
      op.get_required_posting_authorities( _impacted );
      op.get_required_active_authorities( _impacted );
      op.get_required_owner_authorities( _impacted );
   }

   // ops
   void operator()( const account_create_operation& op )
   {
      _impacted.insert( op.new_account_name );
      _impacted.insert( op.creator );
   }

   void operator()( const transfer_operation& op )
   {
      _impacted.insert( op.from );
      _impacted.insert( op.to );
   }

   void operator()( const escrow_transfer_operation& op )
   {
      _impacted.insert( op.from );
      _impacted.insert( op.to );
      _impacted.insert( op.agent );
   }

   void operator()( const escrow_approve_operation& op )
   {
      _impacted.insert( op.from );
      _impacted.insert( op.to );
      _impacted.insert( op.agent );
   }

   void operator()( const escrow_dispute_operation& op )
   {
      _impacted.insert( op.from );
      _impacted.insert( op.to );
      _impacted.insert( op.agent );
   }

   void operator()( const escrow_release_operation& op )
   {
      _impacted.insert( op.from );
      _impacted.insert( op.to );
      _impacted.insert( op.agent );
   }

   void operator()( const transfer_to_vesting_operation& op )
   {
      _impacted.insert( op.from );

      if ( op.to != account_name_type() && op.to != op.from )
      {
         _impacted.insert( op.to );
      }
   }

   void operator()( const set_withdraw_vesting_route_operation& op )
   {
      _impacted.insert( op.from_account );
      _impacted.insert( op.to_account );
   }

   void operator()( const account_witness_vote_operation& op )
   {
      _impacted.insert( op.account );
      _impacted.insert( op.witness );
   }

   void operator()( const account_witness_proxy_operation& op )
   {
      _impacted.insert( op.account );
      _impacted.insert( op.proxy );
   }

   void operator()( const feed_publish_operation& op )
   {
      _impacted.insert( op.publisher );
   }

   void operator()( const request_account_recovery_operation& op )
   {
      _impacted.insert( op.account_to_recover );
      _impacted.insert( op.recovery_account );
   }

   void operator()( const recover_account_operation& op )
   {
      _impacted.insert( op.account_to_recover );
   }

   void operator()( const change_recovery_account_operation& op )
   {
      _impacted.insert( op.account_to_recover );
   }

   void operator()( const transfer_to_savings_operation& op )
   {
      _impacted.insert( op.from );
      _impacted.insert( op.to );
   }

   void operator()( const transfer_from_savings_operation& op )
   {
      _impacted.insert( op.from );
      _impacted.insert( op.to );
   }

   void operator()( const delegate_vesting_shares_operation& op )
   {
      _impacted.insert( op.delegator );
      _impacted.insert( op.delegatee );
   }

   void operator()( const witness_set_properties_operation& op )
   {
      _impacted.insert( op.owner );
   }

   void operator()( const tbd1_operation& op )
   {
      _impacted.insert( op.from );
   }

   void operator()( const tbd2_operation& op )
   {
      _impacted.insert( op.from );
   }

   void operator()( const tbd3_operation& op )
   {
      _impacted.insert( op.from );
   }

   void operator()( const tbd4_operation& op )
   {
      _impacted.insert( op.from );
   }

   void operator()( const tbd5_operation& op )
   {
      _impacted.insert( op.from );
   }

   void operator()( const tbd6_operation& op )
   {
      _impacted.insert( op.from );
   }

   void operator()( const tbd7_operation& op )
   {
      _impacted.insert( op.from );
   }

   void operator()( const tbd8_operation& op )
   {
      _impacted.insert( op.from );
   }

   void operator()( const tbd9_operation& op )
   {
      _impacted.insert( op.from );
   }

   void operator()( const tbd10_operation& op )
   {
      _impacted.insert( op.from );
   }


   // vops

   void operator()( const interest_operation& op )
   {
      _impacted.insert( op.owner );
   }

   void operator()( const fill_convert_request_operation& op )
   {
      _impacted.insert( op.owner );
   }

   void operator()( const fill_vesting_withdraw_operation& op )
   {
      _impacted.insert( op.from_account );
      _impacted.insert( op.to_account );
   }

   void operator()( const fill_order_operation& op )
   {
      _impacted.insert( op.current_owner );
      _impacted.insert( op.open_owner );
   }

   void operator()( const fill_transfer_from_savings_operation& op )
   {
      _impacted.insert( op.from );
      _impacted.insert( op.to );
   }

   void operator()( const return_vesting_delegation_operation& op )
   {
      _impacted.insert( op.account );
   }

   void operator()( const producer_reward_operation& op )
   {
      _impacted.insert( op.producer );
   }

   void operator()( const hardfork_operation& op )
   {
      _impacted.insert( AMALGAM_CREATOR_ACCOUNT );
   }

   //void operator()( const operation& op ){}
};

void operation_get_impacted_accounts( const operation& op, flat_set<account_name_type>& result )
{
   get_impacted_account_visitor vtor = get_impacted_account_visitor( result );
   op.visit( vtor );
}

void transaction_get_impacted_accounts( const transaction& tx, flat_set<account_name_type>& result )
{
   for( const auto& op : tx.operations )
      operation_get_impacted_accounts( op, result );
}

} }
