#pragma once

#include <amalgam/plugins/database_api/database_api.hpp>
#include <amalgam/plugins/network_broadcast_api/network_broadcast_api.hpp>
#include <amalgam/plugins/account_by_key_api/account_by_key_api.hpp>
#include <amalgam/plugins/market_history_api/market_history_api.hpp>

#include <fc/api.hpp>

namespace amalgam { namespace wallet {

using namespace plugins::database_api;
using namespace plugins::network_broadcast_api;
using namespace plugins::account_by_key;
using namespace plugins::market_history;

/**
 * This is a dummy class exists only to provide method signature information to fc::api, not to execute calls.
 * Class is used by wallet to send formatted API calls to database_api plugin on remote node.
 */
struct remote_database_api
{
   DECLARE_API_FC(
        (get_version)
        (get_active_witnesses)
        (get_block)
        (get_ops_in_block)
        (get_dynamic_global_properties)
        (get_chain_properties)
        (get_current_price_feed)
        (get_feed_history)
        (get_witness_schedule)
        (get_hardfork_properties)
        (list_accounts)
        (find_accounts)
        (get_account_history)
        (list_witnesses)
        (find_witnesses)
        (find_owner_histories)
        (find_abd_conversion_requests)
        (find_withdraw_vesting_routes)
        (find_limit_orders)
        (get_order_book)
        (get_transaction)
   )
};

/**
 * This is a dummy class exists only to provide method signature information to fc::api, not to execute calls.
 * Class is used by wallet to send formatted API calls to network_broadcast_api plugin on remote node.
 */
struct remote_network_broadcast_api {
   DECLARE_API_FC(
        (broadcast_transaction_synchronous)
   )
};

/**
 * This is a dummy class exists only to provide method signature information to fc::api, not to execute calls.
 * Class is used by wallet to send formatted API calls to account_by_key_api plugin on remote node.
 */
struct remote_account_by_key_api {
   DECLARE_API_FC(
        (get_key_references)
   )
};
 
/**
 * This is a dummy class exists only to provide method signature information to fc::api, not to execute calls.
 * Class is used by wallet to send formatted API calls to market_history_api plugin on remote node.
 */
struct remote_market_history_api {
   DECLARE_API_FC(
        (get_order_book)
   )
};
 
} }

/**
 * Declaration of remote API formatter to database_api plugin on remote node
 */
FC_API( amalgam::wallet::remote_database_api,
        (get_version)
        (get_active_witnesses)
        (get_block)
        (get_ops_in_block)
        (get_dynamic_global_properties)
        (get_chain_properties)
        (get_current_price_feed)
        (get_feed_history)
        (get_witness_schedule)
        (get_hardfork_properties)
        (list_accounts)
        (find_accounts)
        (get_account_history)
        (list_witnesses)
        (find_witnesses)
        (find_owner_histories)
        (find_abd_conversion_requests)
        (find_withdraw_vesting_routes)
        (find_limit_orders)
        (get_order_book)
        (get_transaction)
)

/**
 * Declaration of remote API formatter to network_broadcast_api plugin on remote node
 */
FC_API( amalgam::wallet::remote_network_broadcast_api,
        (broadcast_transaction_synchronous)
)

/**
 * Declaration of remote API formatter to account_by_key_api plugin on remote node
 */
FC_API( amalgam::wallet::remote_account_by_key_api,
        (get_key_references)
)

/**
 * Declaration of remote API formatter to market_history_api plugin on remote node
 */
FC_API( amalgam::wallet::remote_market_history_api,
        (get_order_book)
)
