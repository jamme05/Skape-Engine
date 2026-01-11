#pragma once

#include <unordered_set>

#include <Input/Input.h>
#include <Misc/Singleton.h>
#include <Reflection/RuntimeStruct.h>
#include <Scene/Scene.h>

namespace sk::Graphics
{
	class cPipeline;
}

namespace sk::Graphics::Rendering
{
	class cRender_Context;
	class cWindow_Context;
}

namespace sk::Platform
{
	class iWindow;
}

namespace sk::Assets
{
	class cShader;
}

MAKE_STRUCT( TestStruct, M( int32_t, wha ) );

MAKE_STRUCT( TestStructNested,
	M( TestStruct, Val1 ),
	M( TestStruct, Val2 ),
)

class cApp : public sk::cSingleton< cApp >, public sk::Input::iListener
{
	bool m_running = true;

public:

	 cApp( void );
	~cApp( void ) override;

	bool running   ( void ) const         { return m_running; }
	void setRunning( const bool _running ){ m_running = _running; m_running_instance_ = m_running ? this : nullptr; }

	sk::Input::response_t onInput( const uint32_t _type, const sk::Input::sEvent& _event ) override;
	
	void create ( void );
	void run    ( void );

	static void print_types( void );

	auto GetMainWindow() const { return m_main_window_; }

	sk::cShared_ptr< sk::cScene > m_scene = nullptr;

	using window_t = sk::Platform::iWindow*;
	std::unordered_set< window_t > m_windows;
	static cApp* getRunningInstance(){ return m_running_instance_; }
	
private:
	void destroy();
	
	window_t     m_main_window_ = nullptr;
	static cApp* m_running_instance_;
};

