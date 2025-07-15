/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Input/Input_Keys.h>
#include <Math/Vector2.h>

namespace sk::Platform
{
	class cPad;
} // sk::Platform::

namespace sk::Input
{
	class iListener;

	enum eInputType : uint32_t
	{
		kKey_Down    = 0x00000001,
		kKey_Up      = 0x00000002,
		kKey         = kKey_Down | kKey_Up,
		kMouse       = 0x00000004,
		kMouse_Down  = 0x00000008,
		kMouse_Up    = 0x00000010,
		kButton_Down = 0x00000020,
		kButton_Up   = 0x00000040,
		kButton      = kButton_Down | kButton_Up,
		kStick       = 0x00000080,
		kAnalog      = 0x00000100,
		kAll         = 0xffffffff,

	}; // eType

	enum class eResponse : uint8_t
	{
		kStop,
		kContinue,
	};

	enum class eAnalog : uint8_t
	{
		kMouse,
		kLeft,
		kRight,

	}; // eAnalog

	struct sEvent;
	struct sRawEvent
	{
		const sEvent* current_event;
	};

	struct sPadEvent : sRawEvent
	{
		uint32_t  button          = eType::kNone;
		eAnalog   analog          = eAnalog::kMouse;
		cVector2f current_stick   = { };
		cVector2f previous_stick  = { };
		float     current_analog  = { };
		float     previous_analog = { };
		Platform::cPad* pad       = nullptr;
	};

	struct sMouseEvent : sRawEvent
	{
		eAnalog analog = eAnalog::kMouse;
		cVector2f current_position  = { };
		cVector2f previous_position = { };
	};

	struct sKeyboardEvent : sRawEvent
	{
		uint32_t key    = eType::kNone;
		uint32_t repeat = 0;
		bool isFirst() const { return repeat == 0; }
	};

	// Only use data from event TEMPORARILY, it'll be removed after the event.
	// Exceptions for pads, but you'd rather want to get a user for that.
	struct sEvent
	{
		const sPadEvent*      pad      = nullptr;
		const sMouseEvent*    mouse    = nullptr;
		const sKeyboardEvent* keyboard = nullptr;
	};

	// Disabled in debug
	extern void setLogInputs  ( const bool _log_inputs );
	extern void addListener   (       iListener* _listener );
	extern void removeListener( const iListener* _listener );

	// THESE HAVE TO BE DECLARED IN EITHER A WINDOW OR PLATFORM MODULE.
	extern void input_event   ( const eInputType _type, sPadEvent&      _event );
	extern void input_event   ( const eInputType _type, sMouseEvent&    _event );
	extern void input_event   ( const eInputType _type, sKeyboardEvent& _event );

	extern void input_event   ( const eInputType _type, const sEvent&   _event );

	class iListener
	{
		uint32_t m_filter;
		uint16_t m_priority;
		bool     m_enabled  = false;

	public:

		         iListener( const uint32_t _filter, const uint16_t _priority, const bool _enabled = false );
		virtual ~iListener( void );

		// No setting the priority after creation... For now
		auto getPriority( void ) const                { return m_priority;    }
		bool getEnabled ( void ) const                { return m_enabled;     }
		auto getFilter  ( void ) const -> uint32_t    { return m_filter;      }
		void setEnabled ( const bool _enabled )       { m_enabled = _enabled; }
		void setFilter  ( const uint32_t _filter )    { m_filter = _filter;   }

		// Override either only a single type of input.
		virtual eResponse onInput( const eInputType _type, const sPadEvent&      _event ){ return onInput( _type, *_event.current_event ); }
		virtual eResponse onInput( const eInputType _type, const sMouseEvent&    _event ){ return onInput( _type, *_event.current_event ); }
		virtual eResponse onInput( const eInputType _type, const sKeyboardEvent& _event ){ return onInput( _type, *_event.current_event ); }

		// Or all of them!
		virtual eResponse onInput( const eInputType _type, const sEvent& _event ) = 0;

	}; // iListener

} // sk::Input::
