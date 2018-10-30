#pragma once
#include <amalgam/app/applied_operation.hpp>
#include <amalgam/app/amalgam_api_objects.hpp>

#include <amalgam/chain/global_property_object.hpp>
#include <amalgam/chain/account_object.hpp>
#include <amalgam/chain/amalgam_objects.hpp>

namespace amalgam { namespace app {
   using std::string;
   using std::vector;

   struct extended_limit_order : public limit_order_api_obj
   {
      extended_limit_order(){}
      extended_limit_order( const limit_order_object& o ):limit_order_api_obj(o){}

      double real_price  = 0;
      bool   rewarded    = false;
   };

   struct extended_account : public account_api_obj
   {
      extended_account(){}
      extended_account( const account_object& a, const database& db ):account_api_obj( a, db ){}

      set<string>                             witness_votes;
   };

   struct candle_stick {
      time_point_sec  open_time;
      uint32_t        period = 0;
      double          high = 0;
      double          low = 0;
      double          open = 0;
      double          close = 0;
      double          amalgam_volume = 0;
      double          dollar_volume = 0;
   };

   struct order_history_item {
      time_point_sec time;
      string         type; // buy or sell
      asset          abd_quantity;
      asset          amalgam_quantity;
      double         real_price = 0;
   };

   struct market {
      vector<extended_limit_order> bids;
      vector<extended_limit_order> asks;
      vector<order_history_item>   history;
      vector<int>                  available_candlesticks;
      vector<int>                  available_zoom;
      int                          current_candlestick = 0;
      int                          current_zoom = 0;
      vector<candle_stick>         price_history;
   };

} }

FC_REFLECT_DERIVED( amalgam::app::extended_account,
                   (amalgam::app::account_api_obj),
                   (witness_votes) )

FC_REFLECT_DERIVED( amalgam::app::extended_limit_order, (amalgam::app::limit_order_api_obj), (real_price)(rewarded) )
FC_REFLECT( amalgam::app::order_history_item, (time)(type)(abd_quantity)(amalgam_quantity)(real_price) );
FC_REFLECT( amalgam::app::market, (bids)(asks)(history)(price_history)(available_candlesticks)(available_zoom)(current_candlestick)(current_zoom) )
FC_REFLECT( amalgam::app::candle_stick, (open_time)(period)(high)(low)(open)(close)(amalgam_volume)(dollar_volume) );
