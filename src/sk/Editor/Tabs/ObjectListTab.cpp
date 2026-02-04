
#include "ObjectListTab.h"

#include <sk/Scene/Scene.h>
#include <sk/Scene/Managers/SceneManager.h>

#include <imgui.h>

using namespace sk::Editor::Tabs;

void cObjectListTab::Create()
{

}

void cObjectListTab::Draw()
{
    const auto& manager = cSceneManager::get();

    for( auto& scenes = manager.GetScenes(); auto& scene : scenes | std::views::values )
    {
        if( scene.IsLoaded() )
            _drawScene( *scene );
        else
            ImGui::CollapsingHeader( scene.GetMeta()->GetName().c_str(), ImGuiTreeNodeFlags_Leaf );
    }
}

void cObjectListTab::Destroy()
{

}

void cObjectListTab::_drawScene( const cScene& _scene )
{
    const auto& meta = *_scene.GetMeta();
    if( ImGui::CollapsingHeader( meta.GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
    {
        for( auto& object : _scene.GetObjects() )
            _drawObjectRecursive( *object );
    }
}

void cObjectListTab::_drawObjectRecursive( const Object::iObject& _object )
{
    auto& children = _object.GetChildren();
    if( !ImGui::TreeNode( _object.GetUUID().to_string().c_str(), "%s", _object.GetName().c_str() ) )
        return;

    for( auto& object : children )
        _drawObjectRecursive( *object );

    // TODO: Draw this node slightly differently to show that this is showing something else.
    if( ImGui::TreeNode( ( "C_" + _object.GetUUID().to_string() ).c_str(), "Components" ) )
    {
        _drawComponentsRecursive( *_object.GetRoot() );

        ImGui::TreePop();
    }

    ImGui::TreePop();
}

void cObjectListTab::_drawComponentsRecursive( const Object::iComponent& _component )
{
    const auto type_name = _component.getClass().getName();

    auto& children = _component.GetChildren();
    const auto flags = children.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None;

    if( !ImGui::TreeNodeEx( _component.GetUUID().to_string().c_str(), flags, "%s", type_name.c_str() ) )
        return;

    for( auto& child : children )
        _drawComponentsRecursive( *child );

    ImGui::TreePop();
}
