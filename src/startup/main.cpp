/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "App.h"

#include <Memory/Tracker/Tracker.h>

namespace sk
{
	static_assert( registry::type_registry< 0 >::valid, "No types registered." );
	constexpr static auto types = registry::type_registry< sk::registry::counter::next() - 1 >::registered;
	const unordered_map< type_hash, const sType_Info* > type_map = { types.begin(), types.end() };
} // sk::

struct example_struct
{
	uint32_t m1;
	uint32_t m2;
};

namespace detail
{
	template< class Ty > struct declval_helper { static Ty value; };
	template< class Ty, auto Ty::*Member >
	struct offset_helper
	{
		using TV = declval_helper< Ty >;
		char for_sizeof[ ( char* )&( TV::value.*Member ) - ( char* )&TV::value ];
	};
	
}

template< class Ty, auto Ty::*Member >
[[ nodiscard ]] consteval size_t sk_offsetOf( void )
{
	return sizeof( detail::offset_helper< Ty, Member >::for_sizeof );
}

int main( int, char** )
{
	constexpr static auto off  = offsetof( example_struct, m2 );
	constexpr static auto off2 = offsetof( example_struct, m2 );
	constexpr int32_t  v1 = 10;
	constexpr uint32_t t1 = std::bit_cast< uint32_t >( v1 );
	constexpr auto     v2 = sk_offsetOf< example_struct, &example_struct::m1 >();
	constexpr auto     v3 = sizeof( detail::offset_helper< example_struct, &example_struct::m2 >::for_sizeof );

	sk::Memory::Tracker::init();
	auto& app =    cApp::init();

	app.create();

	while( app.running() )
		app.run();

	cApp::shutdown();
	sk::Memory::Tracker::shutdown();

	return 0;
}
