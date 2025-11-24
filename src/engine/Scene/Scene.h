/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Object.h"
#include "Assets/Asset.h"

namespace sk
{
	namespace Object::Components
	{
		class cCamera;
	} // Objects::Components

	SK_ASSET_CLASS( Scene )
	{
		SK_CLASS_BODY( Scene )
	public:
		cScene() = default;

		~cScene() override;

		// Creates an empty object, essentially an empty
		template< class Ty, class... Args >
		requires std::is_base_of_v< Object::iObject, Ty >
		cShared_ptr< Ty > create_object( const std::string& _name, Args... _args )
		{
			auto shared = sk::make_shared< Ty >( _name, _args... );
			m_objects.emplace_back( shared );
			return shared;
		} // create_object

		void force_render( void );
		void force_update( void );

	private:
		// TODO: Replace this with a map.
		vector< cShared_ptr< Object::iObject > > m_objects = {};
	};

} // sk::

DECLARE_CLASS( sk::Scene )