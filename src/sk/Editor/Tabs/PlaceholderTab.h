
#pragma once

#include "Tab.h"

namespace sk::Editor::Tabs
{
    class cPlaceholderTab : public aTab
    {
    public:
        explicit cPlaceholderTab( const std::string& _name ) : aTab( _name ){}

        void Create () override {}
        void Draw   () override;
        void Destroy() override {}
    };
} // sk::Editor::Tabs
