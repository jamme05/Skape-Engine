/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "App.h"

#include <Memory/Tracker/Tracker.h>

namespace sk
{
	static_assert( registry::type_registry< 0 >::valid, "No types registered." );
	constexpr static array types = registry::type_registry< sk::registry::counter::next() - 1 >::registered;
	const unordered_map< type_hash, const sType_Info* > type_map = { types.begin(), types.end() };
} // sk::

int main( int, char** )
{
	sk::Memory::Tracker::init();
	auto& app = cApp::initialize();
	app.create();

	while( app.running() )
		app.run();

	app.destroy();

	cApp::shutdown();
	
	sk::Memory::Tracker::shutdown();

	return 0;
}
