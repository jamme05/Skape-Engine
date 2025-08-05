/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Debugging/Macros/Text.h>
#include <Debugging/Debugging.h>
#include <Memory/Memory.h>

namespace sk
{
	// TODO: Fix reflected singleton
	template< class Ty >
	class cSingleton
	{
	private:
		static Ty* m_instance_;
		static void quit( void )
		{
			if( m_instance_ )
				shutdown();
		}

	protected:
		         cSingleton( void ){ m_instance_ = static_cast< Ty* >( this ); }
	public:
		virtual ~cSingleton( void ) = default;

		// Causes error in case the singleton is already initialized good in case the timing is required.
		template< typename... Args >
		static Ty&   init    ( Args&&... _args )
		{
			// TODO: Use reflection to access the name of the instance.
			SK_BREAK_IF( sk::Severity::kGeneral, m_instance_ != nullptr,
				TEXT( "Error: Singleton Instance already exists." ) )

			SK_ERR_IF( std::atexit( &quit ),
				TEXT( "Unable to safely register singleton." ) )

			m_instance_ = Memory::Internal::alloc< Ty >( std::forward< Args >( _args )... );
			return *m_instance_;
		}

		// Safely request the singleton to be initialized multiple times.
		template< typename... Args >
		static Ty& try_init( Args&&... _args )
		{
			if( m_instance_ )
				return *m_instance_;

			return init( std::forward< Args >( _args )... );
		}

		static void shutdown( void )
		{
			SK_BREAK_IF( sk::Severity::kGeneral, m_instance_ == nullptr,
				TEXT( "Warning: Singleton Instance does not exists or is already destroyed." ) )

			Memory::Internal::free( m_instance_ );
			m_instance_ = nullptr;
		}

		static Ty&   get     ( void ){ return *m_instance_; } // TODO: Add error/assert checks
		static auto  getPtr  ( void ){ return  m_instance_; }

	};

	template< class Ty > Ty* cSingleton< Ty >::m_instance_ = nullptr;

} // sk::
