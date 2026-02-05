//
// Created by willness on 2026-02-05.
// Copyright (c) 2026 William Ask S. Ness. All rights reserved.
//


#pragma once

#include <functional>
#include <string_view>
#include <variant>
#include <vector>

namespace sk::Editor::Utils
{
    class cContextMenu
    {
    public:
        using callback_t = std::function< void( void* ) >;
        using value_t    = std::variant< std::monostate, bool*, callback_t, cContextMenu >;


        void Draw( const std::string_view& _name = {} );

        // Adds a text item to the menu
        auto Add( const std::string& _text ) -> cContextMenu&;
        // Do note, the bool value has to survive as long as this context.
        // Adds a checkbox item to the menu
        auto Add( const std::string& _name, bool& _bool_value ) -> cContextMenu&;
        // Adds a button item to the menu
        auto Add( const std::string& _name, const callback_t& _callback ) -> cContextMenu&;

        /**
         *
         * @param _name The name of the submenu
         * @return The submenu to fill with content
         */
        auto AddMenu( const std::string& _name ) -> cContextMenu;
        /**
         *
         * @return A reference to the parent menu
         */
        auto Submit() -> cContextMenu&;

        void SetUserData( void* _data );

    private:
        void _drawItems();

        cContextMenu* m_parent_menu_ = nullptr;
        void*         m_user_data_   = nullptr;
        std::vector< std::pair< std::string, value_t > > m_items_;
    };
} // sk::Editor::Utils::