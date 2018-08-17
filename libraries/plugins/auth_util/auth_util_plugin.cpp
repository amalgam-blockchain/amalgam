

#include <amalgam/plugins/auth_util/auth_util_api.hpp>
#include <amalgam/plugins/auth_util/auth_util_plugin.hpp>

#include <string>

namespace amalgam { namespace plugin { namespace auth_util {

auth_util_plugin::auth_util_plugin( application* app ) : plugin( app ) {}
auth_util_plugin::~auth_util_plugin() {}

std::string auth_util_plugin::plugin_name()const
{
   return "auth_util";
}

void auth_util_plugin::plugin_initialize( const boost::program_options::variables_map& options )
{
}

void auth_util_plugin::plugin_startup()
{
   app().register_api_factory< auth_util_api >( "auth_util_api" );
}

void auth_util_plugin::plugin_shutdown()
{
}

} } } // amalgam::plugin::auth_util

AMALGAM_DEFINE_PLUGIN( auth_util, amalgam::plugin::auth_util::auth_util_plugin )
