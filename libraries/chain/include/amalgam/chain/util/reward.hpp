#pragma once

#include <amalgam/chain/util/asset.hpp>
#include <amalgam/chain/amalgam_objects.hpp>

#include <amalgam/protocol/asset.hpp>
#include <amalgam/protocol/config.hpp>
#include <amalgam/protocol/types.hpp>

#include <fc/reflect/reflect.hpp>

#include <fc/uint128.hpp>

namespace amalgam { namespace chain { namespace util {

using amalgam::protocol::asset;
using amalgam::protocol::price;
using amalgam::protocol::share_type;

using fc::uint128_t;

struct comment_reward_context
{
   share_type rshares;
   uint16_t   reward_weight = 0;
   asset      max_abd;
   uint128_t  total_reward_shares2;
   asset      total_reward_fund_amalgam;
   price      current_amalgam_price;
   curve_id   reward_curve = quadratic;
   uint128_t  content_constant = AMALGAM_CONTENT_CONSTANT;
};

uint64_t get_rshare_reward( const comment_reward_context& ctx );

inline uint128_t get_content_constant_s()
{
   return AMALGAM_CONTENT_CONSTANT; // looking good for posters
}

uint128_t evaluate_reward_curve( const uint128_t& rshares, const curve_id& curve = quadratic, const uint128_t& content_constant = AMALGAM_CONTENT_CONSTANT );

inline bool is_comment_payout_dust( const price& p, uint64_t amalgam_payout )
{
   return to_abd( p, asset( amalgam_payout, AMALGAM_SYMBOL ) ) < AMALGAM_MIN_PAYOUT_ABD;
}

} } } // amalgam::chain::util

FC_REFLECT( amalgam::chain::util::comment_reward_context,
   (rshares)
   (reward_weight)
   (max_abd)
   (total_reward_shares2)
   (total_reward_fund_amalgam)
   (current_amalgam_price)
   (reward_curve)
   (content_constant)
   )
