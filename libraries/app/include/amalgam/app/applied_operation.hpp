#pragma once

#include <amalgam/protocol/operations.hpp>
#include <amalgam/chain/amalgam_object_types.hpp>

namespace amalgam { namespace app {

struct applied_operation
{
   applied_operation();
   applied_operation( const amalgam::chain::operation_object& op_obj );

   amalgam::protocol::transaction_id_type trx_id;
   uint32_t                               block = 0;
   uint32_t                               trx_in_block = 0;
   uint16_t                               op_in_trx = 0;
   uint64_t                               virtual_op = 0;
   fc::time_point_sec                     timestamp;
   amalgam::protocol::operation           op;
};

} }

FC_REFLECT( amalgam::app::applied_operation,
   (trx_id)
   (block)
   (trx_in_block)
   (op_in_trx)
   (virtual_op)
   (timestamp)
   (op)
)
