#include <amalgam/protocol/amalgam_operations.hpp>

#include <fc/io/json.hpp>

#include <locale>

namespace amalgam { namespace protocol {

   void validate_auth_size( const authority& a )
   {
      size_t size = a.account_auths.size() + a.key_auths.size();
      FC_ASSERT( size <= AMALGAM_MAX_AUTHORITY_MEMBERSHIP, "Authority membership exceeded. Max: 10 Current: ${n}", ("n", size) );
   }

   void account_create_operation::validate() const
   {
      validate_account_name( new_account_name );
      FC_ASSERT( is_asset_type( fee, AMALGAM_SYMBOL ), "Account creation fee must be AMALGAM" );
      owner.validate();
      active.validate();

      if ( json_metadata.size() > 0 )
      {
         FC_ASSERT( fc::is_utf8(json_metadata), "JSON Metadata not formatted in UTF8" );
         FC_ASSERT( fc::json::is_valid(json_metadata), "JSON Metadata not valid JSON" );
      }
      FC_ASSERT( fee >= asset( 0, AMALGAM_SYMBOL ), "Account creation fee cannot be negative" );
   }

   void account_update_operation::validate() const
   {
      validate_account_name( account );
      /*if( owner )
         owner->validate();
      if( active )
         active->validate();
      if( posting )
         posting->validate();*/

      if ( json_metadata.size() > 0 )
      {
         FC_ASSERT( fc::is_utf8(json_metadata), "JSON Metadata not formatted in UTF8" );
         FC_ASSERT( fc::json::is_valid(json_metadata), "JSON Metadata not valid JSON" );
      }
   }

   void transfer_operation::validate() const
   { try {
      validate_account_name( from );
      validate_account_name( to );
      FC_ASSERT( amount.symbol != VESTS_SYMBOL, "transferring of Solid Amalgam is not allowed." );
      FC_ASSERT( amount.amount > 0, "Cannot transfer a negative amount (aka: stealing)" );
      FC_ASSERT( memo.size() < AMALGAM_MAX_MEMO_SIZE, "Memo is too large" );
      FC_ASSERT( fc::is_utf8( memo ), "Memo is not UTF8" );
   } FC_CAPTURE_AND_RETHROW( (*this) ) }

   void transfer_to_vesting_operation::validate() const
   {
      validate_account_name( from );
      FC_ASSERT( is_asset_type( amount, AMALGAM_SYMBOL ), "Amount must be AMALGAM" );
      if ( to != account_name_type() ) validate_account_name( to );
      FC_ASSERT( amount.amount > 0, "Must transfer a nonzero amount" );
      FC_ASSERT( memo.size() < AMALGAM_MAX_MEMO_SIZE, "Memo is too large" );
      FC_ASSERT( fc::is_utf8( memo ), "Memo is not UTF8" );
   }

   void withdraw_vesting_operation::validate() const
   {
      validate_account_name( account );
      FC_ASSERT( is_asset_type( vesting_shares, VESTS_SYMBOL), "Amount must be VESTS"  );
   }

   void set_withdraw_vesting_route_operation::validate() const
   {
      validate_account_name( from_account );
      validate_account_name( to_account );
      FC_ASSERT( 0 <= percent && percent <= AMALGAM_100_PERCENT, "Percent must be valid amalgam percent" );
   }

   void witness_update_operation::validate() const
   {
      validate_account_name( owner );

      FC_ASSERT( url.size() <= AMALGAM_MAX_WITNESS_URL_LENGTH, "URL is too long" );

      FC_ASSERT( url.size() > 0, "URL size must be greater than 0" );
      FC_ASSERT( fc::is_utf8( url ), "URL is not valid UTF8" );
      FC_ASSERT( fee >= asset( 0, AMALGAM_SYMBOL ), "Fee cannot be negative" );
      props.validate();
   }

   void witness_set_properties_operation::validate() const
   {
      validate_account_name( owner );

      // current signing key must be present
      FC_ASSERT( props.find( "key" ) != props.end(), "No signing key provided" );

      auto itr = props.find( "account_creation_fee" );
      if( itr != props.end() )
      {
         asset account_creation_fee;
         fc::raw::unpack_from_vector( itr->second, account_creation_fee );
         FC_ASSERT( account_creation_fee.symbol == AMALGAM_SYMBOL, "account_creation_fee must be in AMALGAM" );
         FC_ASSERT( account_creation_fee.amount >= AMALGAM_MIN_ACCOUNT_CREATION_FEE, "account_creation_fee smaller than minimum account creation fee" );
      }

      itr = props.find( "maximum_block_size" );
      if( itr != props.end() )
      {
         uint32_t maximum_block_size;
         fc::raw::unpack_from_vector( itr->second, maximum_block_size );
         FC_ASSERT( maximum_block_size >= AMALGAM_MIN_BLOCK_SIZE_LIMIT, "maximum_block_size smaller than minimum max block size" );
      }

      itr = props.find( "abd_interest_rate" );
      if( itr != props.end() )
      {
         uint16_t abd_interest_rate;
         fc::raw::unpack_from_vector( itr->second, abd_interest_rate );
         FC_ASSERT( abd_interest_rate >= 0, "abd_interest_rate must be positive" );
         FC_ASSERT( abd_interest_rate <= AMALGAM_100_PERCENT, "abd_interest_rate must not exceed 100%" );
      }

      itr = props.find( "new_signing_key" );
      if( itr != props.end() )
      {
         public_key_type signing_key;
         fc::raw::unpack_from_vector( itr->second, signing_key );
         FC_UNUSED( signing_key ); // This tests the deserialization of the key
      }

      itr = props.find( "abd_exchange_rate" );
      if( itr != props.end() )
      {
         price abd_exchange_rate;
         fc::raw::unpack_from_vector( itr->second, abd_exchange_rate );
         FC_ASSERT( ( is_asset_type( abd_exchange_rate.base, ABD_SYMBOL ) && is_asset_type( abd_exchange_rate.quote, AMALGAM_SYMBOL ) ),
            "Price feed must be a AMALGAM/ABD price" );
         abd_exchange_rate.validate();
      }

      itr = props.find( "url" );
      if( itr != props.end() )
      {
         std::string url;
         fc::raw::unpack_from_vector< std::string >( itr->second, url );

         FC_ASSERT( url.size() <= AMALGAM_MAX_WITNESS_URL_LENGTH, "URL is too long" );
         FC_ASSERT( url.size() > 0, "URL size must be greater than 0" );
         FC_ASSERT( fc::is_utf8( url ), "URL is not valid UTF8" );
      }
   }

   void account_witness_vote_operation::validate() const
   {
      validate_account_name( account );
      validate_account_name( witness );
   }

   void account_witness_proxy_operation::validate() const
   {
      validate_account_name( account );
      if( proxy.size() )
         validate_account_name( proxy );
      FC_ASSERT( proxy != account, "Cannot proxy to self" );
   }

   void custom_operation::validate() const {
      /// required auth accounts are the ones whose bandwidth is consumed
      FC_ASSERT( required_auths.size() > 0, "at least one account must be specified" );
   }
   
   void custom_json_operation::validate() const {
      /// required auth accounts are the ones whose bandwidth is consumed
      FC_ASSERT( (required_auths.size() + required_posting_auths.size()) > 0, "at least one account must be specified" );
      FC_ASSERT( id.size() <= 32, "id is too long" );
      FC_ASSERT( fc::is_utf8(json), "JSON Metadata not formatted in UTF8" );
      FC_ASSERT( fc::json::is_valid(json), "JSON Metadata not valid JSON" );
   }
   
   void custom_binary_operation::validate() const {
      /// required auth accounts are the ones whose bandwidth is consumed
      FC_ASSERT( (required_owner_auths.size() + required_active_auths.size() + required_posting_auths.size()) > 0, "at least one account must be specified" );
      FC_ASSERT( id.size() <= 32, "id is too long" );
      for( const auto& a : required_auths ) a.validate();
   }

   void feed_publish_operation::validate()const
   {
      validate_account_name( publisher );
      FC_ASSERT( ( is_asset_type( exchange_rate.base, AMALGAM_SYMBOL ) && is_asset_type( exchange_rate.quote, ABD_SYMBOL ) )
         || ( is_asset_type( exchange_rate.base, ABD_SYMBOL ) && is_asset_type( exchange_rate.quote, AMALGAM_SYMBOL ) ),
         "Price feed must be a AMALGAM/ABD price" );
      exchange_rate.validate();
   }

   void limit_order_create_operation::validate()const
   {
      validate_account_name( owner );

      FC_ASSERT( ( is_asset_type( amount_to_sell, AMALGAM_SYMBOL ) && is_asset_type( min_to_receive, ABD_SYMBOL ) )
         || ( is_asset_type( amount_to_sell, ABD_SYMBOL ) && is_asset_type( min_to_receive, AMALGAM_SYMBOL ) ),
         "Limit order must be for the AMALGAM:ABD market" );

      (amount_to_sell / min_to_receive).validate();
   }

   void limit_order_create2_operation::validate()const
   {
      validate_account_name( owner );

      FC_ASSERT( amount_to_sell.symbol == exchange_rate.base.symbol, "Sell asset must be the base of the price" );
      exchange_rate.validate();

      FC_ASSERT( ( is_asset_type( amount_to_sell, AMALGAM_SYMBOL ) && is_asset_type( exchange_rate.quote, ABD_SYMBOL ) ) ||
                 ( is_asset_type( amount_to_sell, ABD_SYMBOL ) && is_asset_type( exchange_rate.quote, AMALGAM_SYMBOL ) ),
                 "Limit order must be for the AMALGAM:ABD market" );

      FC_ASSERT( (amount_to_sell * exchange_rate).amount > 0, "Amount to sell cannot round to 0 when traded" );
   }

   void limit_order_cancel_operation::validate()const
   {
      validate_account_name( owner );
   }

   void convert_operation::validate()const
   {
      validate_account_name( owner );
      /// only allow conversion from ABD to AMALGAM, allowing the opposite can enable traders to abuse
      /// market fluctuations through converting large quantities without moving the price.
      FC_ASSERT( is_asset_type( amount, ABD_SYMBOL ), "Can only convert ABD to AMALGAM" );
      FC_ASSERT( amount.amount > 0, "Must convert some ABD" );
   }

   void escrow_transfer_operation::validate()const
   {
      validate_account_name( from );
      validate_account_name( to );
      validate_account_name( agent );
      FC_ASSERT( fee.amount >= 0, "fee cannot be negative" );
      FC_ASSERT( abd_amount.amount >= 0, "abd amount cannot be negative" );
      FC_ASSERT( amalgam_amount.amount >= 0, "amalgam amount cannot be negative" );
      FC_ASSERT( abd_amount.amount > 0 || amalgam_amount.amount > 0, "escrow must transfer a non-zero amount" );
      FC_ASSERT( from != agent && to != agent, "agent must be a third party" );
      FC_ASSERT( (fee.symbol == AMALGAM_SYMBOL) || (fee.symbol == ABD_SYMBOL), "fee must be AMALGAM or ABD" );
      FC_ASSERT( abd_amount.symbol == ABD_SYMBOL, "abd amount must contain ABD" );
      FC_ASSERT( amalgam_amount.symbol == AMALGAM_SYMBOL, "amalgam amount must contain AMALGAM" );
      FC_ASSERT( ratification_deadline < escrow_expiration, "ratification deadline must be before escrow expiration" );
      if ( json_meta.size() > 0 )
      {
         FC_ASSERT( fc::is_utf8(json_meta), "JSON Metadata not formatted in UTF8" );
         FC_ASSERT( fc::json::is_valid(json_meta), "JSON Metadata not valid JSON" );
      }
   }

   void escrow_approve_operation::validate()const
   {
      validate_account_name( from );
      validate_account_name( to );
      validate_account_name( agent );
      validate_account_name( who );
      FC_ASSERT( who == to || who == agent, "to or agent must approve escrow" );
   }

   void escrow_dispute_operation::validate()const
   {
      validate_account_name( from );
      validate_account_name( to );
      validate_account_name( agent );
      validate_account_name( who );
      FC_ASSERT( who == from || who == to, "who must be from or to" );
   }

   void escrow_release_operation::validate()const
   {
      validate_account_name( from );
      validate_account_name( to );
      validate_account_name( agent );
      validate_account_name( who );
      validate_account_name( receiver );
      FC_ASSERT( who == from || who == to || who == agent, "who must be from or to or agent" );
      FC_ASSERT( receiver == from || receiver == to, "receiver must be from or to" );
      FC_ASSERT( abd_amount.amount >= 0, "abd amount cannot be negative" );
      FC_ASSERT( amalgam_amount.amount >= 0, "amalgam amount cannot be negative" );
      FC_ASSERT( abd_amount.amount > 0 || amalgam_amount.amount > 0, "escrow must release a non-zero amount" );
      FC_ASSERT( abd_amount.symbol == ABD_SYMBOL, "abd amount must contain ABD" );
      FC_ASSERT( amalgam_amount.symbol == AMALGAM_SYMBOL, "amalgam amount must contain AMALGAM" );
   }

   void request_account_recovery_operation::validate()const
   {
      validate_account_name( recovery_account );
      validate_account_name( account_to_recover );
      new_owner_authority.validate();
   }

   void recover_account_operation::validate()const
   {
      validate_account_name( account_to_recover );
      FC_ASSERT( !( new_owner_authority == recent_owner_authority ), "Cannot set new owner authority to the recent owner authority" );
      FC_ASSERT( !new_owner_authority.is_impossible(), "new owner authority cannot be impossible" );
      FC_ASSERT( !recent_owner_authority.is_impossible(), "recent owner authority cannot be impossible" );
      FC_ASSERT( new_owner_authority.weight_threshold, "new owner authority cannot be trivial" );
      new_owner_authority.validate();
      recent_owner_authority.validate();
   }

   void change_recovery_account_operation::validate()const
   {
      validate_account_name( account_to_recover );
      validate_account_name( new_recovery_account );
   }

   void transfer_to_savings_operation::validate()const {
      validate_account_name( from );
      validate_account_name( to );
      FC_ASSERT( amount.amount > 0 );
      FC_ASSERT( amount.symbol == AMALGAM_SYMBOL || amount.symbol == ABD_SYMBOL );
      FC_ASSERT( memo.size() < AMALGAM_MAX_MEMO_SIZE, "Memo is too large" );
      FC_ASSERT( fc::is_utf8( memo ), "Memo is not UTF8" );
   }
   void transfer_from_savings_operation::validate()const {
      validate_account_name( from );
      validate_account_name( to );
      FC_ASSERT( amount.amount > 0 );
      FC_ASSERT( amount.symbol == AMALGAM_SYMBOL || amount.symbol == ABD_SYMBOL );
      FC_ASSERT( memo.size() < AMALGAM_MAX_MEMO_SIZE, "Memo is too large" );
      FC_ASSERT( fc::is_utf8( memo ), "Memo is not UTF8" );
   }
   void cancel_transfer_from_savings_operation::validate()const {
      validate_account_name( from );
   }

   void decline_voting_rights_operation::validate()const
   {
      validate_account_name( account );
   }

   void delegate_vesting_shares_operation::validate()const
   {
      validate_account_name( delegator );
      validate_account_name( delegatee );
      FC_ASSERT( delegator != delegatee, "You cannot delegate VESTS to yourself" );
      FC_ASSERT( is_asset_type( vesting_shares, VESTS_SYMBOL ), "Delegation must be VESTS" );
      FC_ASSERT( vesting_shares >= asset( 0, VESTS_SYMBOL ), "Delegation cannot be negative" );
   }

   void tbd1_operation::validate()const
   {
   }

   void tbd2_operation::validate()const
   {
   }

   void tbd3_operation::validate()const
   {
   }

   void tbd4_operation::validate()const
   {
   }

   void tbd5_operation::validate()const
   {
   }

   void tbd6_operation::validate()const
   {
   }

   void tbd7_operation::validate()const
   {
   }

   void tbd8_operation::validate()const
   {
   }

   void tbd9_operation::validate()const
   {
   }

   void tbd10_operation::validate()const
   {
   }
} } // amalgam::protocol
