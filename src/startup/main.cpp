/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "App.h"

#include <Memory/Tracker/Tracker.h>
#include <Skape_Main.h>

namespace sk
{
	namespace
	{
		auto create_type_map( const cLinked_Array< type_info_t >& _array )
		{
			std::unordered_map< type_hash, const sType_Info* > map{ _array.size() };

			for( auto& val : _array )
				map.emplace( val->hash, val );

			return map;
		} // create_type_map

		// Why did this stop working?
		constexpr auto& types = registry::type_registry< registry::counter::next() - 1 >;
	} // ::
	// TODO: Add a handler for types.
	const auto type_map = create_type_map( types );
} // sk::

namespace sk::App
{
	void startup( int, char** )
	{
		Memory::Tracker::init();
		auto& app = cApp::init();

		app.create();
	}

	void shutdown()
	{
		cApp::shutdown();
		Memory::Tracker::shutdown();
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
