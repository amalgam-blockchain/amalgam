#pragma once
#include <cstdint>
#include <fc/uint128.hpp>

namespace fc {
class variant;
} // fc

namespace amalgam { namespace protocol {
template< typename Storage = fc::uint128 >
class fixed_string_impl;

} } // amalgam::protocol

namespace fc { namespace raw {

template< typename Stream, typename Storage >
inline void pack( Stream& s, const amalgam::protocol::fixed_string_impl< Storage >& u );
template< typename Stream, typename Storage >
inline void unpack( Stream& s, amalgam::protocol::fixed_string_impl< Storage >& u, uint32_t depth = 0 );

} // raw

template< typename Storage >
inline void to_variant( const amalgam::protocol::fixed_string_impl< Storage >& s, fc::variant& v );
template< typename Storage >
inline void from_variant( const variant& v, amalgam::protocol::fixed_string_impl< Storage >& s );

} // fc
