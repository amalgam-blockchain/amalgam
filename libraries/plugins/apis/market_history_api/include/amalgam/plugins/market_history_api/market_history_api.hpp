#pragma once
#include <amalgam/plugins/json_rpc/utility.hpp>
#include <amalgam/plugins/market_history/market_history_plugin.hpp>

#include <amalgam/protocol/types.hpp>

#include <fc/optional.hpp>
#include <fc/variant.hpp>
#include <fc/vector.hpp>


namespace amalgam { namespace plugins { namespace market_history {


using amalgam::chain::share_type;
using amalgam::chain::asset;
using fc::time_point_sec;
using json_rpc::void_type;


typedef void_type get_ticker_args;

struct get_ticker_return
{
   double      latest = 0;
   double      lowest_ask = 0;
   double      highest_bid = 0;
   double      percent_change = 0;
   asset       amalgam_volume = asset( 0 , AMALGAM_SYMBOL );
   asset       abd_volume = asset( 0, ABD_SYMBOL );
};

typedef void_type get_volume_args;

struct get_volume_return
{
   asset       amalgam_volume = asset( 0, AMALGAM_SYMBOL );
   asset       abd_volume = asset( 0, ABD_SYMBOL );
};

struct order
{
   price          order_price;
   double         real_price;
   share_type     amalgam;
   share_type     abd;
   time_point_sec created;
};

struct order_book
{
   vector< order > bids;
   vector< order > asks;
};

struct get_order_book_args
{
   uint32_t limit = 500;
};

typedef order_book get_order_book_return;

struct market_trade
{
   time_point_sec date;
   asset          current_pays;
   asset          open_pays;
};

struct get_trade_history_args
{
   time_point_sec start;
   time_point_sec end;
   uint32_t       limit = 1000;
};

typedef std::vector< market_trade > get_trade_history_return;

struct get_recent_trades_args
{
   uint32_t limit = 1000;
};

typedef std::vector< market_trade > get_recent_trades_return;

struct get_market_history_args
{
   uint32_t       bucket_seconds;
   time_point_sec start;
   time_point_sec end;
};

typedef std::vector< market_history::bucket_object > get_market_history_return;

typedef void_type get_market_history_buckets_args;

typedef flat_set< uint32_t > get_market_history_buckets_return;


namespace detail { class market_history_api_impl; }


class market_history_api
{
   public:
      market_history_api();
      ~market_history_api();

      DECLARE_API(
         (get_ticker)
         (get_volume)
         (get_order_book)
         (get_trade_history)
         (get_recent_trades)
         (get_market_history)
         (get_market_history_buckets)
      )

   private:
      std::unique_ptr< detail::market_history_api_impl > my;
};

} } } // amalgam::plugins::market_history

FC_REFLECT( amalgam::plugins::market_history::get_ticker_return,
            (latest)(lowest_ask)(highest_bid)(percent_change)(amalgam_volume)(abd_volume) )

FC_REFLECT( amalgam::plugins::market_history::get_volume_return,
            (amalgam_volume)(abd_volume) )

FC_REFLECT( amalgam::plugins::market_history::order,
            (order_price)(real_price)(amalgam)(abd)(created) )

FC_REFLECT( amalgam::plugins::market_history::order_book,
            (bids)(asks) )

FC_REFLECT( amalgam::plugins::market_history::get_order_book_args,
            (limit) )

FC_REFLECT( amalgam::plugins::market_history::market_trade,
            (date)(current_pays)(open_pays) )

FC_REFLECT( amalgam::plugins::market_history::get_trade_history_args,
            (start)(end)(limit) )

FC_REFLECT( amalgam::plugins::market_history::get_recent_trades_args,
            (limit) )

FC_REFLECT( amalgam::plugins::market_history::get_market_history_args,
            (bucket_seconds)(start)(end) )
