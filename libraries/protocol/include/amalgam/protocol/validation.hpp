
#pragma once

#include <amalgam/protocol/base.hpp>
#include <amalgam/protocol/block_header.hpp>
#include <amalgam/protocol/asset.hpp>

#include <fc/utf8.hpp>

namespace amalgam { namespace protocol {

inline bool is_asset_type( asset asset, asset_symbol_type symbol )
{
   return asset.symbol == symbol;
}

inline void validate_account_name( const string& name )
{
   FC_ASSERT( is_valid_account_name( name ), "Account name ${n} is invalid", ("n", name) );
}

} }
