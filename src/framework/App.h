#pragma once

#include <Misc/Singleton.h>

#include "Scene/Scene.h"
#include "Input/Input.h"

#include <Reflection/RuntimeStruct.h>

namespace sk::Platform
{
	class iWindow;
}

namespace sk::Assets {
	class cShader;
}

namespace sk::Graphics::Agc
{
	class cWindow_context;
} // sk::Graphics::Agc

namespace sk::Graphics
{
	class cRender_context;
} // sk::Graphics::

MAKE_STRUCT( TestStruct, M( int32_t, wha ) );

MAKE_STRUCT( TestStructNested,
	M( TestStruct, Val1 ),
	M( TestStruct, Val2 ),
)

class cApp : public sk::cSingleton< cApp >, sk::Input::iListener
{
	bool m_running = true;

public:

	 cApp( void );
	~cApp( void ) override;

	bool running   ( void ) const         { return m_running; }
	void setRunning( const bool _running ){ m_running = _running; m_running_instance_ = m_running ? this : nullptr; }

	sk::Input::eResponse onInput( const uint32_t _type, const sk::Input::sEvent& _event ) override;

	void create ( void );
	void run    ( void );

	static void print_types( void );
	static void custom_event( void );

	sk::cShared_ptr< sk::Assets::cShader > m_mesh_pair = nullptr;
	sk::cShared_ptr< sk::Assets::cShader > m_post_pair = nullptr;

	sk::cShared_ptr< sk::cScene > m_scene = nullptr;

	using window_t = sk::Platform::iWindow*;
	std::unordered_set< window_t > m_windows;
	static cApp* getRunningInstance(){ return m_running_instance_; }

private:
	void destroy( void ) const;
	
	window_t     m_main_window = nullptr;
	static cApp* m_running_instance_;
};
