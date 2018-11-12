#pragma once

#include <amalgam/protocol/amalgam_operations.hpp>

#include <amalgam/chain/evaluator.hpp>

namespace amalgam{ namespace chain {

using namespace amalgam::protocol;

AMALGAM_DEFINE_EVALUATOR( account_create )
AMALGAM_DEFINE_EVALUATOR( account_update )
AMALGAM_DEFINE_EVALUATOR( transfer )
AMALGAM_DEFINE_EVALUATOR( transfer_to_vesting )
AMALGAM_DEFINE_EVALUATOR( witness_update )
AMALGAM_DEFINE_EVALUATOR( account_witness_vote )
AMALGAM_DEFINE_EVALUATOR( account_witness_proxy )
AMALGAM_DEFINE_EVALUATOR( withdraw_vesting )
AMALGAM_DEFINE_EVALUATOR( set_withdraw_vesting_route )
AMALGAM_DEFINE_EVALUATOR( custom )
AMALGAM_DEFINE_EVALUATOR( custom_json )
AMALGAM_DEFINE_EVALUATOR( custom_binary )
AMALGAM_DEFINE_EVALUATOR( feed_publish )
AMALGAM_DEFINE_EVALUATOR( convert )
AMALGAM_DEFINE_EVALUATOR( limit_order_create )
AMALGAM_DEFINE_EVALUATOR( limit_order_cancel )
AMALGAM_DEFINE_EVALUATOR( limit_order_create2 )
AMALGAM_DEFINE_EVALUATOR( escrow_transfer )
AMALGAM_DEFINE_EVALUATOR( escrow_approve )
AMALGAM_DEFINE_EVALUATOR( escrow_dispute )
AMALGAM_DEFINE_EVALUATOR( escrow_release )
AMALGAM_DEFINE_EVALUATOR( request_account_recovery )
AMALGAM_DEFINE_EVALUATOR( recover_account )
AMALGAM_DEFINE_EVALUATOR( change_recovery_account )
AMALGAM_DEFINE_EVALUATOR( transfer_to_savings )
AMALGAM_DEFINE_EVALUATOR( transfer_from_savings )
AMALGAM_DEFINE_EVALUATOR( cancel_transfer_from_savings )
AMALGAM_DEFINE_EVALUATOR( decline_voting_rights )
AMALGAM_DEFINE_EVALUATOR( delegate_vesting_shares )
AMALGAM_DEFINE_EVALUATOR( witness_set_properties )
AMALGAM_DEFINE_EVALUATOR( tbd1 )
AMALGAM_DEFINE_EVALUATOR( tbd2 )
AMALGAM_DEFINE_EVALUATOR( tbd3 )
AMALGAM_DEFINE_EVALUATOR( tbd4 )
AMALGAM_DEFINE_EVALUATOR( tbd5 )
AMALGAM_DEFINE_EVALUATOR( tbd6 )
AMALGAM_DEFINE_EVALUATOR( tbd7 )
AMALGAM_DEFINE_EVALUATOR( tbd8 )
AMALGAM_DEFINE_EVALUATOR( tbd9 )
AMALGAM_DEFINE_EVALUATOR( tbd10 )

} } // amalgam::chain
