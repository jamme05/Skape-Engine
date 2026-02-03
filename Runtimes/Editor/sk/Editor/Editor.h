
#pragma once

#include <sk/Misc/Singleton.h>

#include <atomic>

namespace sk::Editor
{
    class cEditor : public cSingleton< cEditor >
    {
    public:
        cEditor();
        ~cEditor() override;

        bool IsGameRunning() const;

        void Create();
        void Run();
        void Destroy();

    private:
        std::atomic_bool m_is_game_running_ = false;
    };
} // sk::Editor::
