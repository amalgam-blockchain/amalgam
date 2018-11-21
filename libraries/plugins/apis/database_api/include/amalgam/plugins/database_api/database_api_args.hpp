#pragma once
#include <amalgam/plugins/database_api/database_api_objects.hpp>

#include <amalgam/protocol/types.hpp>
#include <amalgam/protocol/transaction.hpp>
#include <amalgam/protocol/block_header.hpp>

#include <amalgam/plugins/json_rpc/utility.hpp>
#include <amalgam/plugins/witness/witness_objects.hpp>

namespace amalgam { namespace plugins { namespace database_api {

using protocol::account_name_type;
using protocol::signed_transaction;
using protocol::transaction_id_type;
using protocol::public_key_type;
using plugins::json_rpc::void_type;

enum sort_order_type
{
   by_name,
   by_proxy,
   by_next_vesting_withdrawal,
   by_account,
   by_expiration,
   by_effective_date,
   by_vote_name,
   by_schedule_time,
   by_account_witness,
   by_witness_account,
   by_from_id,
   by_ratification_deadline,
   by_withdraw_route,
   by_destination,
   by_complete_from_id,
   by_to_complete,
   by_delegation,
   by_account_expiration,
   by_conversion_date,
   by_price
};

/* Blocks and transactions */

/* get_block_header */

struct get_block_header_args
{
   uint32_t block_num;
};

typedef optional< block_header > get_block_header_return;


/* get_block */

struct get_block_args
{
   uint32_t block_num;
};

typedef optional< api_signed_block_object > get_block_return;


/* get_ops_in_block */

struct get_ops_in_block_args
{
   uint32_t block_num;
   bool     only_virtual;
};

typedef std::multiset< api_operation_object > get_ops_in_block_return;


/* get_transaction */

struct get_transaction_args
{
   amalgam::protocol::transaction_id_type id;
};

typedef amalgam::protocol::annotated_signed_transaction get_transaction_return;


/* Globals */

/* get_config */

typedef void_type          get_config_args;
typedef fc::variant_object get_config_return;


/* get_version */

typedef void_type          get_version_args;
struct get_version_return
{
   get_version_return() {}
   get_version_return( fc::string bc_v, fc::string s_v, fc::string fc_v, chain_id_type c_id )
      :blockchain_version( bc_v ), amalgam_revision( s_v ), fc_revision( fc_v ), chain_id( c_id ) {}

   fc::string     blockchain_version;
   fc::string     amalgam_revision;
   fc::string     fc_revision;
   chain_id_type  chain_id;
};


/* get_dynamic_global_properties */

typedef void_type                            get_dynamic_global_properties_args;
typedef api_dynamic_global_property_object   get_dynamic_global_properties_return;


/* get_chain_properties */

typedef void_type        get_chain_properties_args;
typedef chain_properties get_chain_properties_return;


/* get_witness_schedule */

typedef void_type                   get_witness_schedule_args;
typedef api_witness_schedule_object get_witness_schedule_return;


/* get_reserve_ratio */

typedef void_type            get_reserve_ratio_args;
typedef reserve_ratio_object get_reserve_ratio_return;


/* get_hardfork_properties */

typedef void_type                      get_hardfork_properties_args;
typedef api_hardfork_property_object   get_hardfork_properties_return;


/* get_current_price_feed */

typedef void_type get_current_price_feed_args;
typedef price     get_current_price_feed_return;


/* get_current_feed_history */

typedef void_type                get_feed_history_args;
typedef api_feed_history_object  get_feed_history_return;


/* Witnesses */

struct list_witnesses_args
{
   fc::variant       start;
   uint32_t          limit;
   sort_order_type   order;
};

typedef vector< api_witness_object > list_witnesses_return;


struct find_witnesses_args
{
   vector< account_name_type > owners;
};

typedef vector< api_witness_object > find_witnesses_return;


struct list_witness_votes_args
{
   fc::variant       start;
   uint32_t          limit;
   sort_order_type   order;
};

typedef vector< api_witness_vote_object > list_witness_votes_return;


typedef void_type get_active_witnesses_args;

typedef vector< account_name_type > get_active_witnesses_return;


/* Account */

struct list_accounts_args
{
   fc::variant       start;
   uint32_t          limit;
   sort_order_type   order;
};

typedef vector< api_account_object > list_accounts_return;


struct find_accounts_args
{
   vector< account_name_type > accounts;
};

typedef vector< api_account_object > find_accounts_return;


struct get_account_history_args
{
   account_name_type   account;
   uint64_t            start = -1;
   uint32_t            limit = 1000;
};

typedef std::map< uint32_t, api_operation_object > get_account_history_return;


struct get_account_bandwidth_args
{
   account_name_type   account;
   bandwidth_type      type;
};

typedef optional< api_account_bandwidth_object > get_account_bandwidth_return;


/* Owner Auth History */

struct list_owner_histories_args
{
   fc::variant       start;
   uint32_t          limit;
};

typedef vector< api_owner_authority_history_object > list_owner_histories_return;


struct find_owner_histories_args
{
   account_name_type owner;
};

typedef vector< api_owner_authority_history_object > find_owner_histories_return;


/* Account Recovery Requests */

struct list_account_recovery_requests_args
{
   fc::variant       start;
   uint32_t          limit;
   sort_order_type   order;
};

typedef vector< api_account_recovery_request_object > list_account_recovery_requests_return;


struct find_account_recovery_requests_args
{
   vector< account_name_type > accounts;
};

typedef vector< api_account_recovery_request_object > find_account_recovery_requests_return;


/* Change Recovery Account Requests */

struct list_change_recovery_account_requests_args
{
   fc::variant       start;
   uint32_t          limit;
   sort_order_type   order;
};

typedef vector< api_change_recovery_account_request_object > list_change_recovery_account_requests_return;


struct find_change_recovery_account_requests_args
{
   vector< account_name_type > accounts;
};

typedef vector< api_change_recovery_account_request_object > find_change_recovery_account_requests_return;


/* Escrow */

struct list_escrows_args
{
   fc::variant       start;
   uint32_t          limit;
   sort_order_type   order;
};

typedef vector< api_escrow_object > list_escrows_return;


struct find_escrows_args
{
   account_name_type from;
};

typedef vector< api_escrow_object > find_escrows_return;


/* Vesting Withdraw Routes */

struct list_withdraw_vesting_routes_args
{
   fc::variant       start;
   uint32_t          limit;
   sort_order_type   order;
};

typedef vector< api_withdraw_vesting_route_object > list_withdraw_vesting_routes_return;


struct find_withdraw_vesting_routes_args
{
   account_name_type account;
   sort_order_type   order;
};

typedef vector< api_withdraw_vesting_route_object > find_withdraw_vesting_routes_return;


/* Savings Withdraw */

struct list_savings_withdrawals_args
{
   fc::variant       start;
   uint32_t          limit;
   sort_order_type   order;
};

typedef vector< api_savings_withdraw_object > list_savings_withdrawals_return;


struct find_savings_withdrawals_args
{
   account_name_type account;
};

typedef vector< api_savings_withdraw_object > find_savings_withdrawals_return;


/* Vesting Delegations */

struct list_vesting_delegations_args
{
   fc::variant       start;
   uint32_t          limit;
   sort_order_type   order;
};

typedef vector< api_vesting_delegation_object > list_vesting_delegations_return;


struct find_vesting_delegations_args
{
   account_name_type account;
};

typedef vector< api_vesting_delegation_object > find_vesting_delegations_return;


/* Vesting Delegation Expirations */

struct list_vesting_delegation_expirations_args
{
   fc::variant       start;
   uint32_t          limit;
   sort_order_type   order;
};

typedef vector< api_vesting_delegation_expiration_object > list_vesting_delegation_expirations_return;


struct find_vesting_delegation_expirations_args
{
   account_name_type account;
};

typedef vector< api_vesting_delegation_expiration_object > find_vesting_delegation_expirations_return;


/* ABD Conversions */

struct list_abd_conversion_requests_args
{
   fc::variant       start;
   uint32_t          limit;
   sort_order_type   order;
};

typedef vector< api_convert_request_object > list_abd_conversion_requests_return;


struct find_abd_conversion_requests_args
{
   account_name_type account;
};

typedef vector< api_convert_request_object > find_abd_conversion_requests_return;


/* Decline Voting Rights Requests */

struct list_decline_voting_rights_requests_args
{
   fc::variant       start;
   uint32_t          limit;
   sort_order_type   order;
};

typedef vector< api_decline_voting_rights_request_object > list_decline_voting_rights_requests_return;


struct find_decline_voting_rights_requests_args
{
   vector< account_name_type > accounts;
};

typedef vector< api_decline_voting_rights_request_object > find_decline_voting_rights_requests_return;


/* Limit Orders */

struct list_limit_orders_args
{
   fc::variant       start;
   uint32_t          limit;
   sort_order_type   order;
};

typedef vector< api_limit_order_object > list_limit_orders_return;


struct find_limit_orders_args
{
   account_name_type account;
};

typedef vector< api_limit_order_object > find_limit_orders_return;


struct get_transaction_hex_args
{
   signed_transaction trx;
};

typedef std::string get_transaction_hex_return;


struct get_required_signatures_args
{
   signed_transaction          trx;
   flat_set< public_key_type > available_keys;
};

typedef set< public_key_type > get_required_signatures_return;


struct get_potential_signatures_args
{
   signed_transaction trx;
};

typedef set< public_key_type > get_potential_signatures_return;


struct verify_authority_args
{
   signed_transaction trx;
};

typedef bool verify_authority_return;

struct verify_account_authority_args
{
   account_name_type           account;
   flat_set< public_key_type > signers;
};

typedef bool verify_account_authority_return;

struct verify_signatures_args
{
   fc::sha256                    hash;
   vector< signature_type >      signatures;
   vector< account_name_type >   required_owner;
   vector< account_name_type >   required_active;
   vector< account_name_type >   required_posting;
   vector< authority >           required_other;

   void get_required_owner_authorities( flat_set< account_name_type >& a )const
   {
      a.insert( required_owner.begin(), required_owner.end() );
   }

   void get_required_active_authorities( flat_set< account_name_type >& a )const
   {
      a.insert( required_active.begin(), required_active.end() );
   }

   void get_required_posting_authorities( flat_set< account_name_type >& a )const
   {
      a.insert( required_posting.begin(), required_posting.end() );
   }

   void get_required_authorities( vector< authority >& a )const
   {
      a.insert( a.end(), required_other.begin(), required_other.end() );
   }
};

typedef bool verify_signatures_return;

} } } // amalgam::database_api

FC_REFLECT( amalgam::plugins::database_api::get_block_header_args,
   (block_num) )

FC_REFLECT( amalgam::plugins::database_api::get_block_args,
   (block_num) )

FC_REFLECT( amalgam::plugins::database_api::get_version_return,
            (blockchain_version)(amalgam_revision)(fc_revision)(chain_id) )

FC_REFLECT( amalgam::plugins::database_api::get_ops_in_block_args,
   (block_num)(only_virtual) )

FC_REFLECT( amalgam::plugins::database_api::get_transaction_args,
   (id) )

FC_REFLECT_ENUM( amalgam::plugins::database_api::sort_order_type,
   (by_name)
   (by_proxy)
   (by_next_vesting_withdrawal)
   (by_account)
   (by_expiration)
   (by_effective_date)
   (by_vote_name)
   (by_schedule_time)
   (by_account_witness)
   (by_witness_account)
   (by_from_id)
   (by_ratification_deadline)
   (by_withdraw_route)
   (by_destination)
   (by_complete_from_id)
   (by_to_complete)
   (by_delegation)
   (by_account_expiration)
   (by_conversion_date)
   (by_price) )

FC_REFLECT( amalgam::plugins::database_api::list_witnesses_args,
   (start)(limit)(order) )

FC_REFLECT( amalgam::plugins::database_api::find_witnesses_args,
   (owners) )

FC_REFLECT( amalgam::plugins::database_api::list_witness_votes_args,
   (start)(limit)(order) )

FC_REFLECT( amalgam::plugins::database_api::list_accounts_args,
   (start)(limit)(order) )

FC_REFLECT( amalgam::plugins::database_api::find_accounts_args,
   (accounts) )

FC_REFLECT( amalgam::plugins::database_api::get_account_history_args,
   (account)(start)(limit) )

FC_REFLECT( amalgam::plugins::database_api::get_account_bandwidth_args,
            (account)(type) )

FC_REFLECT( amalgam::plugins::database_api::list_owner_histories_args,
   (start)(limit) )

FC_REFLECT( amalgam::plugins::database_api::find_owner_histories_args,
   (owner) )

FC_REFLECT( amalgam::plugins::database_api::list_account_recovery_requests_args,
   (start)(limit)(order) )

FC_REFLECT( amalgam::plugins::database_api::find_account_recovery_requests_args,
   (accounts) )

FC_REFLECT( amalgam::plugins::database_api::list_change_recovery_account_requests_args,
   (start)(limit)(order) )

FC_REFLECT( amalgam::plugins::database_api::find_change_recovery_account_requests_args,
   (accounts) )

FC_REFLECT( amalgam::plugins::database_api::list_escrows_args,
   (start)(limit)(order) )

FC_REFLECT( amalgam::plugins::database_api::find_escrows_args,
   (from) )

FC_REFLECT( amalgam::plugins::database_api::list_withdraw_vesting_routes_args,
   (start)(limit)(order) )

FC_REFLECT( amalgam::plugins::database_api::find_withdraw_vesting_routes_args,
   (account)(order) )

FC_REFLECT( amalgam::plugins::database_api::list_savings_withdrawals_args,
   (start)(limit)(order) )

FC_REFLECT( amalgam::plugins::database_api::find_savings_withdrawals_args,
   (account) )

FC_REFLECT( amalgam::plugins::database_api::list_vesting_delegations_args,
   (start)(limit)(order) )

FC_REFLECT( amalgam::plugins::database_api::find_vesting_delegations_args,
   (account) )

FC_REFLECT( amalgam::plugins::database_api::list_vesting_delegation_expirations_args,
   (start)(limit)(order) )

FC_REFLECT( amalgam::plugins::database_api::find_vesting_delegation_expirations_args,
   (account) )

FC_REFLECT( amalgam::plugins::database_api::list_abd_conversion_requests_args,
   (start)(limit)(order) )

FC_REFLECT( amalgam::plugins::database_api::find_abd_conversion_requests_args,
   (account) )

FC_REFLECT( amalgam::plugins::database_api::list_decline_voting_rights_requests_args,
   (start)(limit)(order) )

FC_REFLECT( amalgam::plugins::database_api::find_decline_voting_rights_requests_args,
   (accounts) )

FC_REFLECT( amalgam::plugins::database_api::list_limit_orders_args,
   (start)(limit)(order) )

FC_REFLECT( amalgam::plugins::database_api::find_limit_orders_args,
   (account) )

FC_REFLECT( amalgam::plugins::database_api::get_transaction_hex_args,
   (trx) )

FC_REFLECT( amalgam::plugins::database_api::get_required_signatures_args,
   (trx)
   (available_keys) )

FC_REFLECT( amalgam::plugins::database_api::get_potential_signatures_args,
   (trx) )

FC_REFLECT( amalgam::plugins::database_api::verify_authority_args,
   (trx) )

FC_REFLECT( amalgam::plugins::database_api::verify_account_authority_args,
   (account)
   (signers) )

FC_REFLECT( amalgam::plugins::database_api::verify_signatures_args,
   (hash)
   (signatures)
   (required_owner)
   (required_active)
   (required_posting)
   (required_other) )
