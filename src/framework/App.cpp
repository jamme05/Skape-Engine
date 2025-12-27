

#include "App.h"

#include "Assets/Asset.h"
#include <Assets/Asset_List.h>
#include <Assets/Management/Asset_Manager.h>
#include "Assets/Texture.h"

#include "Math/Types.h"
#include "Memory/Tracker/Tracker.h"

#include "Scene/Scene.h"
#include "Scene/Components/MeshComponent.h"
#include "Scene/Managers/SceneManager.h"
#include "Scene/Objects/CameraFlight.h"

#include "Containers/Const/Const_Wrapper.h"
#include "Graphics/Renderer_Impl.h"
#include "Platform/Window/Window_Base.h"

#include "Reflection/RuntimeClass.h"
#include "Reflection/RuntimeStruct.h"

#include <print>
#include <random>

#include <Graphics/Rendering/Render_Context.h>

#include "Graphics/Rendering/Window_Context.h"
#include "Misc/UUID.h"

#include <Graphics/Renderer.h>

#include "Assets/Asset_Ptr.h"

cApp* cApp::m_running_instance_ = nullptr;

cApp::cApp( void )
: iListener( sk::Input::eInputType::kAll, 10, true )
{
	m_running_instance_ = this;
	sk::Input::setLogInputs( false );

	m_main_window = sk::Platform::CreateWindow( "Main Window", { 1280, 720 } );
	m_windows.emplace( m_main_window );

	SK_ERR_IFN( m_main_window->SetVisibility( true ),
		"Unable to show window." )

	sk::cAsset_Manager::init();
	// TODO: Move renderer initialization elsewhere.
#if defined( SK_GRAPHICS_OPENGL )
	sk::Graphics::cGLRenderer::init();
#elif defined( SK_GRAPHICS_VULKAN ) // SK_GRAPHICS_OPENGL
	sk::Graphics::cVKRenderer::init();
#endif // SK_GRAPHICS_VULKAN
	
	sk::Graphics::InitRenderer();

	m_window_context = sk::make_shared< sk::Graphics::Rendering::cWindow_Context >( m_main_window->GetResolution() );
	m_render_context = sk::make_shared< sk::Graphics::Rendering::cRender_Context >();

	sk::cSceneManager::init();
} // cApp

cApp::~cApp( void )
{
	destroy();
	m_scene = nullptr;
} // ~cApp

sk::Input::response_t cApp::onInput( const uint32_t _type, const sk::Input::sEvent& _event )
{
	if( _type == sk::Input::kKey_Down && _event.keyboard->key == sk::Input::Keyboard::kEscape )
		return sk::Input::eResponse::kQuit;

	return true;
} // onInput

void cApp::create( void )
{
	//auto list   = sk::cAssetManager::get().loadFolder( "data/" );
	auto list_1 = sk::cAsset_Manager::get().loadFile( "data/humanforscale.glb" );
	auto list_2 = sk::cAsset_Manager::get().loadFile( "data/heheToiletwithtextures.glb" );

	//sk::cAssetManager::get().loadFile( "data/mushroom.glb" );
	
	//auto cube          = sk::cAssetManager::get().getAssetAs< sk::Assets::cMesh >( 0 );
	auto christopher_t = list_1.Get_Asset_Of_Type< sk::Assets::cTexture >();
	auto christopher_m = list_1.Get_Asset_Of_Type< sk::Assets::cMesh    >();
	auto toilet_t      = list_2.Get_Asset_Of_Type< sk::Assets::cTexture >();
	auto toilet_m      = list_2.Get_Asset_Of_Type< sk::Assets::cMesh    >();
	//auto mushroom      = sk::cAssetManager::get().getAssetAs< sk::Assets::cMesh >( 5 );

	m_scene = sk::make_shared< sk::cScene >();
	m_scene->create_object< sk::Object::cCameraFlight >( "Camera Free Flight" )->setAsMain();

	sk::cAsset_Ptr test = sk::cAsset_Manager::get().GetAssetPtrByName( "Mesh", nullptr );

	std::function< void( int ) > test69 = []( int ){ return true; };
	
	// Don't use lambdas for this. Here it's only shown as a quick example and would probably break in practice
	test.on_asset_loaded += CreateEvent( []( bool, const sk::cAsset& _asset )
	{
		std::println( "Asset: {} loaded", _asset.GetMeta()->GetName() );
	} );

	auto event = CreateEvent( []( const sk::cAsset& _asset )
	{
		std::println( "Asset: {} loaded", _asset.GetMeta()->GetName() );
	} );

	auto test_asset = sk::make_shared< sk::Assets::cMesh >( "Some name" );

	sk::Event::cEventDispatcher< int > example_dispatcher { test_asset };
	example_dispatcher += sk::Assets::cMesh::test;
	example_dispatcher -= sk::Assets::cMesh::test;
	example_dispatcher.add_listener( CreateEvent( sk::Assets::cMesh::test ) );
	example_dispatcher.remove_listener_by_id( CreateEvent( sk::Assets::cMesh::test ) );
	
	auto t = []( const sk::cAsset& _asset )
	{
		std::println( "Asset: {} loaded", _asset.GetMeta()->GetName() );
	};
	auto t3 = sk::Event::sEvent{ []( const sk::cAsset& _asset )
	{
		std::println( "Asset: {} loaded", _asset.GetMeta()->GetName() );
	} };
	
	auto test7 = sk::Event::sEvent( t );
	using test5 = sk::Event::function_type_t< &decltype(t)::operator() >;
	auto t2 = std::bind_front( t );
	
	test.on_asset_updated += sk::CreateEvent( this, []( const sk::cAsset& _asset )
	{
		std::println( "Asset: {} updated", _asset.GetMeta()->GetName() );
	} );
	test.on_asset_unloaded += []( bool, const sk::cAsset_Meta& _meta )
	{
		std::println( "Asset: {} unloaded", _meta.GetName() );
	};

	test.on_asset_unloaded += CreateEvent( []( bool, const sk::cAsset_Meta& _meta )
	{
		
	} );

	auto test4 = std::function< void() >{ std::bind_front( &create, this ) };
	sk::Event::sEvent test3 = test4;

	auto test2 = sk::CreateEvent( this, create );
	
	//auto mesh = m_scene->create_object< sk::Object::iObject >( "Mesh Test 1" );
	//mesh->getTransform().getPosition() = { -10.0f, 0.0f, 0.0f };
	//mesh->getTransform().update();
	//mesh->addComponent< sk::Object::Components::cMesh >( cube );

	auto mesh = m_scene->create_object< sk::Object::iObject >( "Mesh Test 2" );
	mesh->GetTransform().getPosition() = { -2.0f, 0.0f, 0.0f };
	mesh->GetTransform().update();
	mesh->AddComponent< sk::Object::Components::cMeshComponent >( christopher_m )->setTexture( christopher_t );

	mesh = m_scene->create_object< sk::Object::iObject >( "Mesh Test 3" );
	mesh->GetTransform().getPosition() = { 2.0f, 0.0f, 0.0f };
	mesh->GetTransform().update();
	mesh->AddComponent< sk::Object::Components::cMeshComponent >( toilet_m )->setTexture( toilet_t );

	//mesh = m_scene->create_object< sk::Object::iObject >( "Mesh Test 4" );
	//mesh->getTransform().getPosition() = { 10.0f, 0.0f, 0.0f };
	//mesh->getTransform().update();
	//mesh->addComponent< sk::Object::Components::cMesh >( cube )->setTexture( toilet_t );

	sk::cSceneManager::get().registerScene( m_scene );

	REGISTER_LISTENER( "Custom Event", &cApp::custom_event )
	sk::Register_Event_Helper( sk::Const( sk::str_hash( "Aaaa" ) ), &cApp::custom_event, this );
	
	print_types();
} // _create

void cApp::print_types( void )
{
	std::vector< sk::cUUID > uuids{ 1024 };
	for( auto& uuid : uuids )
		uuid = sk::GenerateRandomUUID();

	for( auto& uuid : uuids )
		std::println( "{} {}", uuid.to_string(), uuid.to_string( false ) );

	auto& types = sk::Reflection::cType_Manager::get().GetTypes();

	for( const auto& val : types | std::views::values )
	{
		std::println();
		switch( val->type )
		{
		case sk::sType_Info::eType::kStandard:
		{
			std::println( "Type: {} Name: {} Size: {}", val->raw_name, val->name, static_cast< uint64_t >( val->size ) );
		}
		break;
		case sk::sType_Info::eType::kStruct:
		{
			std::println( "Struct: {} Name: {} Size: {}", val->raw_name, val->name, static_cast< uint64_t >( val->size ) );
			for( const auto struct_info = val->as_struct_info(); const auto& member : struct_info->members | std::views::values )
			{
				const auto member_type = member.type;
				std::println( "   Type: {} Name: {} Size: {} Offset: {}", member_type->raw_name, member.display_name, static_cast< uint64_t >( member.size ), static_cast< uint64_t >( member.offset ) );
			}
		}
		break;
		case sk::sType_Info::eType::kClass:
		{
			const auto class_info = val->as_class_info();
			auto variables = class_info->runtime_class->getVariables();
			auto functions = class_info->runtime_class->getFunctions();
			std::println( "Class: {}, Name: {}, Size: {}, Variables: {}, Functions: {}",
				class_info->raw_name, class_info->name, class_info->size,
				variables.size(), functions.size() );

			std::println( "Variables:" );
			for( const auto variable : variables | std::views::values )
			{
				auto type        = variable->getType()->raw_name;
				auto name        = variable->getName();
				auto visibility  = variable->getVisibilityStr();
				auto is_static   = variable->getIsStatic  () ? "static " : "";
				auto is_const    = variable->getIsReadOnly() ? "const "  : "";
				auto flags       = variable->getFlags();
				// Example: protected int hello
				std::println( "    {}: {}{}{} {} Flags: {}", visibility, is_static, is_const, type, name, flags );
			}

			std::println( "Functions:" );
			for( const auto function : functions | std::views::values )
			{
				auto name        = function->getName();
				auto return_type = function->getReturnType()->raw_name;
				auto args_types  = function->getArgumentTypes()->to_raw_string();
				auto visibility  = function->getVisibilityStr();
				auto flags       = function->getFlags();
				auto is_static   = function->getIsStatic() ? " static" : "";
				auto is_const    = function->getIsReadOnly() ? "const " : "";
				// Example: static private void test_function( bool, int )
				std::println( "    {}:{} {} {}( {} ) {} Flags: {}",
					visibility, is_static, return_type, name, args_types, is_const, flags );
			}
		}
		break;
		default: break;
		}
	}
}

void cApp::custom_event( void )
{
	std::println( "Custom Event go brrr" );
} // custom_event

void cApp::destroy( void ) const
{
	sk::cSceneManager::shutdown();
	for( const auto& window : m_windows )
		SK_FREE( window );

	sk::Graphics::cRenderer::shutdown();

	sk::cAsset_Manager::shutdown();

} // _destroy

void cApp::run( void )
{
	auto& scene_manager = sk::cSceneManager::get();
	scene_manager.update();
	scene_manager.render();
	
} // run
