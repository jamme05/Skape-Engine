/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <type_traits>
#include <Macros/manipulation.h>

#include "Type_Hash.h"

namespace sk
{
    class cRuntimeInterface
    {
    public:
        constexpr type_hash getHash( void ) const { return m_hash_; }

    private:
        type_hash m_hash_;
    };

    // TODO: Add actual functionality.
    class cInterface
    {
    };

    namespace Internal
    {
        template< bool IsValid, class Ty >
        struct extract_valid_interface
        {
            static constexpr bool value = false;
        };

        template< class Ty >
        struct my_struct< true, Ty >
        {
            static constexpr bool value = std::is_base_of_v< cInterface, Ty >;
        };

    } // Internal ::

    // Use std::is_base_of or std::is_base_of_v to figure out if a class has a certain interface.
    template< class Ty >
    struct is_interface
    {
        static constexpr bool value = Internal::extract_valid_interface< std::is_class_v< Ty >, Ty >::value;
    };
} // sk::

#define INTERFACE_TYPE( Name ) CONCAT( i,Name )

// Interface inheritance
#define INTERFACE_I _INTERFACE_I
#define EXTRACT_INTERFACE_I( ... ) __VA_ARGS__
#define IS_PARENT_INTERFACE_I( ... ) 1
// Interface function
#define INTERFACE_F _INTERFACE_F
#define IS_PARENT_INTERFACE_F( ... ) 0

#define GET_PARENT_INTERFACE_1( Parent ) CONCAT( EXTRACT, Parent )
#define GET_PARENT_INTERFACE_0( Parent ) sk::cInterface
#define IF_INTERFACE_INHERITANCE( Parent, IsValid ) CONCAT( GET_PARENT_INTERFACE_, IsValid )( Parent )
#define INTERFACE_INHERITANCE( ... ) GET_PARENT_INTERFACE_0

#define GET_PARENT_INTERFACE( PossibleParent ) CONCAT( INTERFACE_INHERITANCE )( PossibleParent, CONCAT( IS_PARENT, PossibleParent ) )

#define SK_INTERFACE_BODY( Interface, ... ) \
    class Interface GET_PARENT_INTERFACE( FIRST( __VA_ARGS__ ) ) {
    
// TODO: Add more functionality and reflection.
/**
 * @param Name
 * @param [Parent] Optional Interface parent.
 */
#define SK_INTERFACE( ... ) \
    SK_INTERFACE_BODY( INTERFACE_TYPE( FIRST( __VA_ARGS__ ) ) )

SK_INTERFACE( Test )
