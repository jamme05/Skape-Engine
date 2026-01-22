

#pragma once

#include <sk/Graphics/Passes/Render_Pass.h>

#include <sk/Assets/Material.h>
#include <sk/Assets/Access/Asset_Ref.h>
#include <sk/Graphics/Buffer/Dynamic_Buffer.h>

namespace sk
{
    class cAsset_Meta;
} // sk::

namespace sk::Platform
{
    class iWindow;
} // sk::Platform::

namespace sk::Graphics::Passes
{
    class cScreen_Pass : public iPass
    {
    public:
        explicit cScreen_Pass( Platform::iWindow* _window, const cShared_ptr< cAsset_Meta >& _screen_material );
        
        void Init   () override;
        bool Begin  () override;
        void End    () override;
        void Destroy() override;
        
    private:
        cDynamic_Buffer    m_screen_vertex_buffer_;
        Platform::iWindow* m_window_;
        
        cWeak_Ptr< cAsset_Meta >        m_material_meta_;
        cAsset_Ref< Assets::cMaterial > m_material_;
    };
} // sk::Graphics::Passes::
