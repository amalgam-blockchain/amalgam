#pragma once
#include <amalgam/protocol/block.hpp>

#include <amalgam/plugins/p2p/p2p_plugin.hpp>
#include <amalgam/plugins/chain/chain_plugin.hpp>
#include <amalgam/plugins/json_rpc/utility.hpp>

#include <fc/optional.hpp>
#include <fc/variant.hpp>
#include <fc/vector.hpp>

#include <boost/thread/mutex.hpp>

namespace amalgam { namespace plugins { namespace network_broadcast_api {

using std::vector;
using fc::variant;
using fc::optional;
using amalgam::plugins::json_rpc::void_type;

using amalgam::protocol::signed_transaction;
using amalgam::protocol::transaction_id_type;
using amalgam::protocol::signed_block;

struct broadcast_transaction_args
{
   signed_transaction   trx;
   int32_t              max_block_age = -1;
};

typedef void_type broadcast_transaction_return;

struct broadcast_transaction_synchronous_args
{
   signed_transaction   trx;
   int32_t              max_block_age = -1;
};

struct broadcast_transaction_synchronous_return
{
   broadcast_transaction_synchronous_return() {}
   broadcast_transaction_synchronous_return( transaction_id_type txid, int32_t bn, int32_t tn, bool ex )
   : id(txid), block_num(bn), trx_num(tn), expired(ex) {}

   transaction_id_type   id;
   int32_t               block_num = 0;
   int32_t               trx_num   = 0;
   bool                  expired   = false;
};

struct broadcast_block_args
{
   signed_block   block;
};

typedef void_type broadcast_block_return;

namespace detail{ class network_broadcast_api_impl; }

class network_broadcast_api
{
   public:
      network_broadcast_api();
      ~network_broadcast_api();

      DECLARE_API(
         (broadcast_transaction)
         (broadcast_transaction_synchronous)
         (broadcast_block)
      )

   private:
      std::unique_ptr< detail::network_broadcast_api_impl > my;
};

} } } // amalgam::plugins::network_broadcast_api

FC_REFLECT( amalgam::plugins::network_broadcast_api::broadcast_transaction_args,
   (trx)(max_block_age) )

FC_REFLECT( amalgam::plugins::network_broadcast_api::broadcast_transaction_synchronous_args,
   (trx)(max_block_age) )

FC_REFLECT( amalgam::plugins::network_broadcast_api::broadcast_transaction_synchronous_return,
   (id)(block_num)(trx_num)(expired) )

FC_REFLECT( amalgam::plugins::network_broadcast_api::broadcast_block_args,
   (block) )
