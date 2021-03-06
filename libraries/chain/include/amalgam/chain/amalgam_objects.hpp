#pragma once

#include <amalgam/protocol/authority.hpp>
#include <amalgam/protocol/amalgam_operations.hpp>

#include <amalgam/chain/amalgam_object_types.hpp>

#include <boost/multi_index/composite_key.hpp>
#include <boost/multiprecision/cpp_int.hpp>


namespace amalgam { namespace chain {

   using amalgam::protocol::asset;
   using amalgam::protocol::price;
   using amalgam::protocol::asset_symbol_type;
   using chainbase::t_deque;

   /**
    *  This object is used to track pending requests to convert abd to amalgam
    */
   class convert_request_object : public object< convert_request_object_type, convert_request_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         convert_request_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         convert_request_object(){}

         id_type           id;

         account_name_type owner;
         uint32_t          requestid = 0; ///< id set by owner, the owner,requestid pair must be unique
         asset             amount;
         time_point_sec    conversion_date; ///< at this time the feed_history_median_price * amount
   };


   class escrow_object : public object< escrow_object_type, escrow_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         escrow_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         escrow_object(){}

         id_type           id;

         uint32_t          escrow_id = 20;
         account_name_type from;
         account_name_type to;
         account_name_type agent;
         time_point_sec    ratification_deadline;
         time_point_sec    escrow_expiration;
         asset             abd_balance;
         asset             amalgam_balance;
         asset             pending_fee;
         bool              to_approved = false;
         bool              agent_approved = false;
         bool              disputed = false;

         bool              is_approved()const { return to_approved && agent_approved; }
   };


   class savings_withdraw_object : public object< savings_withdraw_object_type, savings_withdraw_object >
   {
      savings_withdraw_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         savings_withdraw_object( Constructor&& c, allocator< Allocator > a )
            :memo( a )
         {
            c( *this );
         }

         id_type           id;

         account_name_type from;
         account_name_type to;
         shared_string     memo;
         uint32_t          request_id = 0;
         asset             amount;
         time_point_sec    complete;
   };


   /**
    *  This object gets updated once per hour, on the hour
    */
   class feed_history_object  : public object< feed_history_object_type, feed_history_object >
   {
      feed_history_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         feed_history_object( Constructor&& c, allocator< Allocator > a )
            :price_history( a )
         {
            c( *this );
         }

         id_type                                   id;

         price                                     current_median_history; ///< the current median of the price history, used as the base for convert operations

         using t_price_history = t_deque< price >;

         t_deque< price >   price_history; ///< tracks this last week of median_feed one per hour
   };


   /**
    *  @brief an offer to sell a amount of a asset at a specified exchange rate by a certain time
    *  @ingroup object
    *  @ingroup protocol
    *  @ingroup market
    *
    *  This limit_order_objects are indexed by @ref expiration and is automatically deleted on the first block after expiration.
    */
   class limit_order_object : public object< limit_order_object_type, limit_order_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         limit_order_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         limit_order_object(){}

         id_type           id;

         time_point_sec    created;
         time_point_sec    expiration;
         account_name_type seller;
         uint32_t          orderid = 0;
         share_type        for_sale; ///< asset id is sell_price.base.symbol
         price             sell_price;

         pair< asset_symbol_type, asset_symbol_type > get_market()const
         {
            return sell_price.base.symbol < sell_price.quote.symbol ?
                std::make_pair( sell_price.base.symbol, sell_price.quote.symbol ) :
                std::make_pair( sell_price.quote.symbol, sell_price.base.symbol );
         }

         asset amount_for_sale()const   { return asset( for_sale, sell_price.base.symbol ); }
         asset amount_to_receive()const { return amount_for_sale() * sell_price; }
   };


   /**
    * @breif a route to send withdrawn vesting shares.
    */
   class withdraw_vesting_route_object : public object< withdraw_vesting_route_object_type, withdraw_vesting_route_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         withdraw_vesting_route_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         withdraw_vesting_route_object(){}

         id_type  id;

         account_name_type from_account;
         account_name_type to_account;
         uint16_t          percent = 0;
         bool              auto_vest = false;
   };


   class decline_voting_rights_request_object : public object< decline_voting_rights_request_object_type, decline_voting_rights_request_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         decline_voting_rights_request_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         decline_voting_rights_request_object(){}

         id_type           id;

         account_name_type account;
         time_point_sec    effective_date;
   };

   struct by_price;
   struct by_expiration;
   struct by_account;
   typedef multi_index_container<
      limit_order_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< limit_order_object, limit_order_id_type, &limit_order_object::id > >,
         ordered_non_unique< tag< by_expiration >, member< limit_order_object, time_point_sec, &limit_order_object::expiration > >,
         ordered_unique< tag< by_price >,
            composite_key< limit_order_object,
               member< limit_order_object, price, &limit_order_object::sell_price >,
               member< limit_order_object, limit_order_id_type, &limit_order_object::id >
            >,
            composite_key_compare< std::greater< price >, std::less< limit_order_id_type > >
         >,
         ordered_unique< tag< by_account >,
            composite_key< limit_order_object,
               member< limit_order_object, account_name_type, &limit_order_object::seller >,
               member< limit_order_object, uint32_t, &limit_order_object::orderid >
            >
         >
      >,
      allocator< limit_order_object >
   > limit_order_index;

   struct by_owner;
   struct by_conversion_date;
   typedef multi_index_container<
      convert_request_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< convert_request_object, convert_request_id_type, &convert_request_object::id > >,
         ordered_unique< tag< by_conversion_date >,
            composite_key< convert_request_object,
               member< convert_request_object, time_point_sec, &convert_request_object::conversion_date >,
               member< convert_request_object, convert_request_id_type, &convert_request_object::id >
            >
         >,
         ordered_unique< tag< by_owner >,
            composite_key< convert_request_object,
               member< convert_request_object, account_name_type, &convert_request_object::owner >,
               member< convert_request_object, uint32_t, &convert_request_object::requestid >
            >
         >
      >,
      allocator< convert_request_object >
   > convert_request_index;

   typedef multi_index_container<
      feed_history_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< feed_history_object, feed_history_id_type, &feed_history_object::id > >
      >,
      allocator< feed_history_object >
   > feed_history_index;

   struct by_withdraw_route;
   struct by_destination;
   typedef multi_index_container<
      withdraw_vesting_route_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< withdraw_vesting_route_object, withdraw_vesting_route_id_type, &withdraw_vesting_route_object::id > >,
         ordered_unique< tag< by_withdraw_route >,
            composite_key< withdraw_vesting_route_object,
               member< withdraw_vesting_route_object, account_name_type, &withdraw_vesting_route_object::from_account >,
               member< withdraw_vesting_route_object, account_name_type, &withdraw_vesting_route_object::to_account >
            >,
            composite_key_compare< std::less< account_name_type >, std::less< account_name_type > >
         >,
         ordered_unique< tag< by_destination >,
            composite_key< withdraw_vesting_route_object,
               member< withdraw_vesting_route_object, account_name_type, &withdraw_vesting_route_object::to_account >,
               member< withdraw_vesting_route_object, withdraw_vesting_route_id_type, &withdraw_vesting_route_object::id >
            >
         >
      >,
      allocator< withdraw_vesting_route_object >
   > withdraw_vesting_route_index;

   struct by_from_id;
   struct by_ratification_deadline;
   typedef multi_index_container<
      escrow_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< escrow_object, escrow_id_type, &escrow_object::id > >,
         ordered_unique< tag< by_from_id >,
            composite_key< escrow_object,
               member< escrow_object, account_name_type,  &escrow_object::from >,
               member< escrow_object, uint32_t, &escrow_object::escrow_id >
            >
         >,
         ordered_unique< tag< by_ratification_deadline >,
            composite_key< escrow_object,
               const_mem_fun< escrow_object, bool, &escrow_object::is_approved >,
               member< escrow_object, time_point_sec, &escrow_object::ratification_deadline >,
               member< escrow_object, escrow_id_type, &escrow_object::id >
            >,
            composite_key_compare< std::less< bool >, std::less< time_point_sec >, std::less< escrow_id_type > >
         >
      >,
      allocator< escrow_object >
   > escrow_index;

   struct by_from_rid;
   struct by_to_complete;
   struct by_complete_from_rid;
   typedef multi_index_container<
      savings_withdraw_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< savings_withdraw_object, savings_withdraw_id_type, &savings_withdraw_object::id > >,
         ordered_unique< tag< by_from_rid >,
            composite_key< savings_withdraw_object,
               member< savings_withdraw_object, account_name_type,  &savings_withdraw_object::from >,
               member< savings_withdraw_object, uint32_t, &savings_withdraw_object::request_id >
            >
         >,
         ordered_unique< tag< by_complete_from_rid >,
            composite_key< savings_withdraw_object,
               member< savings_withdraw_object, time_point_sec,  &savings_withdraw_object::complete >,
               member< savings_withdraw_object, account_name_type,  &savings_withdraw_object::from >,
               member< savings_withdraw_object, uint32_t, &savings_withdraw_object::request_id >
            >
         >,
         ordered_unique< tag< by_to_complete >,
            composite_key< savings_withdraw_object,
               member< savings_withdraw_object, account_name_type,  &savings_withdraw_object::to >,
               member< savings_withdraw_object, time_point_sec,  &savings_withdraw_object::complete >,
               member< savings_withdraw_object, savings_withdraw_id_type, &savings_withdraw_object::id >
            >
         >
      >,
      allocator< savings_withdraw_object >
   > savings_withdraw_index;

   struct by_account;
   struct by_effective_date;
   typedef multi_index_container<
      decline_voting_rights_request_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< decline_voting_rights_request_object, decline_voting_rights_request_id_type, &decline_voting_rights_request_object::id > >,
         ordered_unique< tag< by_account >,
            member< decline_voting_rights_request_object, account_name_type, &decline_voting_rights_request_object::account >
         >,
         ordered_unique< tag< by_effective_date >,
            composite_key< decline_voting_rights_request_object,
               member< decline_voting_rights_request_object, time_point_sec, &decline_voting_rights_request_object::effective_date >,
               member< decline_voting_rights_request_object, account_name_type, &decline_voting_rights_request_object::account >
            >,
            composite_key_compare< std::less< time_point_sec >, std::less< account_name_type > >
         >
      >,
      allocator< decline_voting_rights_request_object >
   > decline_voting_rights_request_index;

} } // amalgam::chain

#include <amalgam/chain/account_object.hpp>

FC_REFLECT( amalgam::chain::limit_order_object,
             (id)(created)(expiration)(seller)(orderid)(for_sale)(sell_price) )
CHAINBASE_SET_INDEX_TYPE( amalgam::chain::limit_order_object, amalgam::chain::limit_order_index )

FC_REFLECT( amalgam::chain::feed_history_object,
             (id)(current_median_history)(price_history) )
CHAINBASE_SET_INDEX_TYPE( amalgam::chain::feed_history_object, amalgam::chain::feed_history_index )

FC_REFLECT( amalgam::chain::convert_request_object,
             (id)(owner)(requestid)(amount)(conversion_date) )
CHAINBASE_SET_INDEX_TYPE( amalgam::chain::convert_request_object, amalgam::chain::convert_request_index )

FC_REFLECT( amalgam::chain::withdraw_vesting_route_object,
             (id)(from_account)(to_account)(percent)(auto_vest) )
CHAINBASE_SET_INDEX_TYPE( amalgam::chain::withdraw_vesting_route_object, amalgam::chain::withdraw_vesting_route_index )

FC_REFLECT( amalgam::chain::savings_withdraw_object,
             (id)(from)(to)(memo)(request_id)(amount)(complete) )
CHAINBASE_SET_INDEX_TYPE( amalgam::chain::savings_withdraw_object, amalgam::chain::savings_withdraw_index )

FC_REFLECT( amalgam::chain::escrow_object,
             (id)(escrow_id)(from)(to)(agent)
             (ratification_deadline)(escrow_expiration)
             (abd_balance)(amalgam_balance)(pending_fee)
             (to_approved)(agent_approved)(disputed) )
CHAINBASE_SET_INDEX_TYPE( amalgam::chain::escrow_object, amalgam::chain::escrow_index )

FC_REFLECT( amalgam::chain::decline_voting_rights_request_object,
             (id)(account)(effective_date) )
CHAINBASE_SET_INDEX_TYPE( amalgam::chain::decline_voting_rights_request_object, amalgam::chain::decline_voting_rights_request_index )
