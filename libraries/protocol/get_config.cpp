#include <amalgam/protocol/get_config.hpp>
#include <amalgam/protocol/config.hpp>
#include <amalgam/protocol/asset.hpp>
#include <amalgam/protocol/types.hpp>
#include <amalgam/protocol/version.hpp>

namespace amalgam { namespace protocol {

fc::variant_object get_config()
{
   fc::mutable_variant_object result;

#ifdef IS_TEST_NET
   result[ "IS_TEST_NET" ] = true;
#else
   result[ "IS_TEST_NET" ] = false;
#endif

   result["ABD_SYMBOL"] = ABD_SYMBOL;
   result["AMALGAM_100_PERCENT"] = AMALGAM_100_PERCENT;
   result["AMALGAM_1_PERCENT"] = AMALGAM_1_PERCENT;
   result["AMALGAM_1_TENTH_PERCENT"] = AMALGAM_1_TENTH_PERCENT;
   result["AMALGAM_ABD_INTEREST_COMPOUND_INTERVAL_SEC"] = AMALGAM_ABD_INTEREST_COMPOUND_INTERVAL_SEC;
   result["AMALGAM_ABD_START_PERCENT"] = AMALGAM_ABD_START_PERCENT;
   result["AMALGAM_ABD_STOP_PERCENT"] = AMALGAM_ABD_STOP_PERCENT;
   result["AMALGAM_ACCOUNT_RECOVERY_REQUEST_EXPIRATION_PERIOD"] = AMALGAM_ACCOUNT_RECOVERY_REQUEST_EXPIRATION_PERIOD;
   result["AMALGAM_ADDRESS_PREFIX"] = AMALGAM_ADDRESS_PREFIX;
   result["AMALGAM_BANDWIDTH_AVERAGE_WINDOW_SECONDS"] = AMALGAM_BANDWIDTH_AVERAGE_WINDOW_SECONDS;
   result["AMALGAM_BANDWIDTH_PRECISION"] = AMALGAM_BANDWIDTH_PRECISION;
   result["AMALGAM_BLOCKCHAIN_HARDFORK_VERSION"] = AMALGAM_BLOCKCHAIN_HARDFORK_VERSION;
   result["AMALGAM_BLOCKCHAIN_VERSION"] = AMALGAM_BLOCKCHAIN_VERSION;
   result["AMALGAM_BLOCK_GENERATION_POSTPONED_TX_LIMIT"] = AMALGAM_BLOCK_GENERATION_POSTPONED_TX_LIMIT;
   result["AMALGAM_BLOCK_INTERVAL"] = AMALGAM_BLOCK_INTERVAL;
   result["AMALGAM_BLOCKS_PER_DAY"] = AMALGAM_BLOCKS_PER_DAY;
   result["AMALGAM_BLOCKS_PER_HOUR"] = AMALGAM_BLOCKS_PER_HOUR;
   result["AMALGAM_BLOCKS_PER_YEAR"] = AMALGAM_BLOCKS_PER_YEAR;
   result["AMALGAM_CHAIN_ID"] = AMALGAM_CHAIN_ID;
   result["AMALGAM_CONTENT_REWARD_PERCENT"] = AMALGAM_CONTENT_REWARD_PERCENT;
   result["AMALGAM_CONVERSION_DELAY"] = AMALGAM_CONVERSION_DELAY;
   result["AMALGAM_CREATOR_ACCOUNT"] = AMALGAM_CREATOR_ACCOUNT;
   result["AMALGAM_CREATOR_PUBLIC_KEY_STR"] = AMALGAM_CREATOR_PUBLIC_KEY_STR;
   result["AMALGAM_DEFAULT_ABD_INTEREST_RATE"] = AMALGAM_DEFAULT_ABD_INTEREST_RATE;
   result["AMALGAM_DELEGATION_RETURN_PERIOD"] = AMALGAM_DELEGATION_RETURN_PERIOD;
   result["AMALGAM_FEED_HISTORY_WINDOW"] = AMALGAM_FEED_HISTORY_WINDOW;
   result["AMALGAM_FEED_INTERVAL_BLOCKS"] = AMALGAM_FEED_INTERVAL_BLOCKS;
   result["AMALGAM_GENESIS_TIME"] = AMALGAM_GENESIS_TIME;
   result["AMALGAM_HARDFORK_REQUIRED_WITNESSES"] = AMALGAM_HARDFORK_REQUIRED_WITNESSES;
   result["AMALGAM_INFLATION_NARROWING_PERIOD"] = AMALGAM_INFLATION_NARROWING_PERIOD;
   result["AMALGAM_INFLATION_RATE_START_PERCENT"] = AMALGAM_INFLATION_RATE_START_PERCENT;
   result["AMALGAM_INFLATION_RATE_STOP_PERCENT"] = AMALGAM_INFLATION_RATE_STOP_PERCENT;
   result["AMALGAM_INIT_SUPPLY"] = AMALGAM_INIT_SUPPLY;
   result["AMALGAM_INIT_VESTING_FUND"] = AMALGAM_INIT_VESTING_FUND;
   result["AMALGAM_INIT_VESTING_SHARES"] = AMALGAM_INIT_VESTING_SHARES;
   result["AMALGAM_IRREVERSIBLE_THRESHOLD"] = AMALGAM_IRREVERSIBLE_THRESHOLD;
   result["AMALGAM_MAX_ACCOUNT_CREATION_FEE"] = AMALGAM_MAX_ACCOUNT_CREATION_FEE;
   result["AMALGAM_MAX_ACCOUNT_NAME_LENGTH"] = AMALGAM_MAX_ACCOUNT_NAME_LENGTH;
   result["AMALGAM_MAX_ACCOUNT_WITNESS_VOTES"] = AMALGAM_MAX_ACCOUNT_WITNESS_VOTES;
   result["AMALGAM_MAX_AUTHORITY_MEMBERSHIP"] = AMALGAM_MAX_AUTHORITY_MEMBERSHIP;
   result["AMALGAM_MAX_BLOCK_SIZE"] = AMALGAM_MAX_BLOCK_SIZE;
   result["AMALGAM_MAX_FEED_AGE_SECONDS"] = AMALGAM_MAX_FEED_AGE_SECONDS;
   result["AMALGAM_MAX_LIMIT_ORDER_EXPIRATION"] = AMALGAM_MAX_LIMIT_ORDER_EXPIRATION;
   result["AMALGAM_MAX_MEMO_SIZE"] = AMALGAM_MAX_MEMO_SIZE;
   result["AMALGAM_MAX_PROXY_RECURSION_DEPTH"] = AMALGAM_MAX_PROXY_RECURSION_DEPTH;
   result["AMALGAM_MAX_RESERVE_RATIO"] = AMALGAM_MAX_RESERVE_RATIO;
   result["AMALGAM_MAX_RUNNER_WITNESSES"] = AMALGAM_MAX_RUNNER_WITNESSES;
   result["AMALGAM_MAX_SATOSHIS"] = AMALGAM_MAX_SATOSHIS;
   result["AMALGAM_MAX_SHARE_SUPPLY"] = AMALGAM_MAX_SHARE_SUPPLY;
   result["AMALGAM_MAX_SIG_CHECK_ACCOUNTS"] = AMALGAM_MAX_SIG_CHECK_ACCOUNTS;
   result["AMALGAM_MAX_SIG_CHECK_DEPTH"] = AMALGAM_MAX_SIG_CHECK_DEPTH;
   result["AMALGAM_MAX_TIME_UNTIL_EXPIRATION"] = AMALGAM_MAX_TIME_UNTIL_EXPIRATION;
   result["AMALGAM_MAX_TRANSACTION_SIZE"] = AMALGAM_MAX_TRANSACTION_SIZE;
   result["AMALGAM_MAX_UNDO_HISTORY"] = AMALGAM_MAX_UNDO_HISTORY;
   result["AMALGAM_MAX_VOTED_WITNESSES"] = AMALGAM_MAX_VOTED_WITNESSES;
   result["AMALGAM_MAX_WITHDRAW_ROUTES"] = AMALGAM_MAX_WITHDRAW_ROUTES;
   result["AMALGAM_MAX_WITNESS_URL_LENGTH"] = AMALGAM_MAX_WITNESS_URL_LENGTH;
   result["AMALGAM_MAX_WITNESSES"] = AMALGAM_MAX_WITNESSES;
   result["AMALGAM_MIN_ACCOUNT_CREATION_FEE"] = AMALGAM_MIN_ACCOUNT_CREATION_FEE;
   result["AMALGAM_MIN_ACCOUNT_NAME_LENGTH"] = AMALGAM_MIN_ACCOUNT_NAME_LENGTH;
   result["AMALGAM_MIN_BLOCK_SIZE"] = AMALGAM_MIN_BLOCK_SIZE;
   result["AMALGAM_MIN_BLOCK_SIZE_LIMIT"] = AMALGAM_MIN_BLOCK_SIZE_LIMIT;
   result["AMALGAM_MIN_FEEDS"] = AMALGAM_MIN_FEEDS;
   result["AMALGAM_MIN_TRANSACTION_SIZE_LIMIT"] = AMALGAM_MIN_TRANSACTION_SIZE_LIMIT;
   result["AMALGAM_NULL_ACCOUNT"] = AMALGAM_NULL_ACCOUNT;
   result["AMALGAM_OWNER_AUTH_HISTORY_TRACKING_START_BLOCK_NUM"] = AMALGAM_OWNER_AUTH_HISTORY_TRACKING_START_BLOCK_NUM;
   result["AMALGAM_OWNER_AUTH_RECOVERY_PERIOD"] = AMALGAM_OWNER_AUTH_RECOVERY_PERIOD;
   result["AMALGAM_OWNER_UPDATE_LIMIT"] = AMALGAM_OWNER_UPDATE_LIMIT;
   result["AMALGAM_PENDING_TRANSACTION_EXECUTION_LIMIT"] = AMALGAM_PENDING_TRANSACTION_EXECUTION_LIMIT;
   result["AMALGAM_PROXY_TO_SELF_ACCOUNT"] = AMALGAM_PROXY_TO_SELF_ACCOUNT;
   result["AMALGAM_REGISTRAR_ACCOUNT"] = AMALGAM_REGISTRAR_ACCOUNT;
   result["AMALGAM_REGISTRAR_PUBLIC_KEY_STR"] = AMALGAM_REGISTRAR_PUBLIC_KEY_STR;
   result["AMALGAM_REWARD_FUND_ACCOUNT"] = AMALGAM_REWARD_FUND_ACCOUNT;
   result["AMALGAM_REWARD_FUND_PUBLIC_KEY_STR"] = AMALGAM_REWARD_FUND_PUBLIC_KEY_STR;
   result["AMALGAM_SAVINGS_WITHDRAW_REQUEST_LIMIT"] = AMALGAM_SAVINGS_WITHDRAW_REQUEST_LIMIT;
   result["AMALGAM_SAVINGS_WITHDRAW_TIME"] = AMALGAM_SAVINGS_WITHDRAW_TIME;
   result["AMALGAM_SECONDS_PER_YEAR"] = AMALGAM_SECONDS_PER_YEAR;
   result["AMALGAM_SOFT_MAX_BLOCK_SIZE"] = AMALGAM_SOFT_MAX_BLOCK_SIZE;
   result["AMALGAM_SYMBOL"] = AMALGAM_SYMBOL;
   result["AMALGAM_TEMP_ACCOUNT"] = AMALGAM_TEMP_ACCOUNT;
   result["AMALGAM_VESTING_FUND_PERCENT"] = AMALGAM_VESTING_FUND_PERCENT;
   result["AMALGAM_VESTING_WITHDRAW_INTERVALS"] = AMALGAM_VESTING_WITHDRAW_INTERVALS;
   result["AMALGAM_VESTING_WITHDRAW_INTERVAL_SECONDS"] = AMALGAM_VESTING_WITHDRAW_INTERVAL_SECONDS;
   result["AMALGAM_VIRTUAL_SCHEDULE_LAP_LENGTH"] = AMALGAM_VIRTUAL_SCHEDULE_LAP_LENGTH;
   result["VESTS_SYMBOL"] = VESTS_SYMBOL;

   return result;
}

} } // amalgam::protocol
