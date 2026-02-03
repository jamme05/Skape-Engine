/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include <framework/App.h>

#include <sk/Skape_Main.h>
#include <sk/Editor/Editor.h>
#include <sk/Memory/Tracker/Tracker.h>
#include <sk/Platform/Time.h>

namespace sk::Runtime
{
    void startup( int _argc, char** _argv )
    {
        // TODO: Find asset dir from args. Maybe with prefix -P "path/to/project" or --project "path/to/project"
        Reflection::cType_Manager::init();
        Memory::Tracker::init();
        Time::init();
        auto& editor = Editor::cEditor::init();

        editor.Create();
    }

    bool run()
    {
        // TODO: Add a way to check if the editor is running.
        // Currently not necessary as we're using SDL3 and not sk_main.
        Editor::cEditor::get().Run();
        return true;
    }

    void shutdown()
    {
        Editor::cEditor::shutdown();
        Memory::Tracker::shutdown();
        Reflection::cType_Manager::shutdown();
    }

    /**
     * This is the normal startup/shutdown code for
     * @return Status code
     */
    int sk_main( const int _argc, char** _args )
    {
        startup( _argc, _args );

        // Alternatively
        // while( app.running() ) app.run()

        bool running = true;
        while( running )
            running = run();

        shutdown();

        return 0;
    }
} // sk::Runtime::

// Use normal main if no custom main provided.
#if !defined( SK_CUSTOM_MAIN )

int main( const int _argc, char** _args )
{
    return sk::Runtime::sk_main( _argc, _args );
}

#endif // !SK_CUSTOM_MAIN