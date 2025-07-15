/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Input/Input.h"

#include <mutex>
#include <print>

#include "Memory/Tracker/Tracker.h"

namespace sk
{
	namespace Input
	{
		namespace
		{
			// TODO: Replace with map
			std::vector< iListener* > listeners = { };
			std::mutex                mtx;
			bool                      log_inputs = false;
		} // Table

		void setLogInputs( const bool _log_inputs )
		{
#if defined( DEBUG )
			log_inputs = _log_inputs;
#else // DEBUG
			( void ) _log_inputs;
#endif // !DEBUG
		} // setLogInputs

		void addListener( iListener* _listener )
		{
			std::lock_guard lock( mtx );

			const auto itr = std::ranges::find_if( listeners , [ _listener ]( const iListener* _l )
			{
				return _l->getPriority() > _listener->getPriority();
			} );

			if( itr != listeners.end() )
				listeners.insert( itr, _listener );
			else
				listeners.push_back( _listener );

		} // addListener

		void removeListener( const iListener* _listener )
		{
			std::lock_guard lock( mtx );

			if( const auto itr = std::ranges::find( listeners, _listener ); itr != listeners.end() )
				listeners.erase( itr );

		} // addListener

		void input_event( const eInputType _type, sPadEvent& _event )
		{
			const sEvent event{ .pad = &_event };
			_event.current_event = &event;
			input_event( _type, event );
		} // input_event

		void input_event( const eInputType _type, sMouseEvent& _event )
		{
			const sEvent event{ .mouse = &_event };
			_event.current_event = &event;
			input_event( _type, event );
		} // input_event

		void input_event( const eInputType _type, sKeyboardEvent& _event )
		{
			const sEvent event{ .keyboard = &_event };
			_event.current_event = &event;
			input_event( _type, event );
		} // input_event

		void input_event( const eInputType _type, const sEvent& _event )
		{
#if defined( DEBUG )
			if( log_inputs )
			{
				// If it's a pad event.
				if( _event.pad )
				{
					const char* target_analog = _event.pad->analog == eAnalog::kLeft ? "Left " : "Right";

					// TODO: Add some way to externally add input prints.
					// Maybe a debug_printer using the oninput?
					switch( _type )
					{
					case eInputType::kStick:
						std::println( "Stick       Event: {} Stick, {:.2f} {:.2f} ",
							target_analog, _event.pad->current_stick.x, _event.pad->current_stick.y );
					break;
					case eInputType::kButton_Down:
						std::println( "Button_Down Event: {:6} ", static_cast< int32_t >( _event.pad->button ) );
					break;
					case eInputType::kButton_Up:
						std::println( "Button_Up   Event: {:6} ", static_cast< int32_t >( _event.pad->button ) );
					break;
					case eInputType::kAnalog:
						std::println( "Analog      Event: {} {:.2f} ", target_analog, _event.pad->current_analog );
					break;
					default: std::println( "Developer too lazy to implement." ); break;
					}
				}
			}
#endif // DEBUG

			for( const auto& listener : listeners )
			{
				if( listener->getEnabled() && listener->getFilter() & _type )
				{
					if( listener->onInput( _type, _event ) == eResponse::kStop )
						break;
				}
			}

		} // event

		iListener::iListener( const uint32_t _filter, const uint16_t _priority, const bool _enabled )
		: m_filter( _filter )
		, m_priority( _priority )
		, m_enabled( _enabled )
		{
			Input::addListener( this );

		} // iListener

		iListener::~iListener( void )
		{
			Input::removeListener( this );

		} // ~iListener

	} // Input::

} // sk::
