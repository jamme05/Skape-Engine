//
// Created by willness on 2026-02-06.
// Copyright (c) 2026 William Ask S. Ness. All rights reserved.
//


#pragma once

#include <sk/Editor/Tabs/Tab.h>
#include <sk/Editor/Utils/ContextMenu.h>

#include <filesystem>

namespace sk::Editor::Tabs
{
    class cAssetViewTab : public aTab
    {
    public:
        explicit cAssetViewTab( const std::string& _name ) : aTab( _name ){}

        void Create () override;
        void Draw   () override;
        void Destroy() override;

    private:
        Utils::cContextMenu   m_context_menu_;
        Utils::cContextMenu   m_asset_context_menu_;
        std::filesystem::path m_current_folder_;
    };
} // sk::Editor::Tabs::