
#pragma once

#include <memory>
#include <string>
#include <vector>

namespace amalgam { namespace app {

class abstract_plugin;
class application;

} }

namespace amalgam { namespace plugin {

void initialize_plugin_factories();
std::shared_ptr< amalgam::app::abstract_plugin > create_plugin( const std::string& name, amalgam::app::application* app );
std::vector< std::string > get_available_plugins();

} }
