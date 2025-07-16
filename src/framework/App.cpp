

#include "App.h"

#include "Assets/Asset.h"
#include "Assets/Texture.h"
#include <Assets/Manager/Asset_Manager.h>
#include <Assets/Asset_List.h>

#include "Math/Types.h"
#include "Memory/Tracker/Tracker.h"

#include "Scene/Scene.h"
#include "Scene/Components/MeshComponent.h"
#include "Scene/Managers/SceneManager.h"
#include "Scene/Objects/CameraFlight.h"

#include "Containers/Const/Const_Wrapper.h"
#include "Graphics/Renderer.h"
#include "Platform/Window/Window_Base.h"

#include "Reflection/RuntimeClass.h"
#include "Reflection/RuntimeStruct.h"

cApp* cApp::m_running_instance_ = nullptr;

cApp::cApp( void )
: iListener( sk::Input::eInputType::kAll, 10, true )
{
	m_running_instance_ = this;
	sk::Input::setLogInputs( false );

	m_main_window = sk::Platform::create_window( "Main Window", { 1280 , 720  } );
	m_windows.emplace( m_main_window );

	SK_ERR_IFN( m_main_window->SetVisibility( true ), "Unable to show window." )

	sk::Graphics::cRenderer::init();

} // cApp

cApp::~cApp( void )
{
	destroy();
	m_scene = nullptr;
} // ~cApp

sk::Input::eResponse cApp::onInput( const uint32_t _type, const sk::Input::sEvent& _event )
{
	if( _type == sk::Input::kKey_Down && _event.keyboard->key == sk::Input::Keyboard::kEscape )
		return sk::Input::eResponse::kQuit;

	return sk::Input::eResponse::kContinue;
} // onInput

void cApp::create( void )
{
	sk::cAssetManager::init();
	sk::cSceneManager::init();

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
	// m_scene->create_object< sk::Object::cCameraFlight >( "Camera Free Flight" )->setAsMain();
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
	sk::Register_Event_Helper( sk::Const( sk::str_hash( "Aaaa" ) ), &cApp::custom_event, this );

	print_types();
} // _create

void cApp::print_types( void )
{
	for( const auto& val : sk::type_map | std::views::values )
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
			for( auto struct_info = val->as_struct_info(); const auto& member : struct_info->members | std::views::values )
			{
				if( auto member_type = member.get_type() )
				{
					std::println( "   Type: {} Name: {} Size: {} Offset: {}", member_type->raw_name, member.display_name, static_cast< uint64_t >( member.size ), static_cast< uint64_t >( member.offset ) );
				}
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
				auto flags       = variable->getFlags();
				// Example: protected int hello
				std::println( "    {}: {} {} Flags: {}", visibility, type, name, flags );
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
				// Example: static private void test_function( bool, int )
				std::println( "    {}:{} {} {}( {} ) Flags: {}",
					visibility, is_static, return_type, name, args_types, flags );
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

void cApp::destroy( void ) const
{
	sk::cSceneManager::shutdown();
	sk::cAssetManager::shutdown();
	for( const auto& window : m_windows )
		SK_FREE( window );

	sk::Graphics::cRenderer::shutdown();

} // _destroy

void cApp::run( void )
{
	sk::cSceneManager::get().update();
} // run
