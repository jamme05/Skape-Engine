
#pragma once
#include "Tab.h"
#include "sk/Scene/Object.h"

namespace sk
{
    namespace Object
    {
        class iObject;
    } // sk::Object::

    class cScene;
} // sk::

namespace sk::Editor::Tabs
{
    class cObjectListTab : public aTab
    {
    public:
        explicit cObjectListTab( const std::string& _name ) : aTab( _name ){}

        void Create () override;
        void Draw   () override;
        void Destroy() override;

    private:
        static void _drawScene( const cScene& _scene );
        static void _drawObjectRecursive( const Object::iObject& _object );
        static void _drawComponentsRecursive( const Object::iComponent& _component );
    };
} // sk::Editor::Tabs::

