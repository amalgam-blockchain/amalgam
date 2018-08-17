/*
 * Copyright (c) 2017 Amalgam and contributors.
 */
#pragma once

#define AMALGAM_BLOCKCHAIN_VERSION              ( version(0, 1, 0) )
#define AMALGAM_BLOCKCHAIN_HARDFORK_VERSION     ( hardfork_version( AMALGAM_BLOCKCHAIN_VERSION ) )

#ifdef IS_TEST_NET

#define AMALGAM_INIT_PUBLIC_KEY_STR             "AML8b4wrNBbCPz1ndxQGhMrdqVWLUBxU5pgzDa7hiFHJ8tdT7GR86"
#define AMALGAM_CHAIN_ID                        (fc::sha256::hash("AMALGAMTEST"))

#define AMALGAM_SYMBOL  (uint64_t(3) | (uint64_t('A') << 8) | (uint64_t('M') << 16) | (uint64_t('L') << 24)) ///< AMALGAM with 3 digits of precision
#define VESTS_SYMBOL    (uint64_t(6) | (uint64_t('A') << 8) | (uint64_t('M') << 16) | (uint64_t('L') << 24) | (uint64_t('V') << 32)) ///< AMALGAM VESTS with 6 digits of precision
#define ABD_SYMBOL      (uint64_t(3) | (uint64_t('A') << 8) | (uint64_t('M') << 16) | (uint64_t('L') << 24) | (uint64_t('D') << 32)) ///< AMALGAM Backed Dollars with 3 digits of precision
#define AMALGAM_SYMBOL_NAME                     "AMALGAM"
#define AMALGAM_ADDRESS_PREFIX                  "AML"

#define AMALGAM_GENESIS_TIME                    (fc::time_point_sec(1534377600))
#define AMALGAM_CASHOUT_WINDOW_SECONDS          (60*60) /// 1 hr
#define AMALGAM_VOTE_CHANGE_LOCKOUT_PERIOD      (60*10) /// 10 minutes
#define AMALGAM_UPVOTE_LOCKOUT                  (fc::minutes(5))

#define AMALGAM_MIN_ACCOUNT_CREATION_FEE          0

#define AMALGAM_OWNER_AUTH_RECOVERY_PERIOD                  fc::seconds(60)
#define AMALGAM_ACCOUNT_RECOVERY_REQUEST_EXPIRATION_PERIOD  fc::seconds(12)
#define AMALGAM_OWNER_UPDATE_LIMIT                          fc::seconds(0)
#define AMALGAM_OWNER_AUTH_HISTORY_TRACKING_START_BLOCK_NUM 1

#else // IS LIVE AMALGAM NETWORK

#define AMALGAM_INIT_PUBLIC_KEY_STR             "AML7T4abS2yuPS6nGtvjJiHkLoXTQ8SjoYRZCq9yY66M88xkPyGfX"
#define AMALGAM_CHAIN_ID                        (fc::sha256::hash("AMALGAM"))

#define AMALGAM_SYMBOL  (uint64_t(3) | (uint64_t('A') << 8) | (uint64_t('M') << 16) | (uint64_t('L') << 24)) ///< AMALGAM with 3 digits of precision
#define VESTS_SYMBOL    (uint64_t(6) | (uint64_t('A') << 8) | (uint64_t('M') << 16) | (uint64_t('L') << 24) | (uint64_t('V') << 32)) ///< AMALGAM VESTS with 6 digits of precision
#define ABD_SYMBOL      (uint64_t(3) | (uint64_t('A') << 8) | (uint64_t('M') << 16) | (uint64_t('L') << 24) | (uint64_t('D') << 32)) ///< AMALGAM Backed Dollars with 3 digits of precision
#define AMALGAM_SYMBOL_NAME                     "AMALGAM"
#define AMALGAM_ADDRESS_PREFIX                  "AML"

#define AMALGAM_GENESIS_TIME                    (fc::time_point_sec(1534377600))
#define AMALGAM_CASHOUT_WINDOW_SECONDS          (60*60*24*7)  /// 7 days
#define AMALGAM_VOTE_CHANGE_LOCKOUT_PERIOD      (60*60*2)     /// 2 hours
#define AMALGAM_UPVOTE_LOCKOUT                  (fc::hours(12))

#define AMALGAM_MIN_ACCOUNT_CREATION_FEE           100 // 0.1 AML

#define AMALGAM_OWNER_AUTH_RECOVERY_PERIOD                  fc::days(30)
#define AMALGAM_ACCOUNT_RECOVERY_REQUEST_EXPIRATION_PERIOD  fc::days(1)
#define AMALGAM_OWNER_UPDATE_LIMIT                          fc::minutes(60)
#define AMALGAM_OWNER_AUTH_HISTORY_TRACKING_START_BLOCK_NUM 1

#endif

#define AMALGAM_BLOCK_INTERVAL                  3
#define AMALGAM_BLOCKS_PER_YEAR                 (365*24*60*60/AMALGAM_BLOCK_INTERVAL)
#define AMALGAM_BLOCKS_PER_DAY                  (24*60*60/AMALGAM_BLOCK_INTERVAL)

#define AMALGAM_INIT_WITNESS_NAME               "creator"
#define AMALGAM_NUM_INIT_WITNESSES              1

#define AMALGAM_MAX_WITNESSES                   21

#define AMALGAM_MAX_VOTED_WITNESSES             20
#define AMALGAM_MAX_MINER_WITNESSES             0
#define AMALGAM_MAX_RUNNER_WITNESSES            1

#define AMALGAM_HARDFORK_REQUIRED_WITNESSES     17 // 17 of the 21 dpos witnesses (20 elected and 1 virtual time) required for hardfork. This guarantees 75% participation on all subsequent rounds.
#define AMALGAM_MAX_TIME_UNTIL_EXPIRATION       (60*60) // seconds,  aka: 1 hour
#define AMALGAM_MAX_MEMO_SIZE                   2048
#define AMALGAM_MAX_PROXY_RECURSION_DEPTH       4
#define AMALGAM_VESTING_WITHDRAW_INTERVALS      13
#define AMALGAM_VESTING_WITHDRAW_INTERVAL_SECONDS (60*60*24*7) /// 1 week per interval
#define AMALGAM_MAX_WITHDRAW_ROUTES             10
#define AMALGAM_SAVINGS_WITHDRAW_TIME        	(fc::days(3))
#define AMALGAM_SAVINGS_WITHDRAW_REQUEST_LIMIT  100
#define AMALGAM_VOTE_REGENERATION_SECONDS       (5*60*60*24) // 5 day
#define AMALGAM_MAX_VOTE_CHANGES                5
#define AMALGAM_REVERSE_AUCTION_WINDOW_SECONDS  (60*30) /// 30 minutes
#define AMALGAM_MIN_VOTE_INTERVAL_SEC           1
#define AMALGAM_VOTE_DUST_THRESHOLD             (50000000)

#define AMALGAM_MIN_ROOT_COMMENT_INTERVAL       (fc::seconds(1)) // 1 second
#define AMALGAM_MIN_REPLY_INTERVAL              (fc::seconds(1)) // 1 second

#define AMALGAM_MAX_ACCOUNT_WITNESS_VOTES       30

#define AMALGAM_100_PERCENT                     10000
#define AMALGAM_1_PERCENT                       (AMALGAM_100_PERCENT/100)
#define AMALGAM_1_TENTH_PERCENT                 (AMALGAM_100_PERCENT/1000)
#define AMALGAM_DEFAULT_ABD_INTEREST_RATE       (10*AMALGAM_1_PERCENT) ///< 10% APR

#define AMALGAM_INFLATION_RATE_START_PERCENT    (950) // Fixes block 0 to 9.5%
#define AMALGAM_INFLATION_RATE_STOP_PERCENT     (95) // 0.95%
#define AMALGAM_INFLATION_NARROWING_PERIOD      (250000) // Narrow 0.01% every 250k blocks
#define AMALGAM_CONTENT_REWARD_PERCENT          (75*AMALGAM_1_PERCENT) //75% of inflation
#define AMALGAM_VESTING_FUND_PERCENT            (15*AMALGAM_1_PERCENT) //15% of inflation

#define AMALGAM_BANDWIDTH_AVERAGE_WINDOW_SECONDS (60*60*24*7) ///< 1 week
#define AMALGAM_BANDWIDTH_PRECISION             (uint64_t(1000000)) ///< 1 million
#define AMALGAM_MAX_COMMENT_DEPTH               0xffff // 64k
#define AMALGAM_SOFT_MAX_COMMENT_DEPTH          0xff // 255

#define AMALGAM_MAX_RESERVE_RATIO               (20000)

#define AMALGAM_CREATE_ACCOUNT_WITH_AMALGAM_MODIFIER 30
#define AMALGAM_CREATE_ACCOUNT_DELEGATION_RATIO    5
#define AMALGAM_CREATE_ACCOUNT_DELEGATION_TIME     fc::days(30)

#define AMALGAM_EQUIHASH_N                      140
#define AMALGAM_EQUIHASH_K                      6

#define AMALGAM_POST_REWARD_FUND_NAME           ("post")
#define AMALGAM_RECENT_RSHARES_DECAY_RATE       (fc::days(15))
#define AMALGAM_CONTENT_CONSTANT                (uint128_t(uint64_t(2000000000000ll)))

#define AMALGAM_MIN_PAYOUT_ABD                  (asset(20,ABD_SYMBOL))

#define AMALGAM_ABD_STOP_PERCENT                (5*AMALGAM_1_PERCENT ) // Stop printing ABD at 5% Market Cap
#define AMALGAM_ABD_START_PERCENT               (2*AMALGAM_1_PERCENT) // Start reducing printing of ABD at 2% Market Cap

#define AMALGAM_MIN_ACCOUNT_NAME_LENGTH          3
#define AMALGAM_MAX_ACCOUNT_NAME_LENGTH         16

#define AMALGAM_MIN_PERMLINK_LENGTH             0
#define AMALGAM_MAX_PERMLINK_LENGTH             256
#define AMALGAM_MAX_WITNESS_URL_LENGTH          2048

#define AMALGAM_INIT_SUPPLY                     int64_t(500000000000ll)
#define AMALGAM_MAX_SHARE_SUPPLY                int64_t(1000000000000000ll)
#define AMALGAM_MAX_SIG_CHECK_DEPTH             2

#define AMALGAM_INIT_VESTING_FUND               uint64_t(360000000000ll)
#define AMALGAM_INIT_VESTING_SHARES             uint64_t(900000000000000000ll) // to set a vesting rate of 400 AML per MegaAMLV with about 2% interest
#define AMALGAM_INIT_RECENT_CLAIMS              uint128_t(uint64_t(8400000000000000ll)) // to stabilize payout values at first stage

#define AMALGAM_MIN_TRANSACTION_SIZE_LIMIT      1024
#define AMALGAM_SECONDS_PER_YEAR                (uint64_t(60*60*24*365ll))

#define AMALGAM_ABD_INTEREST_COMPOUND_INTERVAL_SEC  (60*60*24*30)
#define AMALGAM_MAX_TRANSACTION_SIZE            (1024*64)
#define AMALGAM_MIN_BLOCK_SIZE_LIMIT            (AMALGAM_MAX_TRANSACTION_SIZE)
#define AMALGAM_MAX_BLOCK_SIZE                  (AMALGAM_MAX_TRANSACTION_SIZE*AMALGAM_BLOCK_INTERVAL*2000)
#define AMALGAM_SOFT_MAX_BLOCK_SIZE             (2*1024*1024)
#define AMALGAM_MIN_BLOCK_SIZE                  115
#define AMALGAM_BLOCKS_PER_HOUR                 (60*60/AMALGAM_BLOCK_INTERVAL)
#define AMALGAM_FEED_INTERVAL_BLOCKS            (AMALGAM_BLOCKS_PER_HOUR)
#define AMALGAM_FEED_HISTORY_WINDOW             (12*7) // 3.5 days
#define AMALGAM_MAX_FEED_AGE_SECONDS            (60*60*24*7) // 7 days
#define AMALGAM_MIN_FEEDS                       (AMALGAM_MAX_WITNESSES/3) /// protects the network from conversions before price has been established
#define AMALGAM_CONVERSION_DELAY                (fc::hours(AMALGAM_FEED_HISTORY_WINDOW)) //3.5 day conversion

#define AMALGAM_MAX_UNDO_HISTORY                10000

#define AMALGAM_IRREVERSIBLE_THRESHOLD          (75 * AMALGAM_1_PERCENT)

#define VIRTUAL_SCHEDULE_LAP_LENGTH ( fc::uint128::max_value() )

/**
 *  Reserved Account IDs with special meaning
 */
///@{
/// Represents the current witnesses
#define AMALGAM_MINER_ACCOUNT                   "miners"
/// Represents the canonical account with NO authority (nobody can access funds in null account)
#define AMALGAM_NULL_ACCOUNT                    "null"
/// Represents the canonical account with WILDCARD authority (anybody can access funds in temp account)
#define AMALGAM_TEMP_ACCOUNT                    "temp"
/// Represents the canonical account for specifying you will vote for directly (as opposed to a proxy)
#define AMALGAM_PROXY_TO_SELF_ACCOUNT           ""
/// Represents the canonical root post parent account
#define AMALGAM_ROOT_POST_PARENT                (account_name_type())
///@}
