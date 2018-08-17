#include <amalgam/follow/follow_operations.hpp>

#include <amalgam/protocol/operation_util_impl.hpp>

namespace amalgam { namespace follow {

void follow_operation::validate()const
{
   FC_ASSERT( follower != following, "You cannot follow yourself" );
}

void reblog_operation::validate()const
{
   FC_ASSERT( account != author, "You cannot reblog your own content" );
}

} } //amalgam::follow

DEFINE_OPERATION_TYPE( amalgam::follow::follow_plugin_operation )
