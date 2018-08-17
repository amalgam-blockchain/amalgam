#pragma once

#include <amalgam/chain/evaluator.hpp>

#include <amalgam/private_message/private_message_operations.hpp>
#include <amalgam/private_message/private_message_plugin.hpp>

namespace amalgam { namespace private_message {

DEFINE_PLUGIN_EVALUATOR( private_message_plugin, amalgam::private_message::private_message_plugin_operation, private_message )

} }
