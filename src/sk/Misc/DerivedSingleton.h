/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Debugging/Debugging.h>
#include <sk/Memory/Memory.h>

namespace sk
{
    template< class Base, class Ty >
    class cDerivedSingleton : public Base
    {
    public:
        // Causes error in case the singleton is already initialized good in case the timing of initialization is important.
        template< typename... Args >
        static Ty&   init    ( Args&&... _args )
        {
            // TODO: Use reflection to access the name of the instance.
            SK_BREAK_IF( sk::Severity::kGeneral, Base::m_instance_ != nullptr,
                TEXT( "Error: Singleton Instance already exists." ) )

            SK_ERR_IF( std::atexit( &Base::quit ),
                TEXT( "Unable to safely register singleton." ) )

            Ty* instance;
            Base::m_instance_ = instance = Memory::Internal::alloc< Ty >( std::forward< Args >( _args )... );
            return *instance;
        }

        // Safely request the singleton to be initialized. This can allow you to call it multiple times without triggering an error.
        // Useful if multiple systems want to ensure that an optional manager is active.
        template< typename... Args >
        static Ty& try_init( Args&&... _args )
        {
            if( Base::m_instance_ )
                return static_cast< Ty& >( *Base::m_instance_ );

            return init( std::forward< Args >( _args )... );
        }
        
        static Ty& get   (){ return static_cast< Ty& >( *Base::m_instance_ ); } // TODO: Add error/assert checks
        static Ty* getPtr(){ return static_cast< Ty* >( Base::m_instance_ ); }
    };

    template< class Ty >
    class cBaseSingleton
    {
        template< class Base, class Ty2 >
        friend class cDerivedSingleton;
        
        static Ty* m_instance_;
        static void quit()
        {
            if( m_instance_ )
                shutdown();
        }
        
    protected:
        cBaseSingleton(){ m_instance_ = static_cast< Ty* >( this ); }
    public:
        virtual ~cBaseSingleton() = default;

        template< class De >
        using Derived = cDerivedSingleton< Ty, De >;
        
        static void shutdown()
        {
            SK_BREAK_IF( sk::Severity::kGeneral, m_instance_ == nullptr,
                TEXT( "Warning: Singleton Instance does not exists or is already destroyed." ) )

            Memory::Internal::free( m_instance_ );
            m_instance_ = nullptr;
        }

        static Ty&   get   (){ return *m_instance_; } // TODO: Add error/assert checks
        static auto  getPtr(){ return  m_instance_; }
    };
    template< class Ty >
    inline Ty* cBaseSingleton< Ty >::m_instance_ = nullptr;
} // sk::
