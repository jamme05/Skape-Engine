/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <functional>
#include <random>

#include <Containers/Map.h>
#include <Containers/Vector.h>

#include <Misc/Hashing.h>
#include <Misc/Singleton.h>
#include <Misc/Print.h>
#include <Misc/Smart_Ptrs.h>

#include <Reflection/RuntimeClass.h>

namespace sk
{
	class iClass;
}

namespace sk
{
	class cScene;

	namespace Object
	{
		enum eEvents : uint16_t // TODO: Create an event listener class which works for both objects and components
		{
			kNone     = 0x00,

			// Global Events
			kUpdate   = 0x01,
			kRender   = 0x02,
			kUpdateRender = kUpdate | kRender,

			// Entity based Events
			kEnabled  = 0x04,
			kDisabled = 0x10,
			kEnabledDisabled = kEnabled | kDisabled,

			// Debug Events
			kDebugRender = 0x20,
			kAllDebug    = kDebugRender,

			kAll    = 0xffff,
		};
	} // Object

	template<>
	struct hash< Object::eEvents >
	{
		static constexpr size_t int_offset = sizeof( uint32_t ) * 8;

		//constexpr hash( const char* _to_hash )
		//: iHashed( Hashing::fnv1a_32( _to_hash ) )
		//{
		//} // hash

		constexpr hash( const str_hash& _other )
		: m_hash_( _other.value() )
		{
		} // hash
		static constexpr auto tmp = 1ull << 32;

		constexpr hash( const Object::eEvents _event )
		: m_hash_( ( 1ull << 32 ) | static_cast< uint32_t >( _event ) )
		{
		} // hash

		HASH_REQUIREMENTS( hash )
	};

	namespace Event
	{
		// Inspired by: https://stackoverflow.com/a/64782620
		template< class Re, class Ty, class... Args >
		consteval auto get_fn_type_helper( Re( Ty::* )( Args... ) const )
		{
			return std::tuple< Re( Args... ), Re, std::tuple< Args... > >{};
		}

		template< class Re, class Ty, class... Args >
		consteval auto get_fn_type_helper( Re( Ty::* )( Args... ) )
		{
			return std::tuple< Re( Args... ), Re, std::tuple< Args... > >{};
		}
		
		template< auto Fn >
		using function_type_t = std::tuple_element_t< 0, decltype( get_fn_type_helper( Fn ) ) >;
		
		template< auto Fn >
		using function_ret_t = std::tuple_element_t< 1, decltype( get_fn_type_helper( Fn ) ) >;
		
		template< size_t I, auto Fn >
		using function_arg_t = std::tuple_element_t< I, std::tuple_element_t< 2, decltype( get_fn_type_helper( Fn ) ) > >;

		template< class FunTy, class OtFunTy >
		concept convertible = requires( std::function< FunTy >& _l, std::function< OtFunTy >& _r )
		{
			_l = _r;
		};

		// Use event_t to create an event type, as it'll need the helper to function.
		// TODO: Add some way to deduce the helper from a type.
		template< class FuncTy, class Helper >
		struct sEvent
		{
			using raw_t  = FuncTy;
			using func_t = std::function< FuncTy >;
			using ret_t  = func_t::result_type;
			
			std::function< FuncTy > function;
			void*                   raw_ptr;

			sEvent() = default;
			sEvent( const func_t& _function )
			: function( _function )
			, raw_ptr( nullptr )
			{}
			
			template< class Fn >
			requires std::is_same_v< decltype( std::apply( std::declval< Fn >(), std::declval< std::tuple_element_t< 2, Helper > >() ) ), ret_t >
			sEvent( Fn& _invocable )
			: function( _invocable )
			, raw_ptr( nullptr )
			{}
			
			template< class Fn >
			requires std::is_same_v< decltype( std::apply( std::declval< Fn >(), std::declval< std::tuple_element_t< 2, Helper > >() ) ), ret_t >
			sEvent( Fn&& _invocable )
			: function( std::forward< Fn >( _invocable ) )
			, raw_ptr( nullptr )
			{}

			template< class H >
			sEvent( const sEvent< FuncTy, H >& _other )
			: function( _other.function )
			, raw_ptr( _other.raw_ptr )
			{}

			template< class H >
			sEvent( sEvent< FuncTy, H >&& _other )
			: function( std::move( _other.function ) )
			, raw_ptr( std::move( _other.raw_ptr ) )
			{}
			
			template< class Ot, class H >
			requires ( convertible< FuncTy, Ot >
				&& std::is_same_v< std::tuple_element_t< 1, H >, std::tuple_element_t< 1, Helper > > )
			sEvent( const sEvent< Ot, H >& _other )
			: function( _other.function )
			, raw_ptr( _other.raw_ptr )
			{}
			
			template< class Ot, class H >
			requires ( convertible< FuncTy, Ot >
				&& std::is_same_v< std::tuple_element_t< 1, H >, std::tuple_element_t< 1, Helper > > )
			sEvent( const sEvent< Ot, H >&& _other )
			: function( _other.function )
			, raw_ptr( _other.raw_ptr )
			{}
		};

		template< class Re, class... Args >
		requires ( std::is_same_v< Re, void > || std::is_same_v< Re, bool > )
		sEvent( std::function< Re( Args... ) > ) -> sEvent< Re( Args... ), std::tuple< Re( Args... ) > >;
		template< class Fn >
		sEvent( Fn& ) -> sEvent< function_type_t< &Fn::operator() >, decltype( get_fn_type_helper( &Fn::operator() ) ) >;
		template< class Fn >
		sEvent( Fn&& ) -> sEvent< function_type_t< &Fn::operator() >, decltype( get_fn_type_helper( &Fn::operator() ) ) >;

		template< class Re, class... Args >
		using event_t = sEvent< Re( Args... ), std::tuple< Re( Args... ), Re, Args... > >;
	} // sk::Event::
	
	template< class Ty, class Re, class... Args >
	auto CreateEvent( Ty& _instance, Re( Ty::*_function )( Args... ) )
	{
		static_assert( std::is_same_v< Ty, void > || std::is_same_v< Ty, bool >,
			"Event return type HAS to be of type bool or void" );

		// Shared
		if constexpr( std::is_base_of_v< cShared_from_this< Ty >, Ty > )
		{
			Event::event_t< bool, Args... > event;
			event.function = [ weak = _instance->get_weak(), _function ]( Args... _args )
			{
				if( weak == nullptr )
					return false;

				if constexpr( std::is_same_v< Re, bool > )
				{
					return _function( weak.Get(), _args... );
				}
				else
				{
					_function( weak.Get(), _args... );
					return true;
				}
			};
			event.raw_ptr = _function;

			return event;
		}
		else // Not shared
		{
			Event::event_t< Re, Args... > event;
			event.function = std::bind_front( _function, _instance );
			event.raw_ptr  = _function;
				
			return event;
		}
	}
	
	template< class Ty, class Re, class... Args >
	auto CreateEvent( Ty& _instance, Re( Ty::* _function )( Args... ) const )
	{
		static_assert( std::is_same_v< Ty, void > || std::is_same_v< Ty, bool >,
			"Event return type HAS to be of type bool or void" );

		// Shared
		if constexpr( std::is_base_of_v< cShared_from_this< Ty >, Ty > )
		{
			Event::event_t< bool, Args... > event;
			event.function = [ weak = _instance->get_weak(), _function ]( Args... _args )
			{
				if( weak == nullptr )
					return false;

				if constexpr( std::is_same_v< Re, bool > )
				{
					return _function( weak.Get(), _args... );
				}
				else
				{
					_function( weak.Get(), _args... );
					return true;
				}
			};
			event.raw_ptr = _function;

			return event;
		}
		else // Not shared
		{
			Event::event_t< Re, Args... > event;
			event.function = std::bind_front( _function, _instance );
			event.raw_ptr  = _function;
				
			return event;
		}
	}
	
	template< class Ty, class Re, class... Args >
	auto CreateEvent( Ty* _instance, Re( Ty::*_function )( Args... ) )
	{
		return CreateEvent( *_instance, _function );
	}
	
	template< class Ty, class Re, class... Args >
	auto CreateEvent( Ty* _instance, Re( Ty::*_function )( Args... ) const )
	{
		return CreateEvent( *_instance, _function );
	}

	template< class Ty, class Re, class... Args >
	auto CreateEvent( Ty* _instance, Re( Ty::*_function )( Args... ) )
	{
		return CreateEvent( *_instance, _function );
	}

	template< class Ty, class Fn, class Helper, size_t... Indices >
	auto create_event_impl( Ty* _instance, Fn&& _fn, Helper&, std::index_sequence< Indices... > )
	{
		using ret_t  = std::tuple_element_t< 1, Helper >;
		using args_t = std::tuple_element_t< 2, Helper >;
		
		return Event::sEvent{
			[ weak = _instance->get_weak(), func = std::forward< Fn >( _fn ) ](
				std::tuple_element_t< Indices, args_t >... _args )
		{
			if( weak == nullptr )
				return false;
			
			if constexpr( std::is_same_v< ret_t, bool > )
			{
				return func( std::forward< std::tuple_element_t< Indices, args_t > >( _args )... );
			}
			else
			{
				func( std::forward< std::tuple_element_t< Indices, args_t > >( _args )... );
				return true;
			}
		} };
	}

	template< class Ty, class Fn >
	requires ( std::is_same_v< Event::function_ret_t< &Fn::operator() >, bool >
		    || std::is_same_v< Event::function_ret_t< &Fn::operator() >, void >
		    || std::is_base_of_v< cShared_from_this< Ty >, Ty > )
	auto CreateEvent( Ty* _instance, Fn&& _function )
	{
		using raw_t  = decltype( Event::get_fn_type_helper( &Fn::operator() ) );
		using args_t = std::tuple_element_t< 2, raw_t >;

		char illegal_move;

		return create_event_impl( _instance, std::forward< Fn >( _function ),
			reinterpret_cast< raw_t& >( illegal_move ), std::make_index_sequence< std::tuple_size_v< args_t > >{} );
	}

	namespace Event
	{
		template< class... Types >
		struct total_size
		{
			static constexpr size_t value = sizeof( std::tuple< Types... > );
		};

		template<>
		struct total_size< void >
		{
			static constexpr size_t value = 0;
		};

		template<>
		struct total_size<>
		{
			static constexpr size_t value = 0;
		};
		template< class... Types >
		constexpr size_t kTotal_Size = total_size< Types... >::value;

		constexpr size_t kInvalid_Id = ~0llu;

		class iEventDispatcher
		{
		public:
			         iEventDispatcher( void ){}
			virtual ~iEventDispatcher( void ) = default;
			
			iEventDispatcher( const iEventDispatcher& ){}
			iEventDispatcher( iEventDispatcher&& ) noexcept {}
			iEventDispatcher& operator=( const iEventDispatcher& ){ return *this; }
			iEventDispatcher& operator=( iEventDispatcher&& ) noexcept { return *this; }

			virtual void remove_listener( const size_t _id ) = 0;

			virtual size_t get_arg_size( void ) const = 0;
			
			static size_t get_function_hash( void* _ptr )
			{
				// Function hashes will always have a binary 1 in the start.
				return Hashing::fnv1a_64( reinterpret_cast< const char* >( &_ptr ), sizeof( void* ) ) | 1llu;
			}

		protected:
			std::atomic_bool m_is_pushing{ false };
		};

		// TODO: Clean up the number of possible ways to create listeners, as it can get quite confusing.
		template< class... Args >
		class cEventDispatcher : public iEventDispatcher
		{
		public:
			using listener_t      = std::function< void( Args... ) >;
			using weak_listener_t = std::function< bool( Args... ) >;
			using event_t         = Event::event_t< void, Args... >;
			using weak_event_t    = Event::event_t< bool, Args... >;

			struct sSelfWrapper
			{
				using function_t = std::function< bool( const cWeak_Ptr< void >&, Args... ) >;
				
				template< sk_class Ty >
				sSelfWrapper( void( Ty::*_listener )( Args... ) )
				{
					runtime_class = Ty::getStaticClass();
					
					wrapped = [ _listener ]( const cWeak_Ptr< void >& _ptr, Args... _args )
					{
						if( auto ptr = _ptr.get() )
						{
							std::invoke( _listener, ptr, _args... );
							return true;
						}
						return false;
					};
					raw_ptr = _listener;
				}
				
				template< sk_class Ty >
				sSelfWrapper( void( Ty::*_listener )( Args... ) const )
				{
					runtime_class = Ty::getStaticClass();
					
					wrapped = [ _listener ]( const cWeak_Ptr< iClass >& _ptr, Args... _args )
					{
						if( auto ptr = static_cast< Ty* >( _ptr.get() ) )
						{
							std::invoke( _listener, ptr, _args... );
							return true;
						}
						return false;
					};
					raw_ptr = _listener;
				}

				iRuntimeClass* runtime_class;
				void*          raw_ptr;
				function_t     wrapped;
			};

			cEventDispatcher() = default;
			explicit cEventDispatcher( const cWeak_Ptr< iClass >& _self )
			: m_self_( _self )
			{}
			~cEventDispatcher() override
			{
				m_listeners_     .clear();
				m_weak_listeners_.clear();
				m_to_remove_     .clear();
			} // ~cEventDispatcher
			
			cEventDispatcher( const cEventDispatcher& ) = default;
			cEventDispatcher( cEventDispatcher&& ) noexcept = default;
			
			cEventDispatcher& operator=( const cEventDispatcher& ) = default;
			cEventDispatcher& operator=( cEventDispatcher&& ) noexcept = default;

			auto& operator+=( const event_t _listener )
			{
				auto [ id, is_lambda ] = get_function_id( _listener );
				
				SK_WARN_IF( sk::Severity::kGeneral | 200,
					is_lambda, "Warning: You shouldn't add a lambda with the += operator." )

				m_listeners_.emplace( id, _listener );

				return *this;
			}

			auto& operator+=( const weak_event_t& _listener )
			{
				auto [ id, is_lambda ] = get_function_id( _listener );
				
				SK_WARN_IF( sk::Severity::kGeneral | 200,
					is_lambda, "Warning: You shouldn't add a lambda with the += operator." )

				m_weak_listeners_.emplace( id, _listener );

				return *this;
			}

			auto& operator+=( const listener_t& _listener )
			{
				auto [ id, is_lambda ] = get_function_id( _listener );
				
				SK_WARN_IF( sk::Severity::kGeneral | 200,
					is_lambda, "Warning: You shouldn't add a lambda with the += operator." )

				m_listeners_.emplace( id, _listener );

				return *this;
			}

			auto& operator+=( const weak_listener_t& _listener )
			{
				auto [ id, is_lambda ] = get_function_id( _listener );
				
				SK_WARN_IF( sk::Severity::kGeneral | 200,
					is_lambda, "Warning: You shouldn't add a lambda with the += operator." )

				m_weak_listeners_.emplace( id, _listener );

				return *this;
			}

			// Unsubscribes a member function to be called on the assigned self.
			// In case you want to call a function with another class instance, use sk::CreateEvent
			auto& operator+=( const sSelfWrapper& _wrapper )
			{
				SK_ERR_IF( m_self_ == nullptr,
					"Error: This dispatcher doesn't have a this reference specified. You can specify one during the construction." )

				SK_ERR_IFN( m_self_->getClass().isDerivedFrom( _wrapper.runtime_class ),
					"Error: You can't provide a member function from a class which isn't compatible with this class." )
				
				auto id = get_function_hash( _wrapper.raw_ptr );

				m_listeners_.emplace( id, std::bind_front( _wrapper.wrapped, m_self_ ) );

				return *this;
			}
			
			auto& operator-=( const event_t& _listener )
			{
				remove_listener( _listener );
				return *this;
			}

			auto& operator-=( const weak_event_t& _listener )
			{
				remove_listener( _listener );
				return *this;
			}

			// Unsubscribes a member function to be called on the assigned self.
			// In case you want to call a function with another class instance, use sk::CreateEvent
			auto& operator-=( const sSelfWrapper& _wrapper )
			{
				SK_ERR_IF( m_self_ == nullptr,
					"Error: This dispatcher doesn't have a this reference specified. You can specify one during the construction." )
				
				SK_ERR_IFN( m_self_->getClass().isDerivedFrom( _wrapper.runtime_class ),
					"Error: You can't provide a member function from a class which isn't compatible with this class." )
				
				auto id = get_function_hash( _wrapper.raw_ptr );

				if( const auto itr = m_listeners_.find( id ); itr != m_listeners_.end() )
				{
					m_listeners_.erase( itr );
				}

				return *this;
			}
			
			size_t get_arg_size( void ) const override { return kTotal_Size< Args... >; }

			size_t size() const { return m_listeners_.size() + m_weak_listeners_.size() - m_to_remove_.size(); }

			size_t add_listener( const event_t& _listener )
			{
				auto [ id, is_lambda ] = get_function_id( _listener );
				m_listeners_.insert( std::make_pair( id, _listener ) );
				return id;
			} // add_listener

			size_t add_listener( const weak_event_t& _listener )
			{
				auto [ id, is_lambda ] = get_function_id( _listener );
				m_weak_listeners_.insert( std::make_pair( id, _listener ) );
				return id;
			} // add_listener

			void remove_listener( const event_t& _listener )
			{
				auto [ id, is_lambda ] = get_function_id( _listener, true );

				SK_ERR_IF( is_lambda,
					"Error: Can't remove a lambda by using a function pointer." )

				if( const auto itr = m_listeners_.find( id ); itr != m_listeners_.end() )
				{
					m_listeners_.erase( itr );
				}
			}

			void remove_listener( const weak_event_t& _listener )
			{
				auto [ id, is_lambda ] = get_function_id( _listener, true );

				SK_ERR_IF( is_lambda,
					"Error: Can't remove a lambda by using a function pointer." )

				if( const auto itr = m_weak_listeners_.find( id ); itr != m_weak_listeners_.end() )
				{
					m_weak_listeners_.erase( itr );
				}
			}

			void remove_listener( const size_t _id ) override
			{
				if( m_is_pushing )
				{
					m_to_remove_.push_back( _id );
					return;
				}

				if( const auto itr = m_listeners_.find( _id ); itr != m_listeners_.end() )
			 	{
			 		m_listeners_.erase( itr );
				}
				else if( const auto itr_2 = m_weak_listeners_.find( _id ); itr_2 != m_weak_listeners_.end() )
				{
					m_weak_listeners_.erase( itr );
				}
			} // remove_listener

			void push_event( Args... _args )
			{
				m_is_pushing = true;
				if constexpr( kTotal_Size< Args... > == 0 )
				{
					for( auto& callback : m_listeners_ )
					{
						callback.second();
					}
					for( auto& weak_listener : m_weak_listeners_ )
					{
						if( !weak_listener.second() )
							m_to_remove_.push_back( weak_listener.first );
					}
				}
				else
				{
					std::tuple< Args... > tuple{ std::forward< Args >( _args )... };
					for( auto& callback : m_listeners_ )
					{
						std::apply( callback.second, tuple );
					}
					for( auto& weak_listener : m_weak_listeners_ )
					{
						if( !std::apply( weak_listener.second, tuple ) )
							m_to_remove_.push_back( weak_listener.first );
					}
				}
				m_is_pushing = false;

				clean_listeners();
			} // push_event
			
		private:
			// Cleans out all events that were supposed to get deleted during the last event push.
			void clean_listeners()
			{
				for( auto& id : m_to_remove_ )
				{
					if( const auto itr = m_listeners_.find( id ); itr != m_listeners_.end() )
					{
						m_listeners_.erase( itr );
					}
					else if( const auto itr_2 = m_weak_listeners_.find( id ); itr_2 != m_weak_listeners_.end() )
					{
						m_weak_listeners_.erase( itr_2 );
					}
				}
				m_to_remove_.clear();
			} // clean_listeners

			// First: the id, Second: if the function is a lambda
			std::pair< size_t, bool > get_function_id( const event_t& _listener, const bool _ignore_lambda = false )
			{
				if( _listener.raw_ptr )
					return { get_function_hash( _listener.raw_ptr ), false };

				auto ptr = _listener.template target< void(*)( Args... ) >();
				if( ptr != nullptr )
					return { get_function_hash( ptr ), false };

				if( !_ignore_lambda )
					return { get_lambda_hash(), true };

				return { kInvalid_Id, true };
			}
			
			std::pair< size_t, bool > get_function_id( const weak_event_t& _listener, const bool _ignore_lambda = false )
			{
				if( _listener.raw_ptr )
					return { get_function_hash( _listener.raw_ptr ), false };
				
				auto ptr = _listener.function.template target< bool(*)( Args... ) >();
				if( ptr != nullptr )
					return { get_function_hash( ptr ), false };

				if( !_ignore_lambda )
					return { get_lambda_hash(), true };

				return { kInvalid_Id, true };
			}

			size_t get_lambda_hash()
			{
				static std::random_device rd;
				static std::mt19937_64 gen{ rd() };

				// Lambda ids will never have a binary 1 in the start.
				size_t res = gen();
				res &= ~1llu;

				while( m_listeners_.contains( res ) || m_weak_listeners_.contains( res ) )
				{
					res = gen();
					res &= ~1llu;
				}
				
				return gen();
			}

			cWeak_Ptr< iClass >                      m_self_;
			vector< size_t >                         m_to_remove_;
			unordered_map< size_t, listener_t >      m_listeners_;      // Static Listeners
			unordered_map< size_t, weak_listener_t > m_weak_listeners_; // Class Listeners, returns false if no longer valid.
		};

		// Has a unique ptr to a dispatcher but provides proxy functions to it.
		// Used to reduce the size when using a dispatcher.
		template< class... Args >
		class cDispatcherProxy
		{
			using dispatcher_t = cEventDispatcher< Args... >;
		public:
			cDispatcherProxy()
			: m_dispatcher_( std::make_unique< dispatcher_t >() )
			{}
			cDispatcherProxy( const cWeak_Ptr< iClass >& _self )
			: m_dispatcher_( std::make_unique< dispatcher_t >( _self ) )
			{}

			auto& operator+=( const dispatcher_t::event_t _listener )
			{
				return get() += _listener;
			}

			auto& operator+=( const dispatcher_t::weak_event_t& _listener )
			{
				return get() += _listener;
			}

			auto& operator+=( const dispatcher_t::listener_t& _listener )
			{
				return get() += _listener;
			}

			auto& operator+=( const dispatcher_t::weak_listener_t& _listener )
			{
				return get() += _listener;
			}

			// Unsubscribes a member function to be called on the assigned self.
			// In case you want to call a function with another class instance, use sk::CreateEvent
			auto& operator+=( const dispatcher_t::sSelfWrapper& _wrapper )
			{
				return get() += _wrapper;
			}
			
			auto& operator-=( const dispatcher_t::event_t& _listener )
			{
				return get() -= _listener;
			}

			auto& operator-=( const dispatcher_t::weak_event_t& _listener )
			{
				return get() -= _listener;
			}

			// Unsubscribes a member function to be called on the assigned self.
			// In case you want to call a function with another class instance, use sk::CreateEvent
			auto& operator-=( const dispatcher_t::sSelfWrapper& _wrapper )
			{
				return get() -= _wrapper;
			}

			size_t add_listener( const dispatcher_t::event_t& _listener )
			{
				return m_dispatcher_->add_listener( _listener );
			} // add_listener

			size_t add_listener( const dispatcher_t::weak_event_t& _listener )
			{
				return m_dispatcher_->add_listener( _listener );
			} // add_listener

			void remove_listener( const dispatcher_t::event_t& _listener )
			{
				m_dispatcher_->remove_listener( _listener );
			}

			void remove_listener( const dispatcher_t::weak_event_t& _listener )
			{
				m_dispatcher_->remove_listener( _listener );
			}

			void remove_listener( const size_t _id )
			{
				m_dispatcher_->remove_listener( _id );
			}

			void push_event( Args... _args )
			{
				m_dispatcher_->push_event( std::forward< Args >( _args )... );
			}

		private:
			auto& get() -> dispatcher_t&
			{
				return *m_dispatcher_;
			}

			std::unique_ptr< dispatcher_t > m_dispatcher_;
		};
	} // Event

	class cEventManager : public cSingleton< cEventManager >
	{
	public:
		// TODO: Logic?
		 cEventManager( void );
		~cEventManager( void ) override;

	template< class... Args >
	std::pair< bool, size_t > registerLister( const hash< Object::eEvents >& _identity, void( *_function )( Args... ) )
	{
		std::function< void( Args... ) > function = _function;
		if( const auto itr = m_dispatcher.find( _identity ); itr != m_dispatcher.end() )
		{
			if( itr->second->get_arg_size() != Event::kTotal_Size< Args... > )
			{
				printf( "Invalid size of arguments." );
				return { false, 0 };
			}

			// Still unsafe if not same size, but no convenient way to check.

			auto dispatcher = static_cast< Event::cEventDispatcher< Args... >* >( itr->second );
			return std::make_pair( true, dispatcher->add_listener( function ) );
		}

		auto dispatcher = SK_SINGLE_EMPTY( Event::cEventDispatcher< Args... > );
		dispatcher->add_listener( function );
		m_dispatcher.emplace( _identity, dispatcher );
		return { true, 0 };
	} // registerLister


	template< class Ty, class... Args >
	std::pair< bool, size_t > registerLister( const hash< Object::eEvents >& _identity, void( Ty::*_function )( Args... ), const cWeak_Ptr< Ty >& _class )
	{
		std::function< bool( Args... ) > function = [ _class, _function ]( Args... _args ){ if( !_class.is_valid() ) return false; ( _class->*_function )( _args... ); return true; };
		if( const auto itr = m_dispatcher.find( _identity ); itr != m_dispatcher.end() )
		{
			if( itr->second->get_arg_size() != Event::kTotal_Size< Args... > )
			{
				printf( "Invalid size of arguments." );
				return { false, 0 };
			}

			// Still unsafe if not same size, but no convenient way to check.

			auto dispatcher = static_cast< Event::cEventDispatcher< Args... >* >( itr->second );
			return std::make_pair( true, dispatcher->add_listener( function ) );
		}

		auto dispatcher = SK_SINGLE_EMPTY( Event::cEventDispatcher<> );
		dispatcher->add_listener( function );
		m_dispatcher.emplace( _identity, dispatcher );
		return { true, 0 };
	} // registerLister

	template< class... Args >
	bool postEvent( const hash< Object::eEvents >& _event, Args... _args )
	{
		const auto itr = m_dispatcher.find( _event );

		if( itr == m_dispatcher.end() )
		{
			//printf( "Event not registered." );
			return false;
		}

		if( itr->second->get_arg_size() != Event::kTotal_Size< Args... > )
		{
			printf( "Invalid size of arguments. \n" );
			return false;
		}

		static_cast< Event::cEventDispatcher< Args... >* >( itr->second )->post_event( std::forward< Args >( _args )... );

		return true;
	} // postEvent

	bool postEvent( const hash< Object::eEvents >& _event )
	{
		const auto itr = m_dispatcher.find( _event );

		if( itr == m_dispatcher.end() )
		{
			//printf( "Event not registered. \n" );
			return false;
		}

		if( itr->second->get_arg_size() != 0 )
		{
			printf( "Invalid size of arguments., %lu \n", TO_LU( itr->second->get_arg_size() ) );
			return false;
		}

		static_cast< Event::cEventDispatcher<>* >( itr->second )->push_event();

		return true;
	} // postEvent

	void unregisterEvent( const hash< Object::eEvents >& _event, const size_t& _id );

	private:
		std::unordered_map< hash< Object::eEvents >, Event::iEventDispatcher* > m_dispatcher = {};

	};

	namespace Event
	{
		class cEventListener
		{
		public:
			         cEventListener( void ) = default;
			virtual ~cEventListener( void );

		protected:
			// TODO: Add a function to create an event. The event will be a struct with a lambda (similar to the raw registration) and some additional info.
			// This will make registering and validation it easier.
			template< class Ty, class... Args >
			size_t RegisterListener( const hash< Object::eEvents >& _identity, void( Ty::*_function )( Args... ) )
			{
				if constexpr( !std::is_base_of_v< cEventListener, Ty > )
				{
					printf( "Using invalid class" );
					return kInvalid_Id;
				}

				cWeak_Ptr< Ty > weak_ptr;

				if constexpr( std::is_base_of_v< cShared_from_this< Ty >, Ty > )
					weak_ptr = static_cast< Ty* >( this )->get_weak();
				else // Thanks to it unregistering all events, this is a safe option.
					weak_ptr = cWeak_Ptr< Ty >::make_unsafe( static_cast< Ty* >( this ) );

				std::pair< bool, size_t > res = cEventManager::get().registerLister< Ty, Args... >( _identity, _function, weak_ptr );
				if( !res.first )
					return kInvalid_Id;

				m_listeners.insert( { _identity, { std::addressof( _function ), res.second } } );
				return res.second;
			} // RegisterEvent

			template< class Ty, class... Args >
			void UnregisterListener( const hash< Object::eEvents >& _identity, void( Ty::*_function )( Args... ) )
			{
				if( const auto id = FindListenerId( _identity, static_cast< void* >( _function ) ); id != kInvalid_Id )
					cEventManager::get().unregisterEvent( _identity, id );
				else
					printf( "Unregistering invalid listener." );
			} // UnregisterEvent

			template< class Ty, class... Args >
			requires ( std::is_base_of_v< cShared_from_this< Ty >, Ty > )
			std::function< bool( Args... ) > CreateListener( std::function< void( Args... ) > _function )
			{
				return [ _function, ptr = static_cast< cShared_from_this< Ty >* >( this )->get_weak() ]( Args... _args )
				{
					if( ptr == nullptr )
						return false;

					_function( _args... );
					
					return true;
				};
			}

			template< class Ty, class... Args >
			std::pair< bool, size_t > FindListenerId( const hash< Object::eEvents >& _event, void( Ty::*_function )( Args... ) )
			{
				auto id = FindListenerId( _event, static_cast< void* >( _function ) );
				return { id != kInvalid_Id, id };
			} // FindListenerId

			static void UnregisterListener( const hash< Object::eEvents >& _event, size_t _id );

			void UnregisterListenersOfType( const hash< Object::eEvents >& _event );

		private:
			size_t FindListenerId( const hash< Object::eEvents >& _event, void* _raw_ptr );

			multimap< hash< Object::eEvents >, std::pair< void*, size_t > > m_listeners;
		};

		template< class Ty >
		class cHelper
		{
		public:
			// Makes a function into a listener function which unsubscribes itself once the class is deleted.
			template< class Ty2, class... Args >
			requires std::is_base_of_v< Ty2, Ty >
			auto CreateEvent( void( Ty2::*_function )( Args...) )
			{
				return sk::CreateEvent( static_cast< Ty* >( this ), _function );
			}
			
			template< class Ty2, class... Args >
			requires std::is_base_of_v< Ty2, Ty >
			auto CreateEvent( void( Ty2::*_function )( Args...) const )
			{
				return sk::CreateEvent( static_cast< Ty* >( this ), _function );
			}

			// Makes a lambda into a listener function which unsubscribes itself once the class is deleted.
			template< class Fu >
			auto CreateEvent( Fu&& _function )
			{
				return sk::CreateEvent( static_cast< Ty* >( this ), std::forward< Fu >( _function ) );
			}

			template< class Ty2, class... Args >
			auto GetFunctionId( void( Ty2::*_function )( Args...) )
			{
				return iEventDispatcher::get_function_hash( _function );
			}

			template< class Ty2, class... Args >
			auto GetFunctionId( void( Ty2::*_function )( Args...) const )
			{
				return iEventDispatcher::get_function_hash( _function );
			}
		};

	} // Event::

	template< class Ty, class... Args >
	size_t Register_Event_Helper( const hash< Object::eEvents >& _event, void( Ty::*_function )( Args... ), Ty* _class )
	{
		if constexpr( std::is_base_of_v< Event::cEventListener, Ty > )
		{
			return _class->RegisterListener( _event, _function );
		}
		else
		{
			static_assert( std::is_base_of_v< cShared_from_this< Ty >, Ty >, "This class must derive from cShared_from_this, or use a static event function." );
			return cEventManager::get().registerLister( _event, _function, _class->get_weak_this() ).second;
		}
	} // Register_Event_Helper

	template< class... Args >
	size_t Register_Event_Helper( const hash< Object::eEvents >& _event, void( *_function )( Args... ), void* )
	{
		return sk::cEventManager::get().registerLister( _event, _function ).second;
	} // Register_Event_Helper

} // sk::

#define EVENT_HASH( Event ) constexpr sk::hash< sk::Object::eEvents > Hash( Event )

#define POST_EVENT( Event ) { \
EVENT_HASH( Event ); \
sk::cEventManager::get().postEvent( Hash ); }

// Has to be called inside a class
#define REGISTER_LISTENER( Event, Function ) { \
EVENT_HASH( Event ); \
sk::Register_Event_Helper( Hash, Function, this ); }
