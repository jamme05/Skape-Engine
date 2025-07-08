

#include "App.h"

#include "Assets/Material.h"
#include "Assets/cMesh.h"
#include "Assets/Shader.h"
#include "Assets/Asset.h"
#include "Assets/Texture.h"
#include "Assets/Manager/Asset_manager.h"

#include "Embedded/Shaders.h"

#include "Graphics/cRenderer.h"
#include "Graphics/cRender_context.h"

#include "Math/Types.h"
#include "Memory/Tracker/Tracker.h"
#include "Platform/cPlatform.h"

#include "Scene/Scene.h"
#include "Scene/Components/MeshComponent.h"
#include "Scene/Managers/SceneManager.h"
#include "Scene/Objects/CameraFlight.h"

#include "Graphics/cDepth_target.h"
#include "Graphics/cRender_target.h"
#include "Graphics/cUniforms.h"
#include "Graphics/cWindow_context.h"

#include "Containers/allocator.h"
#include "Containers/Const/Const_Wrapper.h"
#include "Containers/Const/String.h"

#include "Reflection/RuntimeClass.h"
#include "Reflection/RuntimeStruct.h"

cApp::cApp( void )
: iListener( sk::Input::eType::kAll, 10, true )
{
	sk::Input::setLogInputs( false );

} // cApp

cApp::~cApp( void )
{
	destroy();
	m_scene = nullptr;
} // ~cApp

bool cApp::onInput( const sk::Input::eType _type, const sk::Input::sEvent& _event )
{
	switch( _type )
	{
	case sk::Input::kButton_Down:
	{
		if( _event.pad->button == sk::Input::eButton::kOptions )
			m_running = false;
	}
	break; // sk::Input::kButton_Down

	default:
		break;
	}

	return false;
} // onInput

void cApp::create( void )
{
	sk::cPlatform::initialize();
	sk::Graphics::cRenderer::initialize();
	sk::cAssetManager::init();
	sk::cSceneManager::init();

	sk::Graphics::cRenderer::get().registerVertexAttribute( sk::Graphics::sBaseVertex::g_attribute_regs );

	m_mesh_pair      = sk::Assets::cShader::create_shared( "Mesh Vertex", Embedded::Shaders::Deferred::Mesh::Vertex, ePrimType_t::kTriList );
	auto temp_shader = sk::Assets::cShader::create_shared( "Mesh Pixel",  Embedded::Shaders::Deferred::Mesh::Pixel,  ePrimType_t::kTriList );

	m_mesh_pair->setAttributes( sk::Graphics::sBaseVertex::g_attribute_regs );
	m_mesh_pair->linkShader( temp_shader );

	m_post_pair = sk::Assets::cShader::create_shared( "Post Vertex", Embedded::Shaders::Post_processing::Vertex, ePrimType_t::kRectList );
	temp_shader = sk::Assets::cShader::create_shared( "Post Pixel",  Embedded::Shaders::Post_processing::Pixel,  ePrimType_t::kRectList );

	m_post_pair->linkShader( temp_shader );

	//auto list   = sk::cAssetManager::get().loadFolder( "data/" );
	auto list_1 = sk::cAssetManager::get().loadFile( "data/humanforscale.glb" );
	auto list_2 = sk::cAssetManager::get().loadFile( "data/heheToiletwithtextures.glb" );

	//sk::cAssetManager::get().loadFile( "data/mushroom.glb" );
	
	//auto cube          = sk::cAssetManager::get().getAssetAs< sk::Assets::cMesh >( 0 );
	auto christopher_t = list_1.Get_Asset_Of_Type< sk::Assets::cTexture >();
	auto christopher_m = list_1.Get_Asset_Of_Type< sk::Assets::cMesh    >();
	auto toilet_t      = list_2.Get_Asset_Of_Type< sk::Assets::cTexture >();
	auto toilet_m      = list_2.Get_Asset_Of_Type< sk::Assets::cMesh    >();
	//auto mushroom      = sk::cAssetManager::get().getAssetAs< sk::Assets::cMesh >( 5 );

	m_scene = sk::cScene::create_shared( "Main" );
	m_scene->create_object< sk::Object::cCameraFlight >( "Camera Free Flight" )->setAsMain();
	//auto mesh = m_scene->create_object< sk::Object::iObject >( "Mesh Test 1" );
	//mesh->getTransform().getPosition() = { -10.0f, 0.0f, 0.0f };
	//mesh->getTransform().update();
	//mesh->addComponent< sk::Object::Components::cMesh >( cube );

	auto mesh = m_scene->create_object< sk::Object::iObject >( "Mesh Test 2" );
	mesh->getTransform().getPosition() = { -2.0f, 0.0f, 0.0f };
	mesh->getTransform().update();
	mesh->addComponent< sk::Object::Components::cMeshComponent >( christopher_m )->setTexture( christopher_t );

	mesh = m_scene->create_object< sk::Object::iObject >( "Mesh Test 3" );
	mesh->getTransform().getPosition() = { 2.0f, 0.0f, 0.0f };
	mesh->getTransform().update();
	mesh->addComponent< sk::Object::Components::cMeshComponent >( toilet_m )->setTexture( toilet_t );

	//mesh = m_scene->create_object< sk::Object::iObject >( "Mesh Test 4" );
	//mesh->getTransform().getPosition() = { 10.0f, 0.0f, 0.0f };
	//mesh->getTransform().update();
	//mesh->addComponent< sk::Object::Components::cMesh >( cube )->setTexture( toilet_t );

	sk::cSceneManager::get().registerScene( m_scene );

	REGISTER_LISTENER( "Custom Event", &cApp::custom_event )
	sk::Register_Event_Helper( sk::Const< sk::str_hash >( "Aaaa" ), &cApp::custom_event, this );

	sk::Assets::cMaterial material{ "Test", *m_mesh_pair };

	constexpr int arr1[]{ 1, 2, 3 };
	constexpr sk::array t1{ arr1 };
	constexpr sk::array t2{ 3, 2, 1 };
	constexpr sk::array t3{ t2 };
	//constexpr int* arr2 = t3;
	constexpr auto t4 = t2 + t1;
	constexpr auto v = sk::arr::concat< t1, { 1 } >::kValue;

	constexpr auto v2 = sk::args_hash< void >::kTypes;
	constexpr auto v3 = sk::validate_args( v2, true );

	print_types();
} // _create

void cApp::print_types( void )
{
	for( const auto& val : sk::type_map | std::views::values )
	{
		switch( val->type )
		{
		case sk::sType_Info::eType::kStandard:
		{
			printf( "Type: %s Name: %s Size: %lu \n", val->raw_name, val->name, static_cast< uint64_t >( val->size ) );
		}
		break;
		case sk::sType_Info::eType::kStruct:
		{
			printf( "Struct: %s Name: %s Size: %lu \n", val->raw_name, val->name, static_cast< uint64_t >( val->size ) );
			for( auto struct_info = val->as_struct_info(); const auto& member : struct_info->members | std::views::values )
			{
				if( auto member_type = member.get_type() )
				{
					printf( "   Type: %s Name: %s Size: %lu Offset: %lu \n", member_type->raw_name, member.display_name, static_cast< uint64_t >( member.size ), static_cast< uint64_t >( member.offset ) );
				}
			}
		}
		break;
		}
	}
}

void cApp::custom_event( void )
{
	printf( "Custom Event go brrr\n" );
} // custom_event

void cApp::destroy( void )
{
	sk::cSceneManager::shutdown();
	sk::cAssetManager::shutdown();
	sk::Graphics::cRenderer::deinitialize();
	sk::cPlatform::deinitialize();

} // _destroy

void cApp::run( void )
{
	sk::cPlatform::Update();
	sk::cSceneManager::get().update();

	auto& resolution   = sk::Graphics::cRenderer::get().getWindowResolution();
	sk::cVector2i ires = resolution;

	sk::Graphics::sViewport viewport{ 0, 0, resolution.x, resolution.y };
	sk::Graphics::sScissor  scissor { 0, 0, ires.x, ires.y };

	if( !sk::Graphics::cRenderer::get().getRenderContext().setShader( *m_mesh_pair ) )
		__debugbreak();

	// Removed due to NDA

	sk::cSceneManager::render();

	//auto& r_ctx = m_render_context->getBackContext();

	static uint64_t counter = 0;
	if( counter++ % 100 == 0 )
	{
		POST_EVENT( "Custom Event" )
	}

	auto& window_context = sk::Graphics::cRenderer::get().getWindowContext();

	window_context.begin( viewport, scissor );
	window_context.clear( sk::Graphics::eClear::kAll, sk::Color::kBlack );

	auto& ctx = window_context.getBackContext();

	// Removed due to NDA

	window_context.end();

} // run
