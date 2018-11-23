#pragma once

#include <amalgam/plugins/json_rpc/utility.hpp>

#include <amalgam/plugins/database_api/database_api_args.hpp>
#include <amalgam/plugins/database_api/database_api_objects.hpp>

#define DATABASE_API_SINGLE_QUERY_LIMIT 1000

namespace amalgam { namespace plugins { namespace database_api {

class database_api_impl;

class database_api
{
   public:
      database_api();
      ~database_api();

      DECLARE_API(

         /////////////////////////////
         // Blocks and transactions //
         /////////////////////////////

         /**
         * @brief Retrieve a block header
         * @param block_num Height of the block whose header should be returned
         * @return header of the referenced block, or null if no matching block was found
         */
         (get_block_header)

         /**
         * @brief Retrieve a full, signed block
         * @param block_num Height of the block to be returned
         * @return the referenced block, or null if no matching block was found
         */
         (get_block)

         /**
          *  @brief Get sequence of operations included/generated within a particular block
          *  @param block_num Height of the block whose generated virtual operations should be returned
          *  @param only_virtual Whether to only include virtual operations in returned results (default: true)
          *  @return sequence of operations included/generated within the block
          */
         (get_ops_in_block)

         /**
          *  @brief Retrieve a transaction
          *  @param id Identifier of the transaction
          *  @return the referenced transaction, or null if no matching transaction was found
          */
         (get_transaction)

         /////////////
         // Globals //
         /////////////

         /**
         * @brief Retrieve compile-time constants
         */
         (get_config)

         /**
          * @brief Return version information and chain_id of running node
          */
         (get_version)

         /**
          * @brief Retrieve the current @ref dynamic_global_property_object
          */
         (get_dynamic_global_properties)

         (get_chain_properties)
         (get_witness_schedule)
         (get_reserve_ratio)
         (get_hardfork_properties)
         (get_current_price_feed)
         (get_feed_history)

         ///////////////
         // Witnesses //
         ///////////////

         (list_witnesses)
         (find_witnesses)
         (list_witness_votes)
         (get_witness_votes_by_account)
         (get_witness_votes_by_witness)
         (get_witnesses_by_vote)
         (get_witness_count)
         (get_active_witnesses)

         //////////////
         // Accounts //
         //////////////

         /**
         * @brief List accounts ordered by specified key
         *
         */
         (list_accounts)

         /**
         * @brief Find accounts by primary key (account name)
         */
         (find_accounts)

         (get_account_count)

         /**
          *  Account operations have sequence numbers from 0 to N where N is the most recent operation. This method
          *  returns operations in the range [from-limit, from]
          *
          *  @param from - the absolute sequence number, -1 means most recent, limit is the number of operations before from.
          *  @param limit - the maximum number of items that can be queried (0 to 1000], must be less than from
          */
         (get_account_history)

         (get_account_bandwidth)

         (list_owner_histories)
         (find_owner_histories)
         (list_account_recovery_requests)
         (find_account_recovery_requests)
         (list_change_recovery_account_requests)
         (find_change_recovery_account_requests)
         (list_escrows)
         (find_escrows)
         (get_escrow)
         (list_withdraw_vesting_routes)
         (find_withdraw_vesting_routes)
         (list_savings_withdrawals)
         (find_savings_withdrawals_from)
         (find_savings_withdrawals_to)
         (list_vesting_delegations)
         (find_vesting_delegations)
         (list_vesting_delegation_expirations)
         (find_vesting_delegation_expirations)
         (list_abd_conversion_requests)
         (find_abd_conversion_requests)
         (list_decline_voting_rights_requests)
         (find_decline_voting_rights_requests)

         ////////////
         // Market //
         ////////////
         (list_limit_orders)
         (find_limit_orders)

         ////////////////////////////
         // Authority / validation //
         ////////////////////////////

         /// @brief Get a hexdump of the serialized binary form of a transaction
         (get_transaction_hex)

         /**
         *  This API will take a partially signed transaction and a set of public keys that the owner has the ability to sign for
         *  and return the minimal subset of public keys that should add signatures to the transaction.
         */
         (get_required_signatures)

         /**
         *  This method will return the set of all public keys that could possibly sign for a given transaction.  This call can
         *  be used by wallets to filter their set of public keys to just the relevant subset prior to calling @ref get_required_signatures
         *  to get the minimum subset.
         */
         (get_potential_signatures)

         /**
         * @return true of the @ref trx has all of the required signatures, otherwise throws an exception
         */
         (verify_authority)

         /**
         * @return true if the signers have enough authority to authorize an account
         */
         (verify_account_authority)

         /*
          * This is a general purpose API that checks signatures against accounts for an arbitrary sha256 hash
          * using the existing authority structures in Amalgam
          */
         (verify_signatures)
      )

   private:
      std::unique_ptr< database_api_impl > my;
};

} } } //amalgam::plugins::database_api

