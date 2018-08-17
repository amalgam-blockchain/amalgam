#include <amalgam/witness/witness_operations.hpp>

#include <amalgam/protocol/operation_util_impl.hpp>

namespace amalgam { namespace witness {

void enable_content_editing_operation::validate()const
{
   chain::validate_account_name( account );
}

} } // amalgam::witness

DEFINE_OPERATION_TYPE( amalgam::witness::witness_plugin_operation )
