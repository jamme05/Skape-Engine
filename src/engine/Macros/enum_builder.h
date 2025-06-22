/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <cstdint>

#include "For_Each.h"

#include "Containers/Const/String.h"
#include <Containers/Const/Const_Map.h>

#define TYPE_ENUM_2( Name, Type ) internal_ ## Name : Type
#define TYPE_ENUM_1( Name ) internal_ ## Name : uint8_t
#define TYPE_ENUM( ... ) enum CONCAT( TYPE_ENUM_, VARGS( __VA_ARGS__ ) )( __VA_ARGS__ )
#define TYPE_ENUMCLASS( ... ) enum class CONCAT( TYPE_ENUM_, VARGS( __VA_ARGS__ ) )( __VA_ARGS__ )
#define TYPE_FLAGS( ... ) enum class CONCAT( TYPE_ENUM_, VARGS( __VA_ARGS__ ) )( __VA_ARGS__ )

// enum_t is safe in all the current scenarios.
#define SAFE_TYPE_ENUM_2( Name, Type ) enum_t
#define SAFE_TYPE_ENUM_1( Name ) uint8_t
#define SAFE_TYPE_ENUM( ... ) CONCAT( SAFE_TYPE_ENUM_, VARGS( __VA_ARGS__ ) )( __VA_ARGS__ )

#define SAFE_TYPE_ENUMCLASS( Name, ... ) enum_t

#define SAFE_TYPE_FLAGS_2( Name, Type ) Type
#define SAFE_TYPE_FLAGS_1( Name ) uint8_t
#define SAFE_TYPE_FLAGS( ... ) CONCAT( SAFE_TYPE_FLAGS_, VARGS( __VA_ARGS__ ) )( __VA_ARGS__ )

#define SIZE_ENUM_2( Name, Type ) Type
#define SIZE_ENUM_1( Name ) uint8_t
#define SIZE_ENUM( ... ) CONCAT( SAFE_TYPE_ENUM_, VARGS( __VA_ARGS__ ) )( __VA_ARGS__ )
#define SIZE_ENUMCLASS( ... ) SIZE_ENUM( __VA_ARGS__ )
#define SIZE_FLAGS( ... ) SIZE_ENUM( __VA_ARGS__ )


#define ENUM( ... ) _ENUM ( __VA_ARGS__ )
#define ENUMCLASS( ... ) _ENUMCLASS ( __VA_ARGS__ )
#define FLAGS( ... ) _FLAGS ( __VA_ARGS__ )

#define RAW_ENUM( Name, ... ) Name
#define RAW_ENUMCLASS( Name, ... ) Name
#define RAW_FLAGS( Name, ... ) Name

#define NAME_ENUM( Name, ... ) internal_ ## Name
#define NAME_ENUMCLASS( Name, ... ) internal_ ## Name
#define NAME_FLAGS( Name, ... ) internal_ ## Name

#define BUILD_MACRO_ENUM( Name, ... ) BUILD_ENUM_ENUM
#define BUILD_MACRO_ENUMCLASS( Name, ... ) BUILD_ENUM_ENUMCLASS
#define BUILD_MACRO_FLAGS( Name, ... ) BUILD_ENUM_FLAGS

// TODO: Rename E objects to ENUM_E.
#define E( ... ) _E( __VA_ARGS__ )

#define NAME_E( Name, ... ) Name
#define STR_NAME_E( Name, ... ) #Name

// TODO: Add max check of some sort
#define VALUE_E_3( Name, Value, ... ) Value
#define VALUE_E_2( Name, Value ) Value
#define VALUE_E_1( Name )
#define VALUE_E( ... ) CONCAT( VALUE_E_, VARGS( __VA_ARGS__ ) )( __VA_ARGS__ )

#define UNPACK_SAFE_E( Name, ... ) __VA_OPT__(,) __VA_ARGS__

#define CREATE_VALUE_METADATA_TYPE( Type ) \
struct sValueInfo \
{ Type Value; \
sk::str_hash name_hash; \
const char*  Name; \
const char*  DisplayName; \
};

namespace sk::Reflection::Enum
{
	consteval int get_safe_enum_value( const char*, const int _counter, ... ){ return _counter; };
	consteval int get_safe_enum_value( const int _value, const int, ... ){ return _value; };
	consteval int get_safe_enum_value( const int _value ){ return _value; };
} // qw::Reflection::Enum::

#define GET_SAFE_VALUE_2( Counter, ... ) = sk::Reflection::Enum::get_safe_enum_value( __VA_ARGS__ __VA_OPT__(,) Counter )
#define GET_SAFE_VALUE_1( Counter, Value )
#define GET_SAFE_VALUE( ... ) CONCAT( GET_SAFE_VALUE_, VARGS( __VA_ARGS__ ) )( __VA_ARGS__ )

#define VALID_E( ... ) ,1

#define IS_VALID_1( ... ) __VA_ARGS__ // Forward
#define IS_VALID_0( ... ) _E( __VA_ARGS__ ) // Create _E Object

#define IS_E_TYPE( ... ) SECOND( __VA_ARGS__, 0 )
#define UNWRAP_E_VALUE( Value ) CONCAT( IS_VALID_, IS_E_TYPE( CONCAT( VALID, Value ) ) ) ( Value )

#define MAKE_ENUM_VALUE_1( Value, Counter ) NAME ## Value GET_SAFE_VALUE( Counter, VALUE ## Value ),
#define MAKE_ENUM_VALUE( Value, Counter ) MAKE_ENUM_VALUE_1( Value, Counter )
#define UNPACK_ENUM_VALUE( Value, Counter ) MAKE_ENUM_VALUE( UNWRAP_E_VALUE( Value ), Counter )

#define BUILD_ENUM_ENUM( Type, ... ) \
	TYPE ## Type { COUNTER_FOR_EACH( UNPACK_ENUM_VALUE, 0, __VA_ARGS__ ) }


#define BUILD_ENUM_ENUMCLASS( Type, ... ) \
	TYPE ## Type { COUNTER_FOR_EACH( UNPACK_ENUM_VALUE, 0, __VA_ARGS__ ) }

#define BUILD_ENUM_FLAGS( Type, ... ) \
	TYPE ## Type { COUNTER_FOR_EACH( UNPACK_ENUM_VALUE, 0, __VA_ARGS__ ) }

#define BUILD_ENUM_BODY( Type, ... ) \
	BUILD_MACRO ## Type ( Type, __VA_ARGS__ )

#define INFO_GETTER_IF_1( Type, Value, Counter ) if( RAW ## Type :: NAME ## Value == _v ) return Values[ Counter ];
#define INFO_GETTER_IF_0( Type, Value, Counter ) INFO_GETTER_IF_1( Type, Value, Counter )
#define INFO_GETTER_IF( Type, Value, Counter ) INFO_GETTER_IF_0( Type, UNWRAP_E_VALUE( Value ), Counter )

#define BUILD_INFO_GETTER( Type, ... ) \
	static constexpr const sValue& get_enum_info( const RAW ## Type _v ){ \
	COUNTER_ARG_FOR_EACH( INFO_GETTER_IF, Type, 0, __VA_ARGS__ ) \
	return kInvalid; \
	}

#define ENUM_VALUE_METADATA_1( Type, Name, Value, ... ) std::pair{ static_cast< value_t >( Name :: NAME ## Value ), sk::Reflection::Enum::enum_value_creator< Name, sValueInfo >( Name :: NAME ## Value, STR_NAME ## Value UNPACK_SAFE ## Value ) } __VA_OPT__(,)
#define ENUM_VALUE_METADATA_0( Type, Value, ... ) ENUM_VALUE_METADATA_1( Type, NAME ## Type, Value, __VA_ARGS__ )
#define ENUM_VALUE_METADATA( Type, Value, ... ) ENUM_VALUE_METADATA_0( Type, UNWRAP_E_VALUE( Value ), __VA_ARGS__ )

#define FAKE_ENUM_MEMBER_1( Type, Value ) static constexpr value_t NAME ## Value = static_cast< value_t >( enum_t :: NAME ## Value );
#define FAKE_ENUM_MEMBER_0( Type, Value ) FAKE_ENUM_MEMBER_1( Type, Value )
#define FAKE_ENUM_MEMBER( Type, Value ) FAKE_ENUM_MEMBER_0( Type, UNWRAP_E_VALUE( Value ) )

// TODO Add extra operators for flags
#define BUILD_ENUM_OPERATORS( Type ) \
	constexpr Type ( void ) : m_value( static_cast< value_t >( 0 ) ){} \
	constexpr ~ Type ( void ) = default; \
	constexpr Type ( const value_t _value ) : m_value( _value ){}   \
	constexpr Type ( const Type &  _other ) = default;              \
	constexpr Type (       Type && _other ) = default;           \
	constexpr operator value_t() const { return m_value; }          \
	constexpr Type& operator=( const value_t _value ){ m_value = _value; return *this; } \
	constexpr Type& operator=( const Type &  _other ) = default;    \
	constexpr Type& operator=(       Type && _other ) = default; \
	constexpr bool operator==( const Type & _right ) const { return m_value == _right.m_value; } \
	constexpr bool operator!=( const Type & _right ) const { return !( *this == _right ); } \
	constexpr bool operator==( const value_t & _right ) const { return m_value == _right; } \
	constexpr bool operator!=( const value_t & _right ) const { return !( *this == _right ); } \
	auto getInfo( void ) const { const auto index = kValues.find( m_value ); return ( index != -1 ? &kValues.get( index ).second : nullptr ); } \
	auto getName( void ) const { if( const auto info = getInfo() ) return info->name; return "Invalid ( Unable to find )"; } \
	auto getDisplayName( void ) const { if( const auto info = getInfo() ) return info->display_name; return "Invalid ( Unable to find )"; } \
	private: \
	value_t m_value;


#define BUILD_ENUM_METADATA( Type, ... )  \
	struct RAW ## Type    \
	{ typedef NAME ## Type enum_t; typedef SAFE_TYPE ## Type value_t; \
	CREATE_VALUE_METADATA_TYPE( enum_t ) \
	FOR_EACH_W_ARG( FAKE_ENUM_MEMBER, Type, __VA_ARGS__ ) \
	static constexpr sk::const_map kValues{ sk::array{ FOR_EACH_FORWARD_W_ARG( ENUM_VALUE_METADATA, Type, __VA_ARGS__ ) } }; \
	BUILD_ENUM_OPERATORS( RAW ## Type ) \
	/* BUILD_INFO_GETTER( Type, __VA_ARGS__ ) */ \
	} // Removed semicolon to give the c++ feel ehe

// Macro used for getting the metadata type. Recommended to use in case of future changes.
#define META( Type ) Type
#define ENUM_INFO( Type, Value ) META( Type ) :: get_enum_info( Value )

// TODO: Documentation
/**
 *
 * Used mainly for internal parts.
 *
 * @param Type ENUM/ENUMCLASS ( Name, [ Size ] )
 * @param ...  Value/E( Name, [ value ], [ Display Name ] )
 *
**/
#define MAKE_UNREFLECTED_ENUM( Type, ... ) \
	BUILD_ENUM_BODY( Type, __VA_ARGS__ );  \
	BUILD_ENUM_METADATA( Type, __VA_ARGS__ )

namespace sk::Reflection::Enum
{
	struct sRawValue
	{
		sk::str_hash name_hash;
		const char*  name;
		const char*  display_name;
	};
	template< class ETy >
	struct sValue : sRawValue
	{
		ETy value;
	};

	
	template< class ETy >
	consteval sValue< ETy > enum_value_builder( ETy _value, const char* _name, const char* _display_name = nullptr )
	{
		if( _display_name == nullptr )
		{
			_display_name = _name;
			if( *_display_name == '_' )
				__nop();
			else if( *_display_name == 'k' )
			{
				++_display_name;
				if( *_display_name == '_' )
					++_display_name;
			}
		}
		sValue< ETy > result = {};
		result.value = _value;
		result.name_hash = sk::str_hash( _name );
		result.name = _name;
		result.display_name = _display_name;

		return result;
	}
	template< class ETy, class Ty >
	consteval sValue< ETy > enum_value_creator( ETy _value, const char* _name, const char* _display_name, ... )
	{
		return enum_value_builder< Ty >( _value, _name, _display_name );
	} // enum_value_creator

	template< class ETy, class Ty >
	consteval sValue< ETy > enum_value_creator( ETy _value, const char* _name, const size_t, const char* _display_name, ... )
	{
		return enum_value_builder< Ty >( _value, _name, _display_name ); // Forward in case of future changes.
	} // enum_value_creator

	template< class ETy, class Ty >
	consteval sValue< ETy > enum_value_creator( ETy _value, const char* _name, const int, const char* _display_name, ... )
	{
		return enum_value_builder< Ty >( _value, _name, _display_name ); // Forward in case of future changes.
	} // enum_value_creator

	template< class ETy, class Ty >
	consteval sValue< ETy > enum_value_creator( ETy _value, const char* _name )
	{
		return enum_value_builder< Ty >( _value, _name );
	} // enum_value_creator

	template< class ETy, class Ty >
	consteval sValue< ETy > enum_value_creator( ETy _value, const char* _name, const size_t )
	{
		return enum_value_builder< Ty >( _value, _name ); // Forward in case of future changes.
	} // enum_value_creator

	template< class ETy, class Ty >
	consteval sValue< ETy > enum_value_creator( ETy _value, const char* _name, const int )
	{
		return enum_value_builder< Ty >( _value, _name ); // Forward in case of future changes.
	} // enum_value_creator
} // qw::Reflection::Enum::

MAKE_UNREFLECTED_ENUM( ENUM( eExample1 ),
	E( kValueEx, 0x00 ),
	E( kDisplayNameEx, "Display Name" ),
	E( kBothEx, 3, "Wha" ),
	E( kNormal )
);

MAKE_UNREFLECTED_ENUM( ENUMCLASS( eExample2 ),
	E( kFirst ),
	E( kSecond ),
	E( kThird )
);

MAKE_UNREFLECTED_ENUM( FLAGS( eExample3, uint16_t ),
	kFirst,
	kSecond,
	kThird
);
