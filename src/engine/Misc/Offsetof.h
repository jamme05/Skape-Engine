
/*
 *
 * Source: https://thecppzoo.blogspot.com/2016/10/constexpr-offsetof-practical-way-to.html
 * 
 */

#pragma once

namespace detail {
	template<typename T>
	struct declval_helper { static T value; };

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wvla-cxx-extension"
#pragma clang diagnostic ignored "-Wgnu-folding-constant"
#pragma clang diagnostic ignored "-Wundefined-var-template"
	template< typename T, auto T::*MPtr >
	struct offset_helper {
		using TV = declval_helper< T >;
		char for_sizeof[
			reinterpret_cast< char* >( &( TV::value.*MPtr ) ) -
			reinterpret_cast< char* >( &TV::value )
		];
	};
#pragma clang diagnostic pop
}

template<typename T, auto T::*MPtr>
constexpr int offset_of() {
	if constexpr( MPtr == nullptr )
		return 0;
	else
		return sizeof(detail::offset_helper<T, MPtr>::for_sizeof);
}

struct my_struct
{
	int mem;
};
static constexpr auto t0 = offset_of< my_struct, &my_struct::mem >();
static constexpr auto t1 = offset_of< my_struct, static_cast< int my_struct::* >( nullptr ) >();
