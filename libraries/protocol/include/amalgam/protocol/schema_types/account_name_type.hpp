
#pragma once

#include <amalgam/schema/abstract_schema.hpp>
#include <amalgam/schema/schema_impl.hpp>

#include <amalgam/protocol/types.hpp>

namespace amalgam { namespace schema { namespace detail {

//////////////////////////////////////////////
// account_name_type                        //
//////////////////////////////////////////////

struct schema_account_name_type_impl
   : public abstract_schema
{
   AMALGAM_SCHEMA_CLASS_BODY( schema_account_name_type_impl )
};

}

template<>
struct schema_reflect< amalgam::protocol::account_name_type >
{
   typedef detail::schema_account_name_type_impl           schema_impl_type;
};

} }

namespace fc {

template<>
struct get_typename< amalgam::protocol::account_name_type >
{
   static const char* name()
   {
      return "amalgam::protocol::account_name_type";
   }
};

}
