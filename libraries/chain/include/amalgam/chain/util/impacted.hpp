#pragma once

#include <fc/container/flat.hpp>
#include <amalgam/protocol/operations.hpp>
#include <amalgam/protocol/transaction.hpp>

#include <fc/string.hpp>

namespace amalgam { namespace app {

using namespace fc;

void operation_get_impacted_accounts(
   const amalgam::protocol::operation& op,
   fc::flat_set<protocol::account_name_type>& result );

void transaction_get_impacted_accounts(
   const amalgam::protocol::transaction& tx,
   fc::flat_set<protocol::account_name_type>& result
   );

} } // amalgam::app
