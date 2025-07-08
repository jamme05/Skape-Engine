#pragma once

#include <Misc/Singleton.h>

#include "Scene/Scene.h"
#include "Input/Input.h"

#include <Reflection/RuntimeStruct.h>

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
	void setRunning( const bool _running ){ m_running = _running; }

	bool onInput( const sk::Input::eType _type, const sk::Input::sEvent& _event ) override;

	void create ( void );
	void destroy( void );
	void run    ( void );

	static void print_types( void );
	static void custom_event( void );

	sk::cShared_ptr< sk::Assets::cShader > m_mesh_pair = nullptr;
	sk::cShared_ptr< sk::Assets::cShader > m_post_pair = nullptr;

	sk::cShared_ptr< sk::cScene > m_scene = nullptr;
	std::vector< sk::cShared_ptr< sk::Assets::cMesh > > m_meshes = {};
};
