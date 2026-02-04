
#pragma once

#include <sk/Math/Vector2.h>

#include <string>

namespace sk::Editor
{
    class cEditor;
} // sk::Editor::

namespace sk::Editor::Tabs
{
    class aTab
    {
        friend class sk::Editor::cEditor;
    public:
        explicit aTab( std::string _name ) : m_name_( std::move( _name ) ) {}
        virtual ~aTab() = default;

        virtual void Create () = 0;
        virtual void Draw   () = 0;
        virtual void Destroy() = 0;

        auto& GetName() const { return m_name_; };

    private:
        std::string m_name_;
        std::string m_name_with_id_ = {};
        size_t      m_id_ = 0;
    };
} // sk::Editor::Tabs