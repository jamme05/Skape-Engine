/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Scene.h"

#include <sk/Seralization/SerializedObject.h>

namespace sk
{
	cScene::cScene( const cShared_ptr< cSerializedObject >& _object )
	: cAsset( _object->GetBase< cAsset >() )
	{
		_object->BeginRead( this );

		for( auto& obj : _object->ReadData< cShared_ptr< cSerializedObject > >( "objects" ).value()->GetArray< cShared_ptr< cSerializedObject > >() )
		{
			auto& object = m_objects.emplace_back( obj->ConstructSharedClass().Cast< Object::cObject >() );
			object->m_parent_scene = this;
		}

		_object->EndRead();
	}

	cScene::~cScene()
	{
		m_objects.clear();
	} // ~cScene

	void cScene::force_render()
	{
		// TODO: Get rid of this

		for( auto& obj : m_objects )
		{
			obj->render();
		}
	} // render

	void cScene::force_update()
	{
		for( auto& obj : m_objects )
			obj->update();
	} // update

	cShared_ptr< cSerializedObject > cScene::Serialize()
	{
		auto object = cSerializedObject::CreateForWrite( this );
		object->AddBase( cAsset::Serialize() );

		std::vector< cShared_ptr< cSerializedObject > > objects_vec;
		for( auto& obj : m_objects )
			objects_vec.emplace_back( obj->Serialize() );

		object->WriteData( "objects", cSerializedObject::CreateArray( objects_vec.data(), objects_vec.size() ) );

		object->EndWrite();
		return object;
	}
} // sk::
