//
// Created by willness on 2026-02-05.
// Copyright (c) 2026 William Ask S. Ness. All rights reserved.
//


#include "ContextMenu.h"

#include <imgui.h>

using namespace sk::Editor::Utils;

namespace
{
    // From: https://en.cppreference.com/w/cpp/utility/variant/visit
    template< class... Ts >
    struct sVisitor : Ts... { using Ts::operator()...; };
} // ::

void cContextMenu::Draw( const std::string_view& _name )
{
    if( m_parent_menu_ == nullptr )
    {
        // Root menu/Popup
        if( ImGui::BeginPopupContextItem( _name.data() ) )
        {
            _drawItems();
            ImGui::EndPopup();
        }
    }
    else if( ImGui::BeginMenu( _name.data() ) )
    {
        // Sub menu
        _drawItems();
        ImGui::EndMenu();
    }
}

auto cContextMenu::Add( const std::string& _text ) -> cContextMenu&
{
    m_items_.emplace_back( _text, std::monostate{} );

    return *this;
}

auto cContextMenu::Add( const std::string& _name, bool& _bool_value ) -> cContextMenu&
{
    m_items_.emplace_back( _name, &_bool_value );

    return *this;
}

auto cContextMenu::Add( const std::string& _name, const callback_t& _callback ) -> cContextMenu&
{
    m_items_.emplace_back( _name, _callback );

    return *this;
}

auto cContextMenu::AddMenu( const std::string& _name ) -> cContextMenu
{
    m_items_.emplace_back( _name, std::monostate{} );

    cContextMenu temp_menu{};
    temp_menu.m_parent_menu_ = this;

    return temp_menu;
}

auto cContextMenu::Submit() -> cContextMenu&
{
    m_parent_menu_->m_items_.back().second = *this;

    return *m_parent_menu_;
}

void cContextMenu::SetUserData( void* _data )
{
    m_user_data_ = _data;
}

void cContextMenu::_drawItems()
{
    for( auto& [ name , value ] : m_items_ )
    {
        sVisitor visitor{
            [&]( std::monostate& )
            {
                ImGui::Text( "%s", name.c_str() );
            },
            [&]( bool*& _bool_value )
            {
                ImGui::Checkbox( name.c_str(), _bool_value );
            },
            [&]( const callback_t& _callback )
            {
                if( ImGui::MenuItem( name.c_str() ) )
                    _callback( m_user_data_ );
            },
            [&]( cContextMenu& _submenu )
            {
                _submenu.Draw( name );
            }
        };

        std::visit( visitor, value );
    }
}
