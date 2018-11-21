#pragma once
#include <amalgam/plugins/json_rpc/utility.hpp>

#include <amalgam/protocol/types.hpp>

#include <fc/optional.hpp>
#include <fc/variant.hpp>
#include <fc/vector.hpp>

namespace amalgam { namespace plugins { namespace account_by_key {

namespace detail
{
   class account_by_key_api_impl;
}

struct get_key_references_args
{
   std::vector< amalgam::protocol::public_key_type > keys;
};

typedef std::vector< std::vector< amalgam::protocol::account_name_type > > get_key_references_return;

class account_by_key_api
{
   public:
      account_by_key_api();
      ~account_by_key_api();

      DECLARE_API( (get_key_references) )

   private:
      std::unique_ptr< detail::account_by_key_api_impl > my;
};

} } } // amalgam::plugins::account_by_key

FC_REFLECT( amalgam::plugins::account_by_key::get_key_references_args,
   (keys) )
