//
// Created by willness on 2026-02-06.
// Copyright (c) 2026 William Ask S. Ness. All rights reserved.
//


#include "AssetViewTab.h"

#include <sk/Assets/Management/Asset_Manager.h>

#include <imgui.h>

using namespace sk::Editor::Tabs;

namespace
{
    struct sAsset
    {
        bool _is_folder;
        sk::cAsset_Meta* meta;
    };
} // ::

void cAssetViewTab::Create()
{
    m_current_folder_ = std::filesystem::current_path();
    m_context_menu_
        .If( [ current_folder=&m_current_folder_ ]( void* )
        {
            return *current_folder == std::filesystem::current_path();
        } )
            .SetDisabled( true )
        .EndIf()
        .Add( "Back", [ current_folder=&m_current_folder_ ]( void* )
        {
            if( *current_folder != std::filesystem::current_path() )
                *current_folder = current_folder->parent_path();
        } )
    .Complete();
}

void cAssetViewTab::Draw()
{
    struct sFolder
    {
        std::string           name;
        std::filesystem::path path;
    };

    m_context_menu_.Draw();

    // TODO: Redo all of this
    auto& assets = cAsset_Manager::get().GetAllAssets();
    std::vector< cShared_ptr< cAsset_Meta > > assets_in_folder;
    std::vector< sFolder > folders_in_folder;

    for( auto& entry : std::filesystem::directory_iterator{ m_current_folder_ } )
    {
        if( entry.is_directory() )
            folders_in_folder.emplace_back( sFolder{ entry.path().filename().string(), entry.path() } );
    }
    for( const auto& asset : assets | std::views::values )
    {
        if( std::filesystem::path{ asset->GetPath().view() }.parent_path() == m_current_folder_ )
            assets_in_folder.emplace_back( asset );
    }

    const auto region = ImGui::GetContentRegionAvail();

    const auto width = region.x / 6;
    auto size  = ImVec2{ width - 5, width - 5 };

    // auto total_items = assets_in_folder.size() + folders_in_folder.size();

    ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 5 } );

    size_t index = 0;
    for( auto& [ name, path ] : folders_in_folder )
    {
        if( ( index++ % 6 ) != 0 )
            ImGui::SameLine();

        if( ImGui::Button( name.c_str(), size ) )
            m_current_folder_ = path;
    }

    for( auto& asset : assets_in_folder )
    {
        if( ( index++ % 6 ) != 0 )
            ImGui::SameLine();

        if( ImGui::Button( std::format( "{}##Asset_{}", asset->GetName().view(), index ).c_str(), size ) )
        {
            println( "Asset {} selected", asset->GetName().c_str() );
        }
    }

    ImGui::PopStyleVar();
}

void cAssetViewTab::Destroy()
{

}
