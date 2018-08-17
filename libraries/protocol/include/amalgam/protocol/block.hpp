#pragma once
#include <amalgam/protocol/block_header.hpp>
#include <amalgam/protocol/transaction.hpp>

namespace amalgam { namespace protocol {

   struct signed_block : public signed_block_header
   {
      checksum_type calculate_merkle_root()const;
      vector<signed_transaction> transactions;
   };

} } // amalgam::protocol

FC_REFLECT_DERIVED( amalgam::protocol::signed_block, (amalgam::protocol::signed_block_header), (transactions) )
