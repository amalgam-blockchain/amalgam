#pragma once

#include <amalgam/protocol/asset.hpp>

namespace amalgam { namespace chain { namespace util {

using amalgam::protocol::asset;
using amalgam::protocol::price;

inline asset to_abd( const price& p, const asset& amalgam )
{
   FC_ASSERT( amalgam.symbol == AMALGAM_SYMBOL );
   if( p.is_null() )
      return asset( 0, ABD_SYMBOL );
   return amalgam * p;
}

inline asset to_amalgam( const price& p, const asset& abd )
{
   FC_ASSERT( abd.symbol == ABD_SYMBOL );
   if( p.is_null() )
      return asset( 0, AMALGAM_SYMBOL );
   return abd * p;
}

} } }
