#pragma once
#include <fc/uint128.hpp>

#include <amalgam/chain/amalgam_object_types.hpp>

#include <amalgam/protocol/asset.hpp>

namespace amalgam { namespace chain {

   using amalgam::protocol::asset;
   using amalgam::protocol::price;

   /**
    * @class dynamic_global_property_object
    * @brief Maintains global state information
    * @ingroup object
    * @ingroup implementation
    *
    * This is an implementation detail. The values here are calculated during normal chain operations and reflect the
    * current values of global blockchain properties.
    */
   class dynamic_global_property_object : public object< dynamic_global_property_object_type, dynamic_global_property_object>
   {
      public:
         template< typename Constructor, typename Allocator >
         dynamic_global_property_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         dynamic_global_property_object(){}

         id_type           id;

         uint32_t          head_block_number = 0;
         block_id_type     head_block_id;
         time_point_sec    time;
         account_name_type current_witness;

         asset       virtual_supply             = asset( 0, AMALGAM_SYMBOL );
         asset       current_supply             = asset( 0, AMALGAM_SYMBOL );
         asset       confidential_supply        = asset( 0, AMALGAM_SYMBOL ); ///< total asset held in confidential balances
         asset       current_abd_supply         = asset( 0, ABD_SYMBOL );
         asset       confidential_abd_supply    = asset( 0, ABD_SYMBOL ); ///< total asset held in confidential balances
         asset       total_vesting_fund_amalgam = asset( 0, AMALGAM_SYMBOL );
         asset       total_vesting_shares       = asset( 0, VESTS_SYMBOL );

         price       get_vesting_share_price() const
         {
            if ( total_vesting_fund_amalgam.amount == 0 || total_vesting_shares.amount == 0 )
               return price ( asset( 1000, AMALGAM_SYMBOL ), asset( 1000000, VESTS_SYMBOL ) );

            return price( total_vesting_shares, total_vesting_fund_amalgam );
         }

         /**
          *  This property defines the interest rate that ABD deposits receive.
          */
         uint16_t abd_interest_rate = 0;

         uint16_t abd_print_rate = AMALGAM_100_PERCENT;

         /**
          *  Maximum block size is decided by the set of active witnesses which change every round.
          *  Each witness posts what they think the maximum size should be as part of their witness
          *  properties, the median size is chosen to be the maximum block size for the round.
          *
          *  @note the minimum value for maximum_block_size is defined by the protocol to prevent the
          *  network from getting stuck by witnesses attempting to set this too low.
          */
         uint32_t     maximum_block_size = 0;

         /**
          * The current absolute slot number.  Equal to the total
          * number of slots since genesis.  Also equal to the total
          * number of missed slots plus head_block_number.
          */
         uint64_t      current_aslot = 0;

         /**
          * used to compute witness participation.
          */
         fc::uint128_t recent_slots_filled;
         uint8_t       participation_count = 0; ///< Divide by 128 to compute participation percentage

         uint32_t last_irreversible_block_num = 0;
   };

   typedef multi_index_container<
      dynamic_global_property_object,
      indexed_by<
         ordered_unique< tag< by_id >,
            member< dynamic_global_property_object, dynamic_global_property_object::id_type, &dynamic_global_property_object::id > >
      >,
      allocator< dynamic_global_property_object >
   > dynamic_global_property_index;

} } // amalgam::chain

FC_REFLECT( amalgam::chain::dynamic_global_property_object,
             (id)
             (head_block_number)
             (head_block_id)
             (time)
             (current_witness)
             (virtual_supply)
             (current_supply)
             (confidential_supply)
             (current_abd_supply)
             (confidential_abd_supply)
             (total_vesting_fund_amalgam)
             (total_vesting_shares)
             (abd_interest_rate)
             (abd_print_rate)
             (maximum_block_size)
             (current_aslot)
             (recent_slots_filled)
             (participation_count)
             (last_irreversible_block_num)
          )
CHAINBASE_SET_INDEX_TYPE( amalgam::chain::dynamic_global_property_object, amalgam::chain::dynamic_global_property_index )
