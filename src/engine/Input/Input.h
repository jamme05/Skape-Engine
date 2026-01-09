/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <variant>
#include <Input/Keyboard_Include.h>
#include <Math/Vector2.h>

namespace sk::Platform
{
	class cPad;
} // sk::Platform::

namespace sk::Input
{
	class iListener;

	enum eInput
	{
		kNone,
	};

	enum eInputType : uint32_t
	{
		kKey_Down      = 0x00000001,
		kKey_Up        = 0x00000002,
		kKey           = kKey_Down | kKey_Up,
		kMouseAbsolute = 0x00000004,
		kMouseRelative = 0x00000008,
		kMouse_Down    = 0x00000010,
		kMouse_Up      = 0x00000020,
		kMouseButton   = kMouse_Down | kMouse_Up,
		kMouse         = kMouseAbsolute | kMouseRelative | kMouseButton,
		kButton_Down   = 0x00000040,
		kButton_Up     = 0x00000080,
		kButton        = kButton_Down | kButton_Up,
		kStick         = 0x00000100,
		kAnalog        = 0x00000200,

		// Custom Events.
		kCustom0     = 0x02000000,
		kCustom1     = 0x04000000,
		kCustom2     = 0x08000000,
		kCustom3     = 0x10000000,
		kCustom4     = 0x20000000,
		kCustom5     = 0x40000000,
		kCustom6     = 0x80000000,
		
		kAll         = 0xffffffff,
	}; // eType

	enum eResponse : uint8_t
	{
		// Prevets other input listener with less priority from getting the event.
		kConsume,
		// Send the event to the next listener.
		kContinue,
		// Tells the application to shut down.
		kQuit
	};

	/**
	 * Variant with bool and eResponse.
	 * 
	 * Bool is if it will continue. So true = eResponse::kContinue and false = eResponse::kConsume.
	 * 
	 * See sk::Input::eResponse for its control.
	 */
	using response_t = std::variant< bool, eResponse >;

	enum class eAnalog : uint8_t
	{
		kNone,
		kMouse,
		kLeft,
		kRight,

	}; // eAnalog

	struct sEvent;
	struct sRawEvent
	{
		const sEvent* current_event;
	};

	// TODO: Document the events.
	struct sPadEvent : sRawEvent
	{
		uint32_t  button          = kNone;
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
		cVector2f relative          = { };
		uint8_t   button            = kNone;
		uint8_t   presses           = 0;
	};

	struct sKeyboardMods
	{
		// 0 = No, 1 = Left Shift, 2 = Right Shift, 3 = Both
		uint8_t  shift : 2;
		// 0 = No, 1 = Left Ctrl, 2 = Right Ctrl, 3 = Both
		uint8_t  ctrl  : 2;
		// 0 = No, 1 = Left Alt, 2 = Right Alt (/Alt Gr), 3 = Both
		uint8_t  alt   : 2;
		uint8_t  caps_lock   : 1;
		uint8_t  scroll_lock : 1;
		// "Windows Key" 0 = No, 1 = Left Os Key, 2 = Right Os Key, 3 = Both
		uint8_t  os_down    : 2;
	};

	struct sKeyboardEvent : sRawEvent
	{
		uint32_t      key    = kNone;
		sKeyboardMods mods   = {};
		uint16_t      repeat = 0;
		[[nodiscard]] bool isFirst() const { return repeat == 0; }
	};

	// Only use data from event TEMPORARILY, it'll be removed after the event.
	// Exceptions for pads, but you'd rather want to get a user for that.
	struct sEvent
	{
		const sPadEvent*      pad         = nullptr;
		const sMouseEvent*    mouse       = nullptr;
		const sKeyboardEvent* keyboard    = nullptr;
		void*                 custom      = nullptr;
	};

	// Disabled in debug
	extern void setLogInputs  ( const bool _log_inputs );
	extern void addListener   (       iListener* _listener );
	extern void removeListener( const iListener* _listener );

	// THESE HAVE TO BE DECLARED IN EITHER A WINDOW OR PLATFORM MODULE.
	extern bool input_event   ( uint32_t _type, sPadEvent&      _event );
	extern bool input_event   ( uint32_t _type, sMouseEvent&    _event );
	extern bool input_event   ( uint32_t _type, sKeyboardEvent& _event );

	// Will call the custom event on the listeners.
	extern bool input_custom_event( uint32_t _type, void* _event );

	// Returns if the application should shut down or not.
	extern bool input_event( uint32_t _type, const sEvent& _event );

	class iListener
	{
		// Could this be combined in some way?
		uint32_t m_filter;
		uint16_t m_priority;
		bool     m_enabled;
	public:

		         iListener( uint32_t _filter, uint16_t _priority, bool _enabled = true );
		virtual ~iListener( void );

		// We don't expect this to be copied or moved.
		iListener( const iListener& ) = delete;
		iListener( iListener&& ) = delete;

		iListener& operator=( const iListener& ) = delete;
		iListener& operator=( iListener&& ) = delete;

		// No setting the priority after creation... For now
		auto getPriority( void ) const                { return m_priority;    }
		bool getEnabled ( void ) const                { return m_enabled;     }
		auto getFilter  ( void ) const -> uint32_t    { return m_filter;      }
		void setEnabled ( const bool _enabled )       { m_enabled = _enabled; }
		void setFilter  ( const uint32_t _filter )    { m_filter = _filter;   }

		// Override either only a single type of input.
		virtual response_t onInput( const uint32_t _type, const sPadEvent&      _event ){ return onInput( _type, *_event.current_event ); }
		virtual response_t onInput( const uint32_t _type, const sMouseEvent&    _event ){ return onInput( _type, *_event.current_event ); }
		virtual response_t onInput( const uint32_t _type, const sKeyboardEvent& _event ){ return onInput( _type, *_event.current_event ); }

		// Or all of them!
		/**
		 * 
		 * @param _type The type of event. See sk::Input::eInputType
		 * @param _event Information about the event.
		 * @return True/sk::Input::kContinue to send the event to the next listener. False/sk::Input::kConsume to consume the input. Or sk::Input::kQuit to quit the application.
		 */
		virtual response_t onInput( const uint32_t _type, const sEvent& _event ) = 0;
	}; // iListener

} // sk::Input::
