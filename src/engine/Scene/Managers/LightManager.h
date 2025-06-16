/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Misc/Singleton.h"

namespace sk
{
	class cLightManager : public cSingleton< cLightManager >
	{
	public:
		cLightManager();
		~cLightManager();

		uint64_t registerLight(  );
	};
}
