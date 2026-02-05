/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */


#include "SceneManager.h"

#include <sk/Scene/Managers/CameraManager.h>
#include <sk/Scene/Managers/Internal_Component_Manager.h>
#include <sk/Scene/Managers/Layer_Manager.h>
#include <sk/Scene/Managers/Light_Manager.h>

using namespace sk;

cSceneManager::cSceneManager()
{
	cEventManager::init();
	Scene::cCameraManager::init();
	Scene::cLayer_Manager::init();
	Scene::cLight_Manager::init();
	Scene::cInternal_Component_Manager::init();
}
cSceneManager::~cSceneManager()
{
	m_scenes_.clear();

	Scene::cInternal_Component_Manager::shutdown();
	Scene::cLight_Manager::shutdown();
	Scene::cCameraManager::shutdown();
	Scene::cLayer_Manager::shutdown();
	cEventManager::shutdown();
} // ~cSceneManager

void cSceneManager::RegisterScene( const cShared_ptr< cAsset_Meta >& _scene_meta )
{
	if( m_scenes_.contains( _scene_meta->GetUUID() ) )
	{
		SK_WARNING( sk::Severity::kEngine, "Warning: Scene \"{}\" is already registered", _scene_meta->GetName().view() )
		return;
	}

	m_scenes_[ _scene_meta->GetUUID() ] = _scene_meta;
} // registerScene
void cSceneManager::UnregisterScene( const cUUID& _scene_uuid )
{
	// TODO: Warn if it wasn't found.
	if( m_scenes_.contains( _scene_uuid ) )
	{
		UnloadScene( _scene_uuid );
		m_scenes_.erase( _scene_uuid );
	}
}

void cSceneManager::LoadScene( const cUUID& _scene_uuid )
{
	if( m_loaded_scenes_.contains( _scene_uuid ) )
		return; // TODO: Throw warning

	auto& ref = m_loaded_scenes_[ _scene_uuid ] = cAsset_Ref< cScene >{};
	ref.on_changed += on_scene_loaded;
	// TODO: Warn if the scene isn't registered.
	ref = m_scenes_[ _scene_uuid ].Lock();
}

void cSceneManager::UnloadScene( const cUUID& _scene_uuid )
{
	m_loaded_scenes_.erase( _scene_uuid );
}

void cSceneManager::update()
{
	if( !m_recently_loaded_scenes_.empty() )
	{
		m_scene_mutex_.lock();
		const auto& scene = *m_recently_loaded_scenes_.back();
		m_recently_loaded_scenes_.pop();
		m_scene_mutex_.unlock();

		for( auto& object : scene.GetObjects() )
			object->registerRecursive();
	}
} // update

void cSceneManager::render()
{
	// TODO: Add logic to prepare for render.
	Scene::cCameraManager::get().render();
} // render

void cSceneManager::on_scene_loaded( Assets::eEventType _action, cAsset_Ref< cScene >& _ref )
{
	auto& self = get();
	self.m_scene_mutex_.lock();
	self.m_recently_loaded_scenes_.emplace( _ref.Get() );
	self.m_scene_mutex_.unlock();
}
