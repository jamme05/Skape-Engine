/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Scene.h"

#include <sk/Seralization/SerializedObject.h>

namespace sk
{
	cScene::~cScene( void )
	{
		m_objects.clear();
	} // ~cScene

	void cScene::force_render( void )
	{
		// TODO: Get rid of this

		for( auto& obj : m_objects )
		{
			obj->render();
		}
	} // render

	void cScene::force_update( void )
	{
		for( auto& obj : m_objects )
			obj->update();
	} // update
	cShared_ptr< cSerializedObject > cScene::Serialize()
	{
		auto object = cSerializedObject::CreateForWrite( this );
		object->AddBase( cAsset::Serialize() );

		object->EndWrite();
		return object;
	}
} // sk::
