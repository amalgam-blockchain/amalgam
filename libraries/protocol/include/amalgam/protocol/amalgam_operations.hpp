#pragma once
#include <amalgam/protocol/base.hpp>
#include <amalgam/protocol/block_header.hpp>
#include <amalgam/protocol/asset.hpp>

#include <fc/utf8.hpp>

namespace amalgam { namespace protocol {

   inline void validate_account_name( const string& name )
   {
      FC_ASSERT( is_valid_account_name( name ), "Account name ${n} is invalid", ("n", name) );
   }

   struct account_create_operation : public base_operation
   {
      asset             fee;
      account_name_type creator;
      account_name_type new_account_name;
      authority         owner;
      authority         active;
      authority         posting;
      public_key_type   memo_key;
      string            json_metadata;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(creator); }
   };


   struct account_update_operation : public base_operation
   {
      account_name_type             account;
      optional< authority >         owner;
      optional< authority >         active;
      optional< authority >         posting;
      public_key_type               memo_key;
      string                        json_metadata;

      void validate()const;

      void get_required_owner_authorities( flat_set<account_name_type>& a )const
      { if( owner ) a.insert( account ); }

      void get_required_active_authorities( flat_set<account_name_type>& a )const
      { if( !owner ) a.insert( account ); }
   };


   /**
    * @ingroup operations
    *
    * @brief Transfers AMALGAM from one account to another.
    */
   struct transfer_operation : public base_operation
   {
      account_name_type from;
      /// Account to transfer asset to
      account_name_type to;
      /// The amount of asset to transfer from @ref from to @ref to
      asset             amount;

      /// The memo is plain-text, any encryption on the memo is up to
      /// a higher level protocol.
      string            memo;

      void              validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ if(amount.symbol != VESTS_SYMBOL) a.insert(from); }
      void get_required_owner_authorities( flat_set<account_name_type>& a )const { if(amount.symbol == VESTS_SYMBOL) a.insert(from); }
   };


   /**
    *  The purpose of this operation is to enable someone to send money contingently to
    *  another individual. The funds leave the *from* account and go into a temporary balance
    *  where they are held until *from* releases it to *to* or *to* refunds it to *from*.
    *
    *  In the event of a dispute the *agent* can divide the funds between the to/from account.
    *  Disputes can be raised any time before or on the dispute deadline time, after the escrow
    *  has been approved by all parties.
    *
    *  This operation only creates a proposed escrow transfer. Both the *agent* and *to* must
    *  agree to the terms of the arrangement by approving the escrow.
    *
    *  The escrow agent is paid the fee on approval of all parties. It is up to the escrow agent
    *  to determine the fee.
    *
    *  Escrow transactions are uniquely identified by 'from' and 'escrow_id', the 'escrow_id' is defined
    *  by the sender.
    */
   struct escrow_transfer_operation : public base_operation
   {
      account_name_type from;
      account_name_type to;
      account_name_type agent;
      uint32_t          escrow_id = 30;

      asset             abd_amount = asset( 0, ABD_SYMBOL );
      asset             amalgam_amount = asset( 0, AMALGAM_SYMBOL );
      asset             fee;

      time_point_sec    ratification_deadline;
      time_point_sec    escrow_expiration;

      string            json_meta;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(from); }
   };


   /**
    *  The agent and to accounts must approve an escrow transaction for it to be valid on
    *  the blockchain. Once a part approves the escrow, the cannot revoke their approval.
    *  Subsequent escrow approve operations, regardless of the approval, will be rejected.
    */
   struct escrow_approve_operation : public base_operation
   {
      account_name_type from;
      account_name_type to;
      account_name_type agent;
      account_name_type who; // Either to or agent

      uint32_t          escrow_id = 30;
      bool              approve = true;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(who); }
   };


   /**
    *  If either the sender or receiver of an escrow payment has an issue, they can
    *  raise it for dispute. Once a payment is in dispute, the agent has authority over
    *  who gets what.
    */
   struct escrow_dispute_operation : public base_operation
   {
      account_name_type from;
      account_name_type to;
      account_name_type agent;
      account_name_type who;

      uint32_t          escrow_id = 30;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(who); }
   };


   /**
    *  This operation can be used by anyone associated with the escrow transfer to
    *  release funds if they have permission.
    *
    *  The permission scheme is as follows:
    *  If there is no dispute and escrow has not expired, either party can release funds to the other.
    *  If escrow expires and there is no dispute, either party can release funds to either party.
    *  If there is a dispute regardless of expiration, the agent can release funds to either party
    *     following whichever agreement was in place between the parties.
    */
   struct escrow_release_operation : public base_operation
   {
      account_name_type from;
      account_name_type to; ///< the original 'to'
      account_name_type agent;
      account_name_type who; ///< the account that is attempting to release the funds, determines valid 'receiver'
      account_name_type receiver; ///< the account that should receive funds (might be from, might be to)

      uint32_t          escrow_id = 30;
      asset             abd_amount = asset( 0, ABD_SYMBOL ); ///< the amount of abd to release
      asset             amalgam_amount = asset( 0, AMALGAM_SYMBOL ); ///< the amount of amalgam to release

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(who); }
   };


   /**
    *  This operation converts AMALGAM into VFS (Vesting Fund Shares) at
    *  the current exchange rate. With this operation it is possible to
    *  give another account vesting shares so that faucets can
    *  pre-fund new accounts with vesting shares.
    */
   struct transfer_to_vesting_operation : public base_operation
   {
      account_name_type from;
      account_name_type to; ///< if null, then same as from
      asset             amount; ///< must be AMALGAM

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(from); }
   };


   /**
    * At any given point in time an account can be withdrawing from their
    * vesting shares. A user may change the number of shares they wish to
    * cash out at any time between 0 and their total vesting stake.
    *
    * After applying this operation, vesting_shares will be withdrawn
    * at a rate of vesting_shares/104 per week for two years starting
    * one week after this operation is included in the blockchain.
    *
    * This operation is not valid if the user has no vesting shares.
    */
   struct withdraw_vesting_operation : public base_operation
   {
      account_name_type account;
      asset             vesting_shares;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(account); }
   };


   /**
    * Allows an account to setup a vesting withdraw but with the additional
    * request for the funds to be transferred directly to another account's
    * balance rather than the withdrawing account. In addition, those funds
    * can be immediately vested again, circumventing the conversion from
    * vests to amalgam and back, guaranteeing they maintain their value.
    */
   struct set_withdraw_vesting_route_operation : public base_operation
   {
      account_name_type from_account;
      account_name_type to_account;
      uint16_t          percent = 0;
      bool              auto_vest = false;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const { a.insert( from_account ); }
   };


   /**
    * Witnesses must vote on how to set certain chain properties to ensure a smooth
    * and well functioning network.  Any time @owner is in the active set of witnesses these
    * properties will be used to control the blockchain configuration.
    */
   struct chain_properties
   {
      /**
       *  This fee, paid in AMALGAM, is converted into VESTING SHARES for the new account. Accounts
       *  without vesting shares cannot earn usage rations and therefore are powerless. This minimum
       *  fee requires all accounts to have some kind of commitment to the network that includes the
       *  ability to vote and make transactions.
       */
      asset             account_creation_fee =
         asset( AMALGAM_MIN_ACCOUNT_CREATION_FEE, AMALGAM_SYMBOL );

      /**
       *  This witnesses vote for the maximum_block_size which is used by the network
       *  to tune rate limiting and capacity
       */
      uint32_t          maximum_block_size = AMALGAM_MIN_BLOCK_SIZE_LIMIT * 2;
      uint16_t          abd_interest_rate  = AMALGAM_DEFAULT_ABD_INTEREST_RATE;

      void validate()const
      {
         FC_ASSERT( account_creation_fee.amount >= AMALGAM_MIN_ACCOUNT_CREATION_FEE);
         FC_ASSERT( maximum_block_size >= AMALGAM_MIN_BLOCK_SIZE_LIMIT);
         FC_ASSERT( abd_interest_rate >= 0 );
         FC_ASSERT( abd_interest_rate <= AMALGAM_100_PERCENT );
      }
   };


   /**
    *  Users who wish to become a witness must pay a fee acceptable to
    *  the current witnesses to apply for the position and allow voting
    *  to begin.
    *
    *  If the owner isn't a witness they will become a witness.  Witnesses
    *  are charged a fee equal to 1 weeks worth of witness pay which in
    *  turn is derived from the current share supply.  The fee is
    *  only applied if the owner is not already a witness.
    *
    *  If the block_signing_key is null then the witness is removed from
    *  contention.  The network will pick the top 21 witnesses for
    *  producing blocks.
    */
   struct witness_update_operation : public base_operation
   {
      account_name_type owner;
      string            url;
      public_key_type   block_signing_key;
      chain_properties  props;
      asset             fee; ///< the fee paid to register a new witness, should be 10x current block production pay

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(owner); }
   };


   /**
    * All accounts with a VFS can vote for or against any witness.
    *
    * If a proxy is specified then all existing votes are removed.
    */
   struct account_witness_vote_operation : public base_operation
   {
      account_name_type account;
      account_name_type witness;
      bool              approve = true;

      void validate() const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(account); }
   };


   struct account_witness_proxy_operation : public base_operation
   {
      account_name_type account;
      account_name_type proxy;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(account); }
   };


   /**
    * @brief provides a generic way to add higher level protocols on top of witness consensus
    * @ingroup operations
    *
    * There is no validation for this operation other than that required auths are valid
    */
   struct custom_operation : public base_operation
   {
      flat_set< account_name_type > required_auths;
      uint16_t                      id = 0;
      vector< char >                data;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_auths ) a.insert(i); }
   };


   /** serves the same purpose as custom_operation but also supports required posting authorities. Unlike custom_operation,
    * this operation is designed to be human readable/developer friendly.
    **/
   struct custom_json_operation : public base_operation
   {
      flat_set< account_name_type > required_auths;
      flat_set< account_name_type > required_posting_auths;
      string                        id; ///< must be less than 32 characters long
      string                        json; ///< must be proper utf8 / JSON string.

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_auths ) a.insert(i); }
      void get_required_posting_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_posting_auths ) a.insert(i); }
   };


   struct custom_binary_operation : public base_operation
   {
      flat_set< account_name_type > required_owner_auths;
      flat_set< account_name_type > required_active_auths;
      flat_set< account_name_type > required_posting_auths;
      vector< authority >           required_auths;

      string                        id; ///< must be less than 32 characters long
      vector< char >                data;

      void validate()const;
      void get_required_owner_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_owner_auths ) a.insert(i); }
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_active_auths ) a.insert(i); }
      void get_required_posting_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_posting_auths ) a.insert(i); }
      void get_required_authorities( vector< authority >& a )const{ for( const auto& i : required_auths ) a.push_back( i ); }
   };


   /**
    *  Feeds can only be published by the top N witnesses which are included in every round and are
    *  used to define the exchange rate between amalgam and the dollar.
    */
   struct feed_publish_operation : public base_operation
   {
      account_name_type publisher;
      price             exchange_rate;

      void  validate()const;
      void  get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(publisher); }
   };


   /**
    *  This operation instructs the blockchain to start a conversion between AMALGAM and ABD,
    *  The funds are deposited after AMALGAM_CONVERSION_DELAY
    */
   struct convert_operation : public base_operation
   {
      account_name_type owner;
      uint32_t          requestid = 0;
      asset             amount;

      void  validate()const;
      void  get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(owner); }
   };


   /**
    * This operation creates a limit order and matches it against existing open orders.
    */
   struct limit_order_create_operation : public base_operation
   {
      account_name_type owner;
      uint32_t          orderid = 0; /// an ID assigned by owner, must be unique
      asset             amount_to_sell;
      asset             min_to_receive;
      bool              fill_or_kill = false;
      time_point_sec    expiration = time_point_sec::maximum();

      void  validate()const;
      void  get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(owner); }

      price             get_price()const { return amount_to_sell / min_to_receive; }

      pair< asset_symbol_type, asset_symbol_type > get_market()const
      {
         return amount_to_sell.symbol < min_to_receive.symbol ?
                std::make_pair(amount_to_sell.symbol, min_to_receive.symbol) :
                std::make_pair(min_to_receive.symbol, amount_to_sell.symbol);
      }
   };


   /**
    *  This operation is identical to limit_order_create except it serializes the price rather
    *  than calculating it from other fields.
    */
   struct limit_order_create2_operation : public base_operation
   {
      account_name_type owner;
      uint32_t          orderid = 0; /// an ID assigned by owner, must be unique
      asset             amount_to_sell;
      bool              fill_or_kill = false;
      price             exchange_rate;
      time_point_sec    expiration = time_point_sec::maximum();

      void  validate()const;
      void  get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(owner); }

      price             get_price()const { return exchange_rate; }

      pair< asset_symbol_type, asset_symbol_type > get_market()const
      {
         return exchange_rate.base.symbol < exchange_rate.quote.symbol ?
                std::make_pair(exchange_rate.base.symbol, exchange_rate.quote.symbol) :
                std::make_pair(exchange_rate.quote.symbol, exchange_rate.base.symbol);
      }
   };


   /**
    *  Cancels an order and returns the balance to owner.
    */
   struct limit_order_cancel_operation : public base_operation
   {
      account_name_type owner;
      uint32_t          orderid = 0;

      void  validate()const;
      void  get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(owner); }
   };


   /**
    * All account recovery requests come from a listed recovery account. This
    * is secure based on the assumption that only a trusted account should be
    * a recovery account. It is the responsibility of the recovery account to
    * verify the identity of the account holder of the account to recover by
    * whichever means they have agreed upon. The blockchain assumes identity
    * has been verified when this operation is broadcast.
    *
    * This operation creates an account recovery request which the account to
    * recover has 24 hours to respond to before the request expires and is
    * invalidated.
    *
    * There can only be one active recovery request per account at any one time.
    * Pushing this operation for an account to recover when it already has
    * an active request will either update the request to a new new owner authority
    * and extend the request expiration to 24 hours from the current head block
    * time or it will delete the request. To cancel a request, simply set the
    * weight threshold of the new owner authority to 0, making it an open authority.
    *
    * Additionally, the new owner authority must be satisfiable. In other words,
    * the sum of the key weights must be greater than or equal to the weight
    * threshold.
    *
    * This operation only needs to be signed by the the recovery account.
    * The account to recover confirms its identity to the blockchain in
    * the recover account operation.
    */
   struct request_account_recovery_operation : public base_operation
   {
      account_name_type recovery_account;       ///< The recovery account is listed as the recovery account on the account to recover.

      account_name_type account_to_recover;     ///< The account to recover. This is likely due to a compromised owner authority.

      authority         new_owner_authority;    ///< The new owner authority the account to recover wishes to have. This is secret
                                                ///< known by the account to recover and will be confirmed in a recover_account_operation

      extensions_type   extensions;             ///< Extensions. Not currently used.

      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert( recovery_account ); }

      void validate() const;
   };


   /**
    * Recover an account to a new authority using a previous authority and verification
    * of the recovery account as proof of identity. This operation can only succeed
    * if there was a recovery request sent by the account's recover account.
    *
    * In order to recover the account, the account holder must provide proof
    * of past ownership and proof of identity to the recovery account. Being able
    * to satisfy an owner authority that was used in the past 30 days is sufficient
    * to prove past ownership. The get_owner_history function in the database API
    * returns past owner authorities that are valid for account recovery.
    *
    * Proving identity is an off chain contract between the account holder and
    * the recovery account. The recovery request contains a new authority which
    * must be satisfied by the account holder to regain control. The actual process
    * of verifying authority may become complicated, but that is an application
    * level concern, not a blockchain concern.
    *
    * This operation requires both the past and future owner authorities in the
    * operation because neither of them can be derived from the current chain state.
    * The operation must be signed by keys that satisfy both the new owner authority
    * and the recent owner authority. Failing either fails the operation entirely.
    *
    * If a recovery request was made inadvertantly, the account holder should
    * contact the recovery account to have the request deleted.
    *
    * The two setp combination of the account recovery request and recover is
    * safe because the recovery account never has access to secrets of the account
    * to recover. They simply act as an on chain endorsement of off chain identity.
    * In other systems, a fork would be required to enforce such off chain state.
    * Additionally, an account cannot be permanently recovered to the wrong account.
    * While any owner authority from the past 30 days can be used, including a compromised
    * authority, the account can be continually recovered until the recovery account
    * is confident a combination of uncompromised authorities were used to
    * recover the account. The actual process of verifying authority may become
    * complicated, but that is an application level concern, not the blockchain's
    * concern.
    */
   struct recover_account_operation : public base_operation
   {
      account_name_type account_to_recover;        ///< The account to be recovered

      authority         new_owner_authority;       ///< The new owner authority as specified in the request account recovery operation.

      authority         recent_owner_authority;    ///< A previous owner authority that the account holder will use to prove past ownership of the account to be recovered.

      extensions_type   extensions;                ///< Extensions. Not currently used.

      void get_required_authorities( vector< authority >& a )const
      {
         a.push_back( new_owner_authority );
         a.push_back( recent_owner_authority );
      }

      void validate() const;
   };


   /**
    * Each account lists another account as their recovery account.
    * The recovery account has the ability to create account_recovery_requests
    * for the account to recover. An account can change their recovery account
    * at any time with a 30 day delay. This delay is to prevent
    * an attacker from changing the recovery account to a malicious account
    * during an attack. These 30 days match the 30 days that an
    * owner authority is valid for recovery purposes.
    *
    * On account creation the recovery account is set either to the creator of
    * the account (The account that pays the creation fee and is a signer on the transaction)
    * or to the empty string if the account was mined. An account with no recovery
    * has the top voted witness as a recovery account, at the time the recover
    * request is created. Note: This does mean the effective recovery account
    * of an account with no listed recovery account can change at any time as
    * witness vote weights. The top voted witness is explicitly the most trusted
    * witness according to stake.
    */
   struct change_recovery_account_operation : public base_operation
   {
      account_name_type account_to_recover;     ///< The account that would be recovered in case of compromise
      account_name_type new_recovery_account;   ///< The account that creates the recover request
      extensions_type   extensions;             ///< Extensions. Not currently used.

      void get_required_owner_authorities( flat_set<account_name_type>& a )const{ a.insert( account_to_recover ); }
      void validate() const;
   };


   struct transfer_to_savings_operation : public base_operation {
      account_name_type from;
      account_name_type to;
      asset             amount;
      string            memo;

      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert( from ); }
      void validate() const;
   };


   struct transfer_from_savings_operation : public base_operation {
      account_name_type from;
      uint32_t          request_id = 0;
      account_name_type to;
      asset             amount;
      string            memo;

      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert( from ); }
      void validate() const;
   };


   struct cancel_transfer_from_savings_operation : public base_operation {
      account_name_type from;
      uint32_t          request_id = 0;

      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert( from ); }
      void validate() const;
   };


   struct decline_voting_rights_operation : public base_operation
   {
      account_name_type account;
      bool              decline = true;

      void get_required_owner_authorities( flat_set<account_name_type>& a )const{ a.insert( account ); }
      void validate() const;
   };

   /**
    * Delegate vesting shares from one account to the other. The vesting shares are still owned
    * by the original account, but content voting rights and bandwidth allocation are transferred
    * to the receiving account. This sets the delegation to `vesting_shares`, increasing it or
    * decreasing it as needed. (i.e. a delegation of 0 removes the delegation)
    *
    * When a delegation is removed the shares are placed in limbo for a week to prevent a satoshi
    * of VESTS from voting on the same content twice.
    */
   struct delegate_vesting_shares_operation : public base_operation
   {
      account_name_type delegator;        ///< The account delegating vesting shares
      account_name_type delegatee;        ///< The account receiving vesting shares
      asset             vesting_shares;   ///< The amount of vesting shares delegated

      void get_required_active_authorities( flat_set< account_name_type >& a ) const { a.insert( delegator ); }
      void validate() const;
   };
   
   struct tbd1_operation : public base_operation
   {
      account_name_type from;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(from); }
   };
   
   struct tbd2_operation : public base_operation
   {
      account_name_type from;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(from); }
   };
   
   struct tbd3_operation : public base_operation
   {
      account_name_type from;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(from); }
   };
   
   struct tbd4_operation : public base_operation
   {
      account_name_type from;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(from); }
   };
   
   struct tbd5_operation : public base_operation
   {
      account_name_type from;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(from); }
   };
   
   struct tbd6_operation : public base_operation
   {
      account_name_type from;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(from); }
   };
   
   struct tbd7_operation : public base_operation
   {
      account_name_type from;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(from); }
   };
   
   struct tbd8_operation : public base_operation
   {
      account_name_type from;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(from); }
   };
   
   struct tbd9_operation : public base_operation
   {
      account_name_type from;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(from); }
   };
   
   struct tbd10_operation : public base_operation
   {
      account_name_type from;

      void validate()const;
      void get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(from); }
   };
} } // amalgam::protocol


FC_REFLECT( amalgam::protocol::transfer_to_savings_operation, (from)(to)(amount)(memo) )
FC_REFLECT( amalgam::protocol::transfer_from_savings_operation, (from)(request_id)(to)(amount)(memo) )
FC_REFLECT( amalgam::protocol::cancel_transfer_from_savings_operation, (from)(request_id) )

FC_REFLECT( amalgam::protocol::convert_operation, (owner)(requestid)(amount) )
FC_REFLECT( amalgam::protocol::feed_publish_operation, (publisher)(exchange_rate) )
FC_REFLECT( amalgam::protocol::chain_properties, (account_creation_fee)(maximum_block_size)(abd_interest_rate) );

FC_REFLECT( amalgam::protocol::account_create_operation,
            (fee)
            (creator)
            (new_account_name)
            (owner)
            (active)
            (posting)
            (memo_key)
            (json_metadata) )

FC_REFLECT( amalgam::protocol::account_update_operation,
            (account)
            (owner)
            (active)
            (posting)
            (memo_key)
            (json_metadata) )

FC_REFLECT( amalgam::protocol::transfer_operation, (from)(to)(amount)(memo) )
FC_REFLECT( amalgam::protocol::transfer_to_vesting_operation, (from)(to)(amount) )
FC_REFLECT( amalgam::protocol::withdraw_vesting_operation, (account)(vesting_shares) )
FC_REFLECT( amalgam::protocol::set_withdraw_vesting_route_operation, (from_account)(to_account)(percent)(auto_vest) )
FC_REFLECT( amalgam::protocol::witness_update_operation, (owner)(url)(block_signing_key)(props)(fee) )
FC_REFLECT( amalgam::protocol::account_witness_vote_operation, (account)(witness)(approve) )
FC_REFLECT( amalgam::protocol::account_witness_proxy_operation, (account)(proxy) )
FC_REFLECT( amalgam::protocol::custom_operation, (required_auths)(id)(data) )
FC_REFLECT( amalgam::protocol::custom_json_operation, (required_auths)(required_posting_auths)(id)(json) )
FC_REFLECT( amalgam::protocol::custom_binary_operation, (required_owner_auths)(required_active_auths)(required_posting_auths)(required_auths)(id)(data) )
FC_REFLECT( amalgam::protocol::limit_order_create_operation, (owner)(orderid)(amount_to_sell)(min_to_receive)(fill_or_kill)(expiration) )
FC_REFLECT( amalgam::protocol::limit_order_create2_operation, (owner)(orderid)(amount_to_sell)(exchange_rate)(fill_or_kill)(expiration) )
FC_REFLECT( amalgam::protocol::limit_order_cancel_operation, (owner)(orderid) )
FC_REFLECT( amalgam::protocol::escrow_transfer_operation, (from)(to)(abd_amount)(amalgam_amount)(escrow_id)(agent)(fee)(json_meta)(ratification_deadline)(escrow_expiration) );
FC_REFLECT( amalgam::protocol::escrow_approve_operation, (from)(to)(agent)(who)(escrow_id)(approve) );
FC_REFLECT( amalgam::protocol::escrow_dispute_operation, (from)(to)(agent)(who)(escrow_id) );
FC_REFLECT( amalgam::protocol::escrow_release_operation, (from)(to)(agent)(who)(receiver)(escrow_id)(abd_amount)(amalgam_amount) );
FC_REFLECT( amalgam::protocol::request_account_recovery_operation, (recovery_account)(account_to_recover)(new_owner_authority)(extensions) );
FC_REFLECT( amalgam::protocol::recover_account_operation, (account_to_recover)(new_owner_authority)(recent_owner_authority)(extensions) );
FC_REFLECT( amalgam::protocol::change_recovery_account_operation, (account_to_recover)(new_recovery_account)(extensions) );
FC_REFLECT( amalgam::protocol::decline_voting_rights_operation, (account)(decline) );
FC_REFLECT( amalgam::protocol::delegate_vesting_shares_operation, (delegator)(delegatee)(vesting_shares) );
FC_REFLECT( amalgam::protocol::tbd1_operation, (from) );
FC_REFLECT( amalgam::protocol::tbd2_operation, (from) );
FC_REFLECT( amalgam::protocol::tbd3_operation, (from) );
FC_REFLECT( amalgam::protocol::tbd4_operation, (from) );
FC_REFLECT( amalgam::protocol::tbd5_operation, (from) );
FC_REFLECT( amalgam::protocol::tbd6_operation, (from) );
FC_REFLECT( amalgam::protocol::tbd7_operation, (from) );
FC_REFLECT( amalgam::protocol::tbd8_operation, (from) );
FC_REFLECT( amalgam::protocol::tbd9_operation, (from) );
FC_REFLECT( amalgam::protocol::tbd10_operation, (from) );
