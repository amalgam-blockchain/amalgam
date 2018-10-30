#pragma once
#include <amalgam/chain/account_object.hpp>
#include <amalgam/chain/block_summary_object.hpp>
#include <amalgam/chain/database.hpp>
#include <amalgam/chain/global_property_object.hpp>
#include <amalgam/chain/history_object.hpp>
#include <amalgam/chain/amalgam_objects.hpp>
#include <amalgam/chain/transaction_object.hpp>
#include <amalgam/chain/witness_objects.hpp>

#include <amalgam/witness/witness_objects.hpp>

namespace amalgam { namespace app {

using namespace amalgam::chain;

typedef chain::change_recovery_account_request_object  change_recovery_account_request_api_obj;
typedef chain::block_summary_object                    block_summary_api_obj;
typedef chain::convert_request_object                  convert_request_api_obj;
typedef chain::escrow_object                           escrow_api_obj;
typedef chain::limit_order_object                      limit_order_api_obj;
typedef chain::withdraw_vesting_route_object           withdraw_vesting_route_api_obj;
typedef chain::decline_voting_rights_request_object    decline_voting_rights_request_api_obj;
typedef chain::witness_vote_object                     witness_vote_api_obj;
typedef chain::witness_schedule_object                 witness_schedule_api_obj;
typedef chain::vesting_delegation_object               vesting_delegation_api_obj;
typedef chain::vesting_delegation_expiration_object    vesting_delegation_expiration_api_obj;
typedef witness::account_bandwidth_object              account_bandwidth_api_obj;

struct account_api_obj
{
   account_api_obj( const chain::account_object& a, const chain::database& db ) :
      id( a.id ),
      name( a.name ),
      memo_key( a.memo_key ),
      json_metadata( to_string( a.json_metadata ) ),
      proxy( a.proxy ),
      last_account_update( a.last_account_update ),
      created( a.created ),
      mined( a.mined ),
      recovery_account( a.recovery_account ),
      last_account_recovery( a.last_account_recovery ),
      can_vote( a.can_vote ),
      balance( a.balance ),
      savings_balance( a.savings_balance ),
      abd_balance( a.abd_balance ),
      abd_seconds( a.abd_seconds ),
      abd_seconds_last_update( a.abd_seconds_last_update ),
      abd_last_interest_payment( a.abd_last_interest_payment ),
      savings_abd_balance( a.savings_abd_balance ),
      savings_abd_seconds( a.savings_abd_seconds ),
      savings_abd_seconds_last_update( a.savings_abd_seconds_last_update ),
      savings_abd_last_interest_payment( a.savings_abd_last_interest_payment ),
      savings_withdraw_requests( a.savings_withdraw_requests ),
      vesting_shares( a.vesting_shares ),
      delegated_vesting_shares( a.delegated_vesting_shares ),
      received_vesting_shares( a.received_vesting_shares ),
      vesting_withdraw_rate( a.vesting_withdraw_rate ),
      next_vesting_withdrawal( a.next_vesting_withdrawal ),
      withdrawn( a.withdrawn ),
      to_withdraw( a.to_withdraw ),
      withdraw_routes( a.withdraw_routes ),
      witnesses_voted_for( a.witnesses_voted_for ),
      last_post( a.last_post ),
      last_root_post( a.last_root_post )
   {
      size_t n = a.proxied_vsf_votes.size();
      proxied_vsf_votes.reserve( n );
      for( size_t i=0; i<n; i++ )
         proxied_vsf_votes.push_back( a.proxied_vsf_votes[i] );

      const auto& auth = db.get< account_authority_object, by_account >( name );
      owner = authority( auth.owner );
      active = authority( auth.active );
      posting = authority( auth.posting );
      last_owner_update = auth.last_owner_update;

      if( db.has_index< witness::account_bandwidth_index >() )
      {
         auto forum_bandwidth = db.find< witness::account_bandwidth_object, witness::by_account_bandwidth_type >( boost::make_tuple( name, witness::bandwidth_type::forum ) );

         if( forum_bandwidth != nullptr )
         {
            average_bandwidth = forum_bandwidth->average_bandwidth;
            lifetime_bandwidth = forum_bandwidth->lifetime_bandwidth;
            last_bandwidth_update = forum_bandwidth->last_bandwidth_update;
         }

         auto market_bandwidth = db.find< witness::account_bandwidth_object, witness::by_account_bandwidth_type >( boost::make_tuple( name, witness::bandwidth_type::market ) );

         if( market_bandwidth != nullptr )
         {
            average_market_bandwidth = market_bandwidth->average_bandwidth;
            lifetime_market_bandwidth = market_bandwidth->lifetime_bandwidth;
            last_market_bandwidth_update = market_bandwidth->last_bandwidth_update;
         }
      }
   }


   account_api_obj(){}

   account_id_type   id;

   account_name_type name;
   authority         owner;
   authority         active;
   authority         posting;
   public_key_type   memo_key;
   string            json_metadata;
   account_name_type proxy;

   time_point_sec    last_owner_update;
   time_point_sec    last_account_update;

   time_point_sec    created;
   bool              mined = false;
   account_name_type recovery_account;
   time_point_sec    last_account_recovery;

   bool              can_vote = false;

   asset             balance;
   asset             savings_balance;

   asset             abd_balance;
   uint128_t         abd_seconds;
   time_point_sec    abd_seconds_last_update;
   time_point_sec    abd_last_interest_payment;

   asset             savings_abd_balance;
   uint128_t         savings_abd_seconds;
   time_point_sec    savings_abd_seconds_last_update;
   time_point_sec    savings_abd_last_interest_payment;

   uint8_t           savings_withdraw_requests = 0;

   asset             vesting_shares;
   asset             delegated_vesting_shares;
   asset             received_vesting_shares;
   asset             vesting_withdraw_rate;
   time_point_sec    next_vesting_withdrawal;
   share_type        withdrawn;
   share_type        to_withdraw;
   uint16_t          withdraw_routes = 0;

   vector< share_type > proxied_vsf_votes;

   uint16_t          witnesses_voted_for;

   share_type        average_bandwidth = 0;
   share_type        lifetime_bandwidth = 0;
   time_point_sec    last_bandwidth_update;

   share_type        average_market_bandwidth = 0;
   share_type        lifetime_market_bandwidth = 0;
   time_point_sec    last_market_bandwidth_update;

   time_point_sec    last_post;
   time_point_sec    last_root_post;
};

struct owner_authority_history_api_obj
{
   owner_authority_history_api_obj( const chain::owner_authority_history_object& o ) :
      id( o.id ),
      account( o.account ),
      previous_owner_authority( authority( o.previous_owner_authority ) ),
      last_valid_time( o.last_valid_time )
   {}

   owner_authority_history_api_obj() {}

   owner_authority_history_id_type  id;

   account_name_type                account;
   authority                        previous_owner_authority;
   time_point_sec                   last_valid_time;
};

struct account_recovery_request_api_obj
{
   account_recovery_request_api_obj( const chain::account_recovery_request_object& o ) :
      id( o.id ),
      account_to_recover( o.account_to_recover ),
      new_owner_authority( authority( o.new_owner_authority ) ),
      expires( o.expires )
   {}

   account_recovery_request_api_obj() {}

   account_recovery_request_id_type id;
   account_name_type                account_to_recover;
   authority                        new_owner_authority;
   time_point_sec                   expires;
};

struct account_history_api_obj
{

};

struct savings_withdraw_api_obj
{
   savings_withdraw_api_obj( const chain::savings_withdraw_object& o ) :
      id( o.id ),
      from( o.from ),
      to( o.to ),
      memo( to_string( o.memo ) ),
      request_id( o.request_id ),
      amount( o.amount ),
      complete( o.complete )
   {}

   savings_withdraw_api_obj() {}

   savings_withdraw_id_type   id;
   account_name_type          from;
   account_name_type          to;
   string                     memo;
   uint32_t                   request_id = 0;
   asset                      amount;
   time_point_sec             complete;
};

struct feed_history_api_obj
{
   feed_history_api_obj( const chain::feed_history_object& f ) :
      id( f.id ),
      current_median_history( f.current_median_history ),
      price_history( f.price_history.begin(), f.price_history.end() )
   {}

   feed_history_api_obj() {}

   feed_history_id_type id;
   price                current_median_history;
   deque< price >       price_history;
};

struct witness_api_obj
{
   witness_api_obj( const chain::witness_object& w ) :
      id( w.id ),
      owner( w.owner ),
      created( w.created ),
      url( to_string( w.url ) ),
      total_missed( w.total_missed ),
      last_aslot( w.last_aslot ),
      last_confirmed_block_num( w.last_confirmed_block_num ),
      signing_key( w.signing_key ),
      props( w.props ),
      abd_exchange_rate( w.abd_exchange_rate ),
      last_abd_exchange_update( w.last_abd_exchange_update ),
      votes( w.votes ),
      virtual_last_update( w.virtual_last_update ),
      virtual_position( w.virtual_position ),
      virtual_scheduled_time( w.virtual_scheduled_time ),
      running_version( w.running_version ),
      hardfork_version_vote( w.hardfork_version_vote ),
      hardfork_time_vote( w.hardfork_time_vote )
   {}

   witness_api_obj() {}

   witness_id_type   id;
   account_name_type owner;
   time_point_sec    created;
   string            url;
   uint32_t          total_missed = 0;
   uint64_t          last_aslot = 0;
   uint64_t          last_confirmed_block_num = 0;
   public_key_type   signing_key;
   chain_properties  props;
   price             abd_exchange_rate;
   time_point_sec    last_abd_exchange_update;
   share_type        votes;
   fc::uint128       virtual_last_update;
   fc::uint128       virtual_position;
   fc::uint128       virtual_scheduled_time;
   version           running_version;
   hardfork_version  hardfork_version_vote;
   time_point_sec    hardfork_time_vote;
};

struct signed_block_api_obj : public signed_block
{
   signed_block_api_obj( const signed_block& block ) : signed_block( block )
   {
      block_id = id();
      signing_key = signee();
      transaction_ids.reserve( transactions.size() );
      for( const signed_transaction& tx : transactions )
         transaction_ids.push_back( tx.id() );
   }
   signed_block_api_obj() {}

   block_id_type                 block_id;
   public_key_type               signing_key;
   vector< transaction_id_type > transaction_ids;
};

struct dynamic_global_property_api_obj : public dynamic_global_property_object
{
   dynamic_global_property_api_obj( const dynamic_global_property_object& gpo, const chain::database& db ) :
      dynamic_global_property_object( gpo )
   {
      if( db.has_index< witness::reserve_ratio_index >() )
      {
         const auto& r = db.find( witness::reserve_ratio_id_type() );

         if( BOOST_LIKELY( r != nullptr ) )
         {
            current_reserve_ratio = r->current_reserve_ratio;
            average_block_size = r->average_block_size;
            max_virtual_bandwidth = r->max_virtual_bandwidth;
         }
      }
   }

   dynamic_global_property_api_obj( const dynamic_global_property_object& gpo ) :
      dynamic_global_property_object( gpo ) {}

   dynamic_global_property_api_obj() {}

   uint32_t    current_reserve_ratio = 0;
   uint64_t    average_block_size = 0;
   uint128_t   max_virtual_bandwidth = 0;
};

} } // amalgam::app

FC_REFLECT( amalgam::app::account_api_obj,
             (id)(name)(owner)(active)(posting)(memo_key)(json_metadata)(proxy)(last_owner_update)(last_account_update)
             (created)(mined)
             (recovery_account)(last_account_recovery)
             (can_vote)
             (balance)
             (savings_balance)
             (abd_balance)(abd_seconds)(abd_seconds_last_update)(abd_last_interest_payment)
             (savings_abd_balance)(savings_abd_seconds)(savings_abd_seconds_last_update)(savings_abd_last_interest_payment)(savings_withdraw_requests)
             (vesting_shares)(delegated_vesting_shares)(received_vesting_shares)(vesting_withdraw_rate)(next_vesting_withdrawal)(withdrawn)(to_withdraw)(withdraw_routes)
             (proxied_vsf_votes)(witnesses_voted_for)
             (average_bandwidth)(lifetime_bandwidth)(last_bandwidth_update)
             (average_market_bandwidth)(lifetime_market_bandwidth)(last_market_bandwidth_update)
             (last_post)(last_root_post)
          )

FC_REFLECT( amalgam::app::owner_authority_history_api_obj,
             (id)
             (account)
             (previous_owner_authority)
             (last_valid_time)
          )

FC_REFLECT( amalgam::app::account_recovery_request_api_obj,
             (id)
             (account_to_recover)
             (new_owner_authority)
             (expires)
          )

FC_REFLECT( amalgam::app::savings_withdraw_api_obj,
             (id)
             (from)
             (to)
             (memo)
             (request_id)
             (amount)
             (complete)
          )

FC_REFLECT( amalgam::app::feed_history_api_obj,
             (id)
             (current_median_history)
             (price_history)
          )

FC_REFLECT( amalgam::app::witness_api_obj,
             (id)
             (owner)
             (created)
             (url)(votes)(virtual_last_update)(virtual_position)(virtual_scheduled_time)(total_missed)
             (last_aslot)(last_confirmed_block_num)(signing_key)
             (props)
             (abd_exchange_rate)(last_abd_exchange_update)
             (running_version)
             (hardfork_version_vote)(hardfork_time_vote)
          )

FC_REFLECT_DERIVED( amalgam::app::signed_block_api_obj, (amalgam::protocol::signed_block),
                     (block_id)
                     (signing_key)
                     (transaction_ids)
                  )

FC_REFLECT_DERIVED( amalgam::app::dynamic_global_property_api_obj, (amalgam::chain::dynamic_global_property_object),
                     (current_reserve_ratio)
                     (average_block_size)
                     (max_virtual_bandwidth)
                  )
