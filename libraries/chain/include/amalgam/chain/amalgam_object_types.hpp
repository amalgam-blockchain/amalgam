#pragma once
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include <chainbase/chainbase.hpp>

#include <amalgam/protocol/types.hpp>
#include <amalgam/protocol/authority.hpp>

#include <amalgam/chain/buffer_type.hpp>

namespace amalgam { namespace chain {

namespace bip = chainbase::bip;
using namespace boost::multi_index;

using boost::multi_index_container;

using chainbase::object;
using chainbase::oid;
using chainbase::allocator;

using amalgam::protocol::block_id_type;
using amalgam::protocol::transaction_id_type;
using amalgam::protocol::chain_id_type;
using amalgam::protocol::account_name_type;
using amalgam::protocol::share_type;

using chainbase::shared_string;

inline std::string to_string( const shared_string& str ) { return std::string( str.begin(), str.end() ); }
inline void from_string( shared_string& out, const string& in ){ out.assign( in.begin(), in.end() ); }

struct by_id;
struct by_name;

enum object_type
{
   dynamic_global_property_object_type,
   account_object_type,
   account_authority_object_type,
   witness_object_type,
   transaction_object_type,
   block_summary_object_type,
   witness_schedule_object_type,
   witness_vote_object_type,
   limit_order_object_type,
   feed_history_object_type,
   convert_request_object_type,
   operation_object_type,
   account_history_object_type,
   hardfork_property_object_type,
   withdraw_vesting_route_object_type,
   owner_authority_history_object_type,
   account_recovery_request_object_type,
   change_recovery_account_request_object_type,
   escrow_object_type,
   savings_withdraw_object_type,
   decline_voting_rights_request_object_type,
   block_stats_object_type,
   vesting_delegation_object_type,
   vesting_delegation_expiration_object_type
};

class dynamic_global_property_object;
class account_object;
class account_authority_object;
class witness_object;
class transaction_object;
class block_summary_object;
class witness_schedule_object;
class witness_vote_object;
class limit_order_object;
class feed_history_object;
class convert_request_object;
class operation_object;
class account_history_object;
class hardfork_property_object;
class withdraw_vesting_route_object;
class owner_authority_history_object;
class account_recovery_request_object;
class change_recovery_account_request_object;
class escrow_object;
class savings_withdraw_object;
class decline_voting_rights_request_object;
class block_stats_object;
class vesting_delegation_object;
class vesting_delegation_expiration_object;

typedef oid< dynamic_global_property_object         > dynamic_global_property_id_type;
typedef oid< account_object                         > account_id_type;
typedef oid< account_authority_object               > account_authority_id_type;
typedef oid< witness_object                         > witness_id_type;
typedef oid< transaction_object                     > transaction_object_id_type;
typedef oid< block_summary_object                   > block_summary_id_type;
typedef oid< witness_schedule_object                > witness_schedule_id_type;
typedef oid< witness_vote_object                    > witness_vote_id_type;
typedef oid< limit_order_object                     > limit_order_id_type;
typedef oid< feed_history_object                    > feed_history_id_type;
typedef oid< convert_request_object                 > convert_request_id_type;
typedef oid< operation_object                       > operation_id_type;
typedef oid< account_history_object                 > account_history_id_type;
typedef oid< hardfork_property_object               > hardfork_property_id_type;
typedef oid< withdraw_vesting_route_object          > withdraw_vesting_route_id_type;
typedef oid< owner_authority_history_object         > owner_authority_history_id_type;
typedef oid< account_recovery_request_object        > account_recovery_request_id_type;
typedef oid< change_recovery_account_request_object > change_recovery_account_request_id_type;
typedef oid< escrow_object                          > escrow_id_type;
typedef oid< savings_withdraw_object                > savings_withdraw_id_type;
typedef oid< decline_voting_rights_request_object   > decline_voting_rights_request_id_type;
typedef oid< block_stats_object                     > block_stats_id_type;
typedef oid< vesting_delegation_object              > vesting_delegation_id_type;
typedef oid< vesting_delegation_expiration_object   > vesting_delegation_expiration_id_type;

enum bandwidth_type
{
   forum,   ///< Rate limiting for all forum related actions
   market   ///< Rate limiting for all other actions
};

} } //amalgam::chain

namespace fc
{
   class variant;
   inline void to_variant( const amalgam::chain::shared_string& s, variant& var )
   {
      var = fc::string( amalgam::chain::to_string( s ) );
   }

   inline void from_variant( const variant& var, amalgam::chain::shared_string& s )
   {
      auto str = var.as_string();
      s.assign( str.begin(), str.end() );
   }

   template<typename T>
   void to_variant( const chainbase::oid<T>& var,  variant& vo )
   {
      vo = var._id;
   }
   template<typename T>
   void from_variant( const variant& vo, chainbase::oid<T>& var )
   {
      var._id = vo.as_int64();
   }

   namespace raw {
      template<typename Stream, typename T>
      inline void pack( Stream& s, const chainbase::oid<T>& id )
      {
         s.write( (const char*)&id._id, sizeof(id._id) );
      }
      template<typename Stream, typename T>
      inline void unpack( Stream& s, chainbase::oid<T>& id )
      {
         s.read( (char*)&id._id, sizeof(id._id));
      }
#ifndef ENABLE_STD_ALLOCATOR
      template< typename T >
      inline T unpack_from_vector( const amalgam::chain::buffer_type& s )
      { try  {
         T tmp;
         if( s.size() ) {
         datastream<const char*>  ds( s.data(), size_t(s.size()) );
         fc::raw::unpack(ds,tmp);
         }
         return tmp;
      } FC_RETHROW_EXCEPTIONS( warn, "error unpacking ${type}", ("type",fc::get_typename<T>::name() ) ) }
#endif
   }
}

FC_REFLECT_ENUM( amalgam::chain::object_type,
                 (dynamic_global_property_object_type)
                 (account_object_type)
                 (account_authority_object_type)
                 (witness_object_type)
                 (transaction_object_type)
                 (block_summary_object_type)
                 (witness_schedule_object_type)
                 (witness_vote_object_type)
                 (limit_order_object_type)
                 (feed_history_object_type)
                 (convert_request_object_type)
                 (operation_object_type)
                 (account_history_object_type)
                 (hardfork_property_object_type)
                 (withdraw_vesting_route_object_type)
                 (owner_authority_history_object_type)
                 (account_recovery_request_object_type)
                 (change_recovery_account_request_object_type)
                 (escrow_object_type)
                 (savings_withdraw_object_type)
                 (decline_voting_rights_request_object_type)
                 (block_stats_object_type)
                 (vesting_delegation_object_type)
                 (vesting_delegation_expiration_object_type)
               )

#ifndef ENABLE_STD_ALLOCATOR
FC_REFLECT_TYPENAME( amalgam::chain::shared_string )
#endif

FC_REFLECT_ENUM( amalgam::chain::bandwidth_type, (forum)(market) )
