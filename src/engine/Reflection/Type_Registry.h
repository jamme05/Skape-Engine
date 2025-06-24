/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#ifndef SK_ALLOW_DIRECT_REGISTRY_ACCESS
#error You aren't allowed to access the type registry directly. Include <Reflection/Types.h> instead.
#endif // !SK_ALLOW_DIRECT_REGISTRY_ACCESS

#include "Containers/Const/Array.h"
#include <Containers/Map.h>

#include "Misc/Hashing.h"
#include "Macros/Enum_Builder.h"
#include <Macros/Manipulation.h>

#include "Type_Hash.h"
#include "Input/Input.h"
#include "Misc/Counter.h"

namespace sk
{
    constexpr static type_hash kInvalid_Id = static_cast< uint64_t >( 0 );

    struct sType_Info;
    extern const unordered_map< type_hash, const sType_Info* > type_map;
    typedef std::pair< type_hash, const sType_Info* > type_pair_t;

    // Forward declare to allow for simpler casting.
    struct sStruct_Type_Info;

    struct sType_Info
    {
        MAKE_UNREFLECTED_ENUM( ENUMCLASS( eType ),
            E( kStandard ),
            E( kEnum ),
            E( kArray ),
            E( kStruct ),
            E( kClass )
        );

        eType       type;
        type_hash   hash;
        size_t      size;
        const char* name;
        const char* raw_name;

        constexpr type_pair_t pair( void ) const
        {
            return { hash, static_cast< const sType_Info* >( this ) };
        } // pair
        /**
         * Function for converting to an instance of struct info.
         * @return Itself as a sStruct_Type_Info if valid, or nullptr if invalid.
         */
        constexpr const sStruct_Type_Info* as_struct_info( void ) const;
    };

    // TODO: Move individual types to their own spaces.
    namespace runtime_struct
    {
        struct sMemberInfo
        {
            type_hash   type;
            str_hash    name_hash; // Original name shouldn't be required to be accessed?
            const char* display_name;
            size_t      size;
            size_t      offset;

            [[nodiscard]] const sType_Info* get_type( void ) const;
        };

        inline const sType_Info* sMemberInfo::get_type( void ) const
        {
            if( const auto it = type_map.find( type ); it != type_map.end() )
                return it->second;
            return nullptr;
        } // get_type
    } // runtime_struct::

    struct sStruct_Type_Info : sType_Info
    {
        map_ref< str_hash, runtime_struct::sMemberInfo, std::less< str_hash > > members;
    };

    struct sEnum_Type_Info : sType_Info
    {
        map_ref< str_hash, runtime_struct::sMemberInfo, std::less< str_hash > > members;
    };

    // Functions for converting sType_Info
    constexpr const sStruct_Type_Info* sType_Info::as_struct_info() const
    {
        // In case this fails someone created it with the wrong type.
        if( type == eType::kStruct )
            return static_cast< const sStruct_Type_Info* >( this );
        return nullptr;
    } // as_struct_info

    struct sModifier
    {
        enum class eType : uint8_t
        {
            kArray,
            kPointer,
            kReference,
        };
        constexpr sModifier( const eType   _type  ) : array_size( 0 ),     type( _type ){}
        constexpr sModifier( const uint16_t _size ) : array_size( _size ), type( eType::kArray ){}
        uint16_t array_size;
        eType    type;
    };

    // Maybe not required for the future?
    template< class Ty >
    struct type_registry_internal
    {
    };

    struct template_type_info
    {
        constexpr static auto kMods  = array< sModifier, 0 >{};
        constexpr static bool kValid = true;
    };

    template< class Ty >
    struct get_type_info : template_type_info
    {
        // static_assert( false, "Invalid Type" );
        constexpr static sType_Info kInfo = {
            .type = sType_Info::eType::kStandard,
            .hash = kInvalid_Id,
            .size = 0,
            .name = "Invalid",
            .raw_name = "invalid"
        };
    };
    
    // TODO: Use modifiers and recursive instead to allow for a better experience.
    template< class Ty >
    requires get_type_info< Ty >::kValid
    struct get_type_info< Ty* > : get_type_info< Ty >
    {
        typedef get_type_info< Ty > prev_t;
        constexpr static sType_Info& kInfo = prev_t::kInfo;
        constexpr static auto        kMods = array{ sModifier{ sModifier::eType::kPointer } } + prev_t::kMods;
    };

    template< class Ty >
    requires get_type_info< Ty >::kValid
    struct get_type_info< Ty& > : get_type_info< Ty >
    {
        typedef get_type_info< Ty > prev_t;
        constexpr static sType_Info& kInfo = prev_t::kInfo;
        constexpr static auto        kMods = array{ sModifier{ sModifier::eType::kReference  } } + prev_t::kMods;
    };

    template< class Ty, size_t Size >
    requires get_type_info< Ty >::kValid
    struct get_type_info< Ty[ Size ] > : get_type_info< Ty >
    {
        typedef get_type_info< Ty > prev_t;
        constexpr static sType_Info& kInfo = prev_t::kInfo;
        constexpr static auto        kMods = array{ sModifier{ Size } } + prev_t::kMods;
    };

    template< class Ty >
    struct get_type_info< Ty&& > : get_type_info< Ty >
    {
        // TODO: Figure out ways to actually support it?
        static_assert( false, "R-Reference isn't supported." );
    };

    template< class Ty >
    requires get_type_info< Ty >::kValid
    constexpr inline type_hash get_type_hash_v = get_type_info< Ty >::kInfo.hash;

    class sType
    {
    public:
        template< class Ty >
        requires get_type_info< Ty >::kValid
        sType( void );

        [[ nodiscard ]] constexpr auto get_info     ( void ) const { return m_type_info_; }
        [[ nodiscard ]] constexpr auto get_modifiers( void ) const { return m_modifiers_; }

    private:
        const sType_Info*        m_type_info_;
        std::vector< sModifier > m_modifiers_ = {};
    };

    template< class Ty >
    requires get_type_info< Ty >::kValid
    sType::sType( void )
    : m_type_info_( &get_type_info< Ty >::kInfo )
    {
    }

    template< class Ty >
    struct get_type
    {
        using type_info = get_type_info< Ty >;
        
    };

    template< class Ty >
    constexpr static auto& kTypeId = get_type_info< Ty >::kInfo;

    template< class Ty >
    constexpr uint64_t get_type_value( void ) // Make some version already applying the prime_64_const
    {
            typedef get_type_info< Ty > type_container_t;
            static_assert( type_container_t::kValid, "The type specified isn't registered as a valid Type." );

            return type_container_t::kInfo.hash.getValue();
    } // get_type_hash

    enum eArgumentError : uint8_t
    {
        kValid = 0, // The argument types are valid.
        kVoidNotAllowed = 1, // Void isn't allowed in this scenario.
        kVoidNotOnly   = 2,  // Void isn't the only type in this scenario.
    };
    template< size_t Size >
    constexpr uint8_t validate_args( const array< type_hash, Size >& _array, const bool _allow_void = true );

    template< class Ty >
    struct is_valid_type
    {
        constexpr static bool value = get_type_info< Ty >::kValid;
    };
    template< class Ty >
    constexpr inline bool kValidType = is_valid_type< Ty >::value;

    template< class... Types >
    constexpr inline bool kValidTypes = std::conjunction_v< is_valid_type< Types >... >;
    template<>
    constexpr inline bool kValidTypes<> = false;

    consteval type_hash calculate_types_hash( const array_ref< type_hash >& _hashes )
    {
        if( _hashes.size() == 0 )
            return get_type_hash_v< void >;

        uint64_t value = Hashing::prime_64_const;
        for( auto& hash : _hashes )
            value = ( value ^ hash.getValue() ) * Hashing::prime_64_const;
        return type_hash{ value };
    } // calculate_types_hash
    
    template< class... Types >
    consteval type_hash calculate_types_hash( void )
    {
        static constexpr auto hashes = array{ get_type_hash_v< Types >... };
        if constexpr ( sizeof...( Types ) > 0 )
            return calculate_types_hash( hashes );
        else
            return get_type_hash_v< void >;
    }

    template< bool AllowVoid, class... Types >
    requires kValidTypes< Types... >
    struct types_hash
    {
        constexpr static size_t    kCount = sizeof...( Types );
        constexpr static array     kTypes = array< type_hash, kCount >{ get_type_info< Types >::kInfo... };
        constexpr static type_hash kHash  = calculate_types_hash< Types... >();
        constexpr static auto      kError = validate_args( kTypes, AllowVoid );
        static_assert( !( kCount == 0 && !AllowVoid ), "This reflection system doesn't support empty structs. Make native C++ structs instead." );
        static_assert( !( kError & kVoidNotAllowed ),  "Type Void found but was not allowed in this scenario." );
        static_assert( !( kError & kVoidNotOnly ),     "Type Void isn't the only type in this scenario." );
    };
    template< class... Types >
    using args_hash = types_hash< true, Types... >;
    template< class... Types >
    using struct_hash = types_hash< false, Types... >;
} // sk::

// Pre-declare void type.
template<> struct sk::get_type_info< void >
{
    constexpr static sType_Info kInfo   = { .type = sType_Info::eType::kStandard, .hash = { "Void" }, .size = 0, .name = "Void", .raw_name = "void" };
    constexpr static bool       kValid = true;
}; // Void my beloved

template< size_t Size >
constexpr uint8_t sk::validate_args( const array< type_hash, Size >& _array, const bool _allow_void )
{
    // TODO: Decide if void* should be allowed in the reflection system.
    if constexpr( Size == 1 )
    {
        // If not void it's always going to be valid.
        if( _array[ 0 ] != get_type_hash_v< void > )
            return kValid;
        // Otherwise the bool decides the fate.
        return _allow_void ? kValid : kVoidNotAllowed; // -1 = Type void found but was not allowed.
    }
    else
    {
        // Validate so there's only a single void type.
        for( size_t i = 0; i < Size; ++i )
        {
            // in case it has a void type, it will always be an error.
            if( _array[ i ] == get_type_hash_v< void > )
            {
                if( !_allow_void )
                    return kVoidNotAllowed | kVoidNotOnly; // -1 = Type void found but was not allowed
                return kVoidNotOnly; // -2 = Type void is only allowed if it's the only type.
            }
        }
            
    }
    return kValid; // Nothing wrong.
}

namespace sk::registry
{
    struct registry_tag{};
    typedef const_counter< registry_tag > counter;
    template< int64_t Iteration >
    struct type_registry
    {
        constexpr static array< type_pair_t, 0 > registered = {};
        constexpr static bool                    valid      = false;
    };
} // sk::

#define REGISTER_TYPE_INTERNAL_0( Type, IdLocation ) \
constexpr static auto IdLocation = sk::registry::counter::next(); \
template<> struct sk::registry::type_registry< IdLocation >{ \
    typedef type_registry< IdLocation - 1 > previous_t; \
    constexpr static auto   Iteration  = IdLocation; \
    constexpr static array  registered = previous_t::registered + array{ get_type_info< Type >::kInfo.pair() }; \
    constexpr static bool valid = true; \
};
#define REGISTER_TYPE_INTERNAL( Type ) REGISTER_TYPE_INTERNAL_0( Type, CONCAT( type_registry_, __COUNTER__ ) )

// Finally register void as everything exists.
REGISTER_TYPE_INTERNAL( void )

#define MAKE_TYPE_INFO_DIRECT( Type, Name, HashMacro, ... ) \
template<> struct sk::get_type_info< Type >{ \
constexpr static sType_Info kInfo   = { .type = sType_Info::eType::kStandard, .hash HashMacro( __VA_ARGS__ ) , .size = sizeof( Type ), .name = Name, .raw_name = #Type }; \
constexpr static bool      kValid = true; \
};

#define MAKE_DEFAULT_HASH( HashName, ... ) { HashName }
#define REGISTER_DEFAULT_TYPE( Type, InternalName, Name ) \
    MAKE_TYPE_INFO_DIRECT( Type, Name, MAKE_DEFAULT_HASH, Name ) \
    REGISTER_TYPE_INTERNAL( Type )

#define REGISTER_T_TYPE( Type, Name ) REGISTER_DEFAULT_TYPE( Type ## _t, #Type, Name )
#define REGISTER_TYPE( Type, Name ) REGISTER_DEFAULT_TYPE( Type, #Type, Name )
