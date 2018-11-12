#pragma once
#include <amalgam/plugins/json_rpc/utility.hpp>

#include <amalgam/protocol/types.hpp>

#include <fc/optional.hpp>

namespace amalgam { namespace plugins { namespace chain {

namespace detail { class chain_api_impl; }

struct push_block_args
{
   amalgam::chain::signed_block block;
   bool                         currently_syncing = false;
};


struct push_block_return
{
   bool              success;
   optional<string>  error;
};

typedef amalgam::chain::signed_transaction push_transaction_args;

struct push_transaction_return
{
   bool              success;
   optional<string>  error;
};


class chain_api
{
   public:
      chain_api();
      ~chain_api();

      DECLARE_API(
         (push_block)
         (push_transaction) )
      
   private:
      std::unique_ptr< detail::chain_api_impl > my;
};

} } } // amalgam::plugins::chain

FC_REFLECT( amalgam::plugins::chain::push_block_args, (block)(currently_syncing) )
FC_REFLECT( amalgam::plugins::chain::push_block_return, (success)(error) )
FC_REFLECT( amalgam::plugins::chain::push_transaction_return, (success)(error) )
