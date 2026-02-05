
#include "ObjectListTab.h"

#include <sk/Editor/Managers/SelectionManager.h>
#include <sk/Editor/Utils/ContextMenu.h>
#include <sk/Scene/Scene.h>
#include <sk/Scene/Managers/SceneManager.h>

#include <imgui.h>



using namespace sk::Editor::Tabs;

void cObjectListTab::Create()
{
    m_context_menu_
    .AddMenu( "Settings" )
        .Add( "Debug Mode", m_debug_view_ )
        .Add( "Show Components", m_show_components_ )
    .Submit();

    m_scene_context_menu_
    .Add( "Load", []( void* _meta )
    {
        const auto& meta = *static_cast< cAsset_Meta* >( _meta );
        cSceneManager::get().LoadScene( meta.GetUUID() );
    } )
    .Add( "Unload", []( void* _meta )
    {
        const auto& meta = *static_cast< cAsset_Meta* >( _meta );
        cSceneManager::get().UnloadScene( meta.GetUUID() );
    } )
    .Add( "Remove", []( void* _meta )
    {
        const auto& meta = *static_cast< cAsset_Meta* >( _meta );
        cSceneManager::get().UnregisterScene( meta.GetUUID() );
    } );
}

void cObjectListTab::Draw()
{
    const auto& manager = cSceneManager::get();

    m_context_menu_.Draw();

    for( auto scenes = manager.GetScenes();
        auto& scene_meta : scenes | std::views::values )
    {
        if( scene_meta->IsLoaded() )
            _drawScene( static_cast< cScene& >( *scene_meta->GetAsset() ) ); // NOLINT(*-pro-type-static-cast-downcast)
        else
        {
            ImGui::CollapsingHeader( scene_meta->GetName().c_str(), ImGuiTreeNodeFlags_Leaf );
            m_scene_context_menu_.SetUserData( scene_meta.get() );
            m_scene_context_menu_.Draw();
        }
    }
}

void cObjectListTab::Destroy()
{

}

void cObjectListTab::_drawScene( const cScene& _scene )
{
    auto& meta = *_scene.GetMeta();

    if( ImGui::CollapsingHeader( meta.GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
    {
        m_scene_context_menu_.SetUserData( &meta );
        m_scene_context_menu_.Draw();

        for( auto& object : _scene.GetObjects() )
            _drawObjectRecursive( *object );
    }
}

void cObjectListTab::_drawObjectRecursive( const Object::iObject& _object )
{
    auto& selection_manager = Managers::cSelectionManager::get();

    auto& children = _object.GetChildren();
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
    flags |= ImGuiTreeNodeFlags_DefaultOpen;

    if( children.empty() && !m_show_components_ )
        flags |= ImGuiTreeNodeFlags_Leaf;


    if( selection_manager.IsSelected( _object ) )
        flags |= ImGuiTreeNodeFlags_Selected;

    if( !ImGui::TreeNodeEx( _object.GetUUID().to_string().c_str(), flags, "%s", _object.GetName().c_str() ) )
        return;

    if( ImGui::IsItemClicked() )
    {
        if( ImGui::IsKeyDown( ImGuiMod_Ctrl ) )
            Managers::cSelectionManager::get().ToggleSelectedObject( _object.get_shared() );
        else
            Managers::cSelectionManager::get().AddSelectedObject( _object.get_shared(), !ImGui::IsKeyDown( ImGuiMod_Shift ) );
    }

    for( auto& object : children )
        _drawObjectRecursive( *object );

    // TODO: Draw this node slightly differently to show that this is showing something else.
    if( m_show_components_ && ImGui::TreeNode( ( "C_" + _object.GetUUID().to_string() ).c_str(), "Components" ) )
    {
        _drawComponentsRecursive( *_object.GetRoot() );

        ImGui::TreePop();
    }

    ImGui::TreePop();
}

void cObjectListTab::_drawComponentsRecursive( const Object::iComponent& _component )
{
    // TODO: Allow the user to toggle the visual
    if( !m_debug_view_ && _component.GetIsInternal() )
        return;

    auto& selection_manager = Managers::cSelectionManager::get();

    const auto type_name = _component.getClass().getName();

    auto& children = _component.GetChildren();
    const auto has_non_internal_child = std::ranges::any_of( children, []( const auto& _child ){ return !_child->GetIsInternal(); } );

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
    flags |= has_non_internal_child ? ImGuiTreeNodeFlags_None : ImGuiTreeNodeFlags_Leaf;
    if( selection_manager.IsSelected( _component ) )
        flags |= ImGuiTreeNodeFlags_Selected;

    if( !ImGui::TreeNodeEx( _component.GetUUID().to_string().c_str(), flags, "%s", type_name.c_str() ) )
        return;

    if( ImGui::IsItemClicked() )
    {
        if( ImGui::IsKeyDown( ImGuiMod_Ctrl ) )
            Managers::cSelectionManager::get().ToggleSelectedComponent( _component.get_shared() );
        else
            Managers::cSelectionManager::get().AddSelectedComponent( _component.get_shared(), !ImGui::IsKeyDown( ImGuiMod_Shift ) );
    }

    for( auto& child : children )
        _drawComponentsRecursive( *child );

    ImGui::TreePop();
}
