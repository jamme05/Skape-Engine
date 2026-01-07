/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "App.h"

#include <Memory/Tracker/Tracker.h>
#include <Skape_Main.h>

#include "Platform/Time.h"

namespace sk::App
{
	void startup( int, char** )
	{
		Reflection::cType_Manager::init();
		Memory::Tracker::init();
		Time::init();
		auto& app = cApp::init();

		app.create();
	}

	void shutdown()
	{
		cApp::shutdown();
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

		while( const auto app = cApp::getRunningInstance() )
			app->run();

		shutdown();

		return 0;
	}
} // ::

// Use normal main if no custom main provided.
#if !defined( SK_CUSTOM_MAIN )

int main( const int _argc, char** _args )
{
	return sk::App::sk_main( _argc, _args );
}

#endif // !SK_CUSTOM_MAIN
