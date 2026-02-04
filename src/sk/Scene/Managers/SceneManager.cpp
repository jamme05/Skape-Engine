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

void cSceneManager::registerScene( const cShared_ptr< cAsset_Meta >& _scene_meta )
{
	if( m_scenes_.contains( _scene_meta->GetUUID() ) )
	{
		SK_WARNING( sk::Severity::kEngine, "Warning: Scene \"{}\" is already registered", _scene_meta->GetName().view() )
		return;
	}

	auto& ref = m_scenes_[ _scene_meta->GetUUID() ];
	ref.on_changed += on_scene_loaded;
	ref = _scene_meta;
} // registerScene

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
