#pragma once

#include <amalgam/protocol/amalgam_operations.hpp>

#include <amalgam/chain/evaluator.hpp>

namespace amalgam{ namespace chain {

using namespace amalgam::protocol;

DEFINE_EVALUATOR( account_create )
DEFINE_EVALUATOR( account_update )
DEFINE_EVALUATOR( transfer )
DEFINE_EVALUATOR( transfer_to_vesting )
DEFINE_EVALUATOR( witness_update )
DEFINE_EVALUATOR( account_witness_vote )
DEFINE_EVALUATOR( account_witness_proxy )
DEFINE_EVALUATOR( withdraw_vesting )
DEFINE_EVALUATOR( set_withdraw_vesting_route )
DEFINE_EVALUATOR( custom )
DEFINE_EVALUATOR( custom_json )
DEFINE_EVALUATOR( custom_binary )
DEFINE_EVALUATOR( feed_publish )
DEFINE_EVALUATOR( convert )
DEFINE_EVALUATOR( limit_order_create )
DEFINE_EVALUATOR( limit_order_cancel )
DEFINE_EVALUATOR( limit_order_create2 )
DEFINE_EVALUATOR( escrow_transfer )
DEFINE_EVALUATOR( escrow_approve )
DEFINE_EVALUATOR( escrow_dispute )
DEFINE_EVALUATOR( escrow_release )
DEFINE_EVALUATOR( request_account_recovery )
DEFINE_EVALUATOR( recover_account )
DEFINE_EVALUATOR( change_recovery_account )
DEFINE_EVALUATOR( transfer_to_savings )
DEFINE_EVALUATOR( transfer_from_savings )
DEFINE_EVALUATOR( cancel_transfer_from_savings )
DEFINE_EVALUATOR( decline_voting_rights )
DEFINE_EVALUATOR( delegate_vesting_shares )
DEFINE_EVALUATOR( tbd1 )
DEFINE_EVALUATOR( tbd2 )
DEFINE_EVALUATOR( tbd3 )
DEFINE_EVALUATOR( tbd4 )
DEFINE_EVALUATOR( tbd5 )
DEFINE_EVALUATOR( tbd6 )
DEFINE_EVALUATOR( tbd7 )
DEFINE_EVALUATOR( tbd8 )
DEFINE_EVALUATOR( tbd9 )
DEFINE_EVALUATOR( tbd10 )

} } // amalgam::chain
