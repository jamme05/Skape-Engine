//
// Created by jag on 2026-02-03.
//

#include "Editor.h"

#include <sk/Assets/Material.h>
#include <sk/Assets/Management/Asset_Manager.h>
#include <sk/Graphics/Renderer.h>
#include <sk/Graphics/Pipelines/Deferred_Pipeline.h>
#include <sk/Graphics/Pipelines/Pipeline.h>
#include <sk/Graphics/Utils/RenderUtils.h>
#include <sk/Graphics/Utils/Shader_Link.h>
#include <sk/Input/Input.h>
#include <sk/Platform/Time.h>
#include <sk/Platform/ImGui/ImGuiHelper.h>
#include <sk/Platform/Window/Window_Base.h>
#include <sk/Scene/Scene.h>
#include <sk/Scene/Components/LightComponent.h>
#include <sk/Scene/Components/MeshComponent.h>
#include <sk/Scene/Components/SpinComponent.h>
#include <sk/Scene/Managers/SceneManager.h>
#include <sk/Scene/Objects/CameraFlight.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "sk/Graphics/Rendering/Frame_Buffer.h"


#ifdef SKAPE_EDITOR_AVAILABLE

using namespace sk::Editor;

cEditor::cEditor()
{
    Input::setLogInputs( false );

    cAsset_Manager::init();

    m_main_window_ = Platform::CreateWindow( "Main Window", { 1280, 720 } );

    Graphics::InitRenderer();
    Graphics::Utils::InitUtils();
    m_main_window_->Init();

    SK_ERR_IFN( m_main_window_->SetVisibility( true ),
        "Unable to show window." )

    // TODO: Create a render context.


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    Gui::InitImGui( m_main_window_ );

    cSceneManager::init();
}

cEditor::~cEditor()
{
    cSceneManager::shutdown();

    Gui::ImGuiShutdown();
    ImGui::DestroyContext();

    SK_DELETE( m_main_window_ );

    cAsset_Manager::shutdown();

    // I don't really like the choice of having the renderer shut down after the asset system,
    // with the asset system being started before the renderer.
    // TODO: Add some RegisterRendererListeners function or something in the future.
    Graphics::Utils::ShutdownUtils();
    Graphics::cRenderer::shutdown();
    cStringIDManager::shutdown();
}

bool cEditor::IsGameRunning() const
{
    return m_is_game_running_.load();
}

void cEditor::Create()
{
    auto& asset_m = cAsset_Manager::get();

	auto list_1 = asset_m.loadFile( "models/humanforscale.glb" );
	auto list_2 = asset_m.loadFile( "models/heheToiletwithtextures.glb" );
	const auto shader_frag = *asset_m.loadFile( "shaders/gpass.frag" ).begin();
	const auto shader_vert = *asset_m.loadFile( "shaders/default.vert" ).begin();
	asset_m.loadFile( "shaders/screen.vert" );
	asset_m.loadFile( "shaders/deferred.frag" );

	// TODO: Create a material instance class.

	Graphics::cRenderer::get().SetPipeline( SK_SINGLE( sk::Graphics::cDeferred_Pipeline ) );

	// Testing Scene
	const auto christopher_t = list_1.GetAssetOfType< Assets::cTexture >();
	auto christopher_m       = list_1.GetAssetOfType< Assets::cMesh    >();
	const auto toilet_t      = list_2.GetAssetOfType< Assets::cTexture >();
	auto toilet_m            = list_2.GetAssetOfType< Assets::cMesh    >();

	auto scene = sk::make_shared< cScene >();
	scene->create_object< Object::cCameraFlight >( "Camera Free Flight" )->setAsMain();

	auto mat1 = asset_m.CreateAsset< Assets::cMaterial >( "Material Test",
		Graphics::Utils::cShader_Link{ shader_vert, shader_frag } );
	mat1.second->SetTexture( "mainTexture", christopher_t );

	auto mesh = scene->create_object< Object::iObject >( "Mesh Test 2" );
	mesh->GetTransform().SetLocalPosition( { 0.0f, -16.0f, 90.0f } );
	auto component = mesh->AddComponent< Object::Components::cMeshComponent >( christopher_m, mat1.first );
	component->enabled();
	component->SetScale( cVector3f{ 100.0f } );

	auto mat2 = asset_m.CreateAsset< Assets::cMaterial >( "Material Test 2",
		Graphics::Utils::cShader_Link{ shader_vert, shader_frag } );
	mat2.second->SetTexture( "mainTexture", toilet_t );

	mesh = scene->create_object< Object::iObject >( "Mesh Test 3" );
	mesh->GetTransform().SetLocalPosition( { -0.1f, 0.0f, 2.5f } );
	auto spin_component = mesh->AddComponent< Object::Components::cSpinComponent >( cVector3f{ 0.0f, 5.0f, 0.0f } );
	component = mesh->AddComponent< Object::Components::cMeshComponent >( toilet_m, mat2.first );
	component->enabled();
	component->SetParent( spin_component );

	Scene::Light::sSettings light_settings{};
	auto light_object = scene->create_object< Object::iObject >( "Directional Light" );
	light_settings.shadow_resolution = 1024;
	light_settings.casts_shadows     = true;
	auto directional_light = light_object->AddComponent< Object::Components::cLightComponent >( light_settings );
	light_object->GetTransform().SetLocalRotation( { -45.0f, 0.0f, 0.0f } );

	light_object = scene->create_object< Object::iObject >( "Point Light" );
	light_settings.type              = Scene::Light::eType::kPoint;
	light_settings.casts_shadows     = false;
	light_settings.shadow_resolution = 256;
	auto point_light = light_object->AddComponent< Object::Components::cLightComponent >( light_settings );
	light_object->GetTransform().SetLocalRotation( { -45.0f, 0.0f, 0.0f } );

	light_object = scene->create_object< Object::iObject >( "Spot Light 1" );
	light_settings.type              = Scene::Light::eType::kSpot;
	light_settings.casts_shadows     = true;
	light_settings.shadow_resolution = 512;
	auto spot_light = light_object->AddComponent< Object::Components::cLightComponent >( light_settings );
	light_object->GetTransform().SetLocalRotation( { -45.0f, 0.0f, 0.0f } );
	light_object->GetTransform().SetLocalPosition( { 0.0f, 5.0f, 0.0f } );

	light_object = scene->create_object< Object::iObject >( "Spot Light 2" );
	light_settings.type              = Scene::Light::eType::kSpot;
	light_settings.casts_shadows     = true;
	light_settings.shadow_resolution = 512;
	spot_light = light_object->AddComponent< Object::Components::cLightComponent >( light_settings );
	light_object->GetTransform().SetLocalRotation( { -45.0f, 90.0f, 0.0f } );
	light_object->GetTransform().SetLocalPosition( { 0.0f, 5.0f, 0.0f } );

	light_object = scene->create_object< Object::iObject >( "Spot Light 3" );
	light_settings.type              = Scene::Light::eType::kSpot;
	light_settings.casts_shadows     = true;
	light_settings.shadow_resolution = 512;
	spot_light = light_object->AddComponent< Object::Components::cLightComponent >( light_settings );
	light_object->GetTransform().SetLocalRotation( { -45.0f, 180.0f, 0.0f } );
	light_object->GetTransform().SetLocalPosition( { 0.0f, 5.0f, 0.0f } );

	// Fuck it we ball
	constexpr auto kGridWidth = 5;
	std::random_device rd;
	std::mt19937 gen( rd() );
	std::uniform_real_distribution dis( -25.0f, 25.0f );
	for( int64_t x = -kGridWidth; x < kGridWidth; ++x )
	{
		for( int64_t y = -kGridWidth; y < kGridWidth; ++y )
		{
			auto mesh_object = scene->create_object< Object::iObject >( "Clone" );
			mesh_object->GetTransform().SetLocalPosition( { x * 2, 0.0f, y * 2 } );
			auto spin = mesh_object->AddComponent< Object::Components::cSpinComponent >( cVector3f{ 0.0f, dis( gen ), 0.0f } );
			auto mesh_component = mesh_object->AddComponent< Object::Components::cMeshComponent >( christopher_m, mat1.first );
			mesh_component->enabled();
			mesh_component->SetRotation( { -90.0f, 0.0f, 0.0f } );
			mesh_component->SetParent( spin );
		}
	}

	cSceneManager::get().registerScene( scene );

	// Editor Scene

	auto editor_scene = sk::make_shared< cScene >();
	m_camera_ = scene->create_object< Object::cCameraFlight >( "Editor Viewport" );
	m_camera_->setAsMain();
	m_camera_->setFilter( m_camera_->getFilter() | Input::kEditor );
	cSceneManager::get().registerScene( editor_scene );
}

void cEditor::Run()
{
    m_main_window_->UseContext();

    Gui::ImGuiNewFrame();

    _drawMainWindow();

    // Time::Update();

    // cSceneManager::get().update();
    Graphics::cRenderer::get().Update();
    // auto& pipeline = *Graphics::cRenderer::get().GetPipeline();
    // pipeline.Execute();

    Gui::ImGuiRender();
    m_main_window_->SwapBuffers();

    Gui::ImGuiUpdateWindows();
}

void cEditor::Destroy()
{

}

void cEditor::_drawMainWindow()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    static ImGuiID main_viewport_id = ImGui::GetMainViewport()->ID;

    const ImGuiViewport* viewport = ImGui::FindViewportByID( main_viewport_id );
    ImGui::SetNextWindowPos( viewport->WorkPos );
    ImGui::SetNextWindowSize( viewport->WorkSize );
    ImGui::SetNextWindowViewport( main_viewport_id );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    bool open = true;
    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f } );
    ImGui::Begin("Test", &open, window_flags );
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    auto dockspace_id = ImGui::GetID( "RootDockspace" );

    if( ImGui::DockBuilderGetNode( dockspace_id ) == nullptr )
    {
        ImGui::DockBuilderRemoveNode( dockspace_id );
        ImGui::DockBuilderAddNode( dockspace_id );

        ImGui::DockBuilderFinish( dockspace_id );
    }

    ImGui::DockSpace( dockspace_id );
    ImGui::End();

    if( ImGui::Begin( "Test 1" ) )
    {
        ImGui::Text("Test");
    }
    ImGui::End();

    if( ImGui::Begin( "Test 2" ) )
    {
        ImGui::Text("Test");
    }
    ImGui::End();

    if( ImGui::Begin( "Test 3" ) )
    {
        ImGui::Text("Test");
    }
    ImGui::End();

    if( ImGui::Begin( "Viewport" ) )
    {
    	const auto  region = ImGui::GetContentRegionAvail();
    	const auto& front = Graphics::cRenderer::get().GetPipeline()->GetFallbackContext()->GetFront();

    	ImGui::Image( front.GetRenderTarget( 0 )->get_texture_object(), region );
    }
    ImGui::End();

}

#endif // SKAPE_EDITOR_AVAILABLE
