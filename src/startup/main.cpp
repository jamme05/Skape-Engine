/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "App.h"

#include <Memory/Tracker/Tracker.h>

namespace sk
{
	// NOTE: This will always fail in editor, but SHOULD be valid when compiling.
	static_assert( registry::type_registry< 0 >::valid, "No types registered." );
	constexpr static auto types = registry::type_registry< unique_id() - 1 >::registered;
	const unordered_map< type_hash, const sType_Info* > type_map = { types.begin(), types.end() };
} // sk::

int main( int, char** )
{
	sk::Memory::cTracker::initialize();
	auto& app = cApp::initialize();
	app.create();

	while( app.running() )
		app.run();

	app.destroy();

	cApp::deinitialize();

	eExample1 test = eExample1::kBothEx;
	test.getDisplayName()
	
	sk::Memory::cTracker::deinitialize();

	return 0;
}
