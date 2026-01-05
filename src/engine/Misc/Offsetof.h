
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
			( char* )( &( TV::value.*MPtr ) ) -
			( char* )( &TV::value )
		];
	};
#pragma clang diagnostic pop
}

namespace sk
{
	// Made the offset_of function use this as a helper class to deduce away the explicit mention of the members class.
	template< class Ty, class Value >
	struct sMemberVariable
	{
		using class_type = Ty;
		using value_type = Value;
	            
		consteval sMemberVariable( Value Ty::* _ptr )
		: ptr( _ptr )
		{}
		Value Ty::* ptr;
	};
} // sk::

template< sk::sMemberVariable Member, class Class = decltype( Member )::class_type >
constexpr int offset_of() {
	if constexpr( Member.ptr == nullptr )
		return 0;
	else
		return sizeof( detail::offset_helper< Class, Member.ptr >::for_sizeof );
}