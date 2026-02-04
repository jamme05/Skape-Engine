
#pragma once

#ifdef SKAPE_EDITOR_AVAILABLE

// TODO: Move to another project and only link if the Editor is a runtime.

#include <sk/Graphics/Utils/RenderSurface.h>
#include <sk/Misc/Singleton.h>
#include <sk/Misc/Smart_Ptrs.h>
#include <sk/Platform/Window/Window_Base.h>

#include <atomic>




namespace sk::Object
{
    class cCameraFlight;
} // sk::Object::

namespace sk::Editor
{
    class cEditor : public cSingleton< cEditor >
    {
    public:
        cEditor();
        ~cEditor() override;

        bool IsGameRunning() const;

        void Create ();
        void Run    ();
        void Destroy();

    private:
        void _drawMainWindow();

        cShared_ptr< Object::cCameraFlight > m_camera_ = nullptr;
        std::atomic_bool   m_is_game_running_ = false;
        Platform::iWindow* m_main_window_;
        std::unique_ptr< Graphics::Utils::cRenderSurface > m_surface_;
    };
} // sk::Editor::

#endif // SKAPE_EDITOR_AVAILABLE