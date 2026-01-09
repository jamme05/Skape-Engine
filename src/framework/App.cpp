

#include "App.h"

#include <print>
#include <random>

#include <Assets/Management/Asset_Manager.h>
#include <Assets/Utils/Asset_List.h>
#include <Graphics/Renderer.h>
#include <Graphics/Rendering/Render_Context.h>
#include "Assets/Asset.h"
#include "Assets/Texture.h"
#include "Assets/Management/Asset_Job_Manager.h"
#include "Graphics/Renderer_Impl.h"
#include "Graphics/Pipelines/Deferred_Pipeline.h"
#include "Graphics/Pipelines/Forward_Pipeline.h"
#include "Graphics/Pipelines/Pipeline.h"
#include "Math/Types.h"
#include "Memory/Tracker/Tracker.h"
#include "Misc/UUID.h"
#include "Platform/Time.h"
#include "Platform/Window/Window_Base.h"
#include "Reflection/RuntimeClass.h"
#include "Reflection/RuntimeStruct.h"
#include "Scene/Scene.h"
#include "Scene/Components/MeshComponent.h"
#include "Scene/Components/SpinComponent.h"
#include "Scene/Managers/SceneManager.h"
#include "Scene/Objects/CameraFlight.h"

cApp* cApp::m_running_instance_ = nullptr;

cApp::cApp( void )
: iListener( sk::Input::eInputType::kAll, 10, true )
{
	m_running_instance_ = this;
	sk::Input::setLogInputs( false );
	
	sk::cAsset_Manager::init();

	m_main_window_ = sk::Platform::CreateWindow( "Main Window", { 1280, 720 } );
	m_main_window_->PushContext();
	
	sk::Graphics::InitRenderer();
	m_main_window_->Init();
	
	m_windows.emplace( m_main_window_ );

	SK_ERR_IFN( m_main_window_->SetVisibility( true ),
		"Unable to show window." )
	
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
	auto& asset_m = sk::cAsset_Manager::get();

	auto list_1 = asset_m.loadFile( "models/humanforscale.glb" );
	auto list_2 = asset_m.loadFile( "models/heheToiletwithtextures.glb" );
	const auto shader_frag = *asset_m.loadFile( "shaders/gpass.frag" ).begin();
	const auto shader_vert = *asset_m.loadFile( "shaders/default.vert" ).begin();
	asset_m.loadFile( "shaders/screen.vert" );
	asset_m.loadFile( "shaders/deferred.frag" );	
	
	// TODO: Create a material instance class.
	
	// sk::Graphics::cRenderer::get().SetPipeline( SK_SINGLE( sk::Graphics::cForward_Pipeline, m_main_window_ ) );
	sk::Graphics::cRenderer::get().SetPipeline( SK_SINGLE( sk::Graphics::cDeferred_Pipeline, m_main_window_ ) );

	const auto christopher_t = list_1.GetAssetOfType< sk::Assets::cTexture >();
	auto christopher_m = list_1.GetAssetOfType< sk::Assets::cMesh    >();
	const auto toilet_t      = list_2.GetAssetOfType< sk::Assets::cTexture >();
	auto toilet_m      = list_2.GetAssetOfType< sk::Assets::cMesh    >();

	m_scene = sk::make_shared< sk::cScene >();
	m_scene->create_object< sk::Object::cCameraFlight >( "Camera Free Flight" )->setAsMain();
	
	auto mat1 = asset_m.CreateAsset< sk::Assets::cMaterial >( "Material Test",
	sk::Graphics::Utils::cShader_Link{ shader_vert, shader_frag } );
	mat1.second->SetTexture( "mainTexture", christopher_t );

	auto mesh = m_scene->create_object< sk::Object::iObject >( "Mesh Test 2" );
	mesh->GetTransform().getPosition() = { 1.0f, 0.0f, 3.5f };
	mesh->GetTransform().update();
	auto spin_component = mesh->AddComponent< sk::Object::Components::cSpinComponent >();
	auto component = mesh->AddComponent< sk::Object::Components::cMeshComponent >( christopher_m, mat1.first );
	component->enabled();
	component->GetTransform().update();
	component->SetParent( spin_component );
	
		
	auto mat2 = asset_m.CreateAsset< sk::Assets::cMaterial >( "Material Test 2",
		sk::Graphics::Utils::cShader_Link{ shader_vert, shader_frag } );
	mat2.second->SetTexture( "mainTexture", toilet_t );

	mesh = m_scene->create_object< sk::Object::iObject >( "Mesh Test 3" );
	mesh->GetTransform().getPosition() = { -2.0f, 0.0f, 3.5f };
	mesh->GetTransform().update();
	spin_component = mesh->AddComponent< sk::Object::Components::cSpinComponent >();
	component = mesh->AddComponent< sk::Object::Components::cMeshComponent >( toilet_m, mat2.first );
	component->enabled();
	component->GetTransform().update();
	component->SetParent( spin_component );

	sk::cSceneManager::get().registerScene( m_scene );
} // create

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

void cApp::destroy() const
{
	sk::cSceneManager::shutdown();
	for( const auto& window : m_windows )
		SK_DELETE( window );

	sk::cAsset_Manager::shutdown();

	// I don't really like the choice of having the renderer shut down after the asset system,
	// with the asset system being started before the renderer.
	// TODO: Add some RegisterRendererListeners function or something in the future.
	sk::Graphics::cRenderer::shutdown();
	sk::cStringIDManager::shutdown();

} // _destroy

void cApp::run()
{
	sk::Time::Update();
	
	sk::cSceneManager::get().update();
	auto& pipeline = *sk::Graphics::cRenderer::get().GetPipeline();
	
	sk::Graphics::cRenderer::get().Update();
	
	pipeline.Execute();
	
} // run
