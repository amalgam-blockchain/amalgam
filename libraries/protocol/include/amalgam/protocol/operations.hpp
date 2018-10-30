#pragma once

#include <amalgam/protocol/operation_util.hpp>
#include <amalgam/protocol/amalgam_operations.hpp>
#include <amalgam/protocol/amalgam_virtual_operations.hpp>

namespace amalgam { namespace protocol {

   /** NOTE: do not change the order of any operations prior to the virtual operations
    * or it will trigger a hardfork.
    */
   typedef fc::static_variant<
            transfer_operation,
            transfer_to_vesting_operation,
            withdraw_vesting_operation,

            limit_order_create_operation,
            limit_order_cancel_operation,

            feed_publish_operation,
            convert_operation,

            account_create_operation,
            account_update_operation,

            witness_update_operation,
            account_witness_vote_operation,
            account_witness_proxy_operation,

            custom_operation,
            custom_json_operation,
            set_withdraw_vesting_route_operation,
            limit_order_create2_operation,
            request_account_recovery_operation,
            recover_account_operation,
            change_recovery_account_operation,
            escrow_transfer_operation,
            escrow_dispute_operation,
            escrow_release_operation,
            escrow_approve_operation,
            transfer_to_savings_operation,
            transfer_from_savings_operation,
            cancel_transfer_from_savings_operation,
            custom_binary_operation,
            decline_voting_rights_operation,
            delegate_vesting_shares_operation,

            tbd1_operation,
            tbd2_operation,
            tbd3_operation,
            tbd4_operation,
            tbd5_operation,
            tbd6_operation,
            tbd7_operation,
            tbd8_operation,
            tbd9_operation,
            tbd10_operation,

            /// virtual operations below this point
            fill_convert_request_operation,
            interest_operation,
            fill_vesting_withdraw_operation,
            fill_order_operation,
            shutdown_witness_operation,
            fill_transfer_from_savings_operation,
            hardfork_operation,
            return_vesting_delegation_operation,
            producer_reward_operation
         > operation;

   bool is_market_operation( const operation& op );

   bool is_virtual_operation( const operation& op );

} } // amalgam::protocol

DECLARE_OPERATION_TYPE( amalgam::protocol::operation )
FC_REFLECT_TYPENAME( amalgam::protocol::operation )
