/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */


#include "SceneManager.h"

#include <Graphics/Rendering/Frame_Buffer.h>
#include <Graphics/Rendering/Render_Context.h>
#include <Scene/Components/CameraComponent.h>
#include <Scene/Managers/CameraManager.h>
#include <Scene/Managers/Layer_Manager.h>
#include <Scene/Managers/Light_Manager.h>

namespace sk
{
	Graphics::Rendering::cRender_Context* cSceneManager::m_active_context = nullptr;
	cSceneManager::sObjectBuffer* cSceneManager::m_out_buffer = nullptr;

	cSceneManager::cSceneManager()
	{
		cEventManager::init();
		Scene::cCameraManager::init();
		Scene::cLayer_Manager::init();
		Scene::cLight_Manager::init();
	}
	cSceneManager::~cSceneManager()
	{
		m_scenes.clear();
		cEventManager::shutdown();
		Scene::cCameraManager::shutdown();
		Scene::cLayer_Manager::shutdown();
		Scene::cLight_Manager::shutdown();
	} // ~cSceneManager

	void cSceneManager::registerScene( const cShared_ptr< cScene >& _scene )
	{
		m_scenes.push_back( _scene );
	} // registerScene

	void cSceneManager::update()
	{
		cEventManager::get().postEvent( Object::kUpdate );
	} // update

	void cSceneManager::render()
	{
		// TODO: Add logic to prepare for render.
		Scene::cCameraManager::get().render();
		m_active_context = nullptr;
	} // render

	void cSceneManager::render_with( Scene::camera_t& _camera, Graphics::Rendering::cRender_Context& _context, const bool _swap )
	{
		// TODO: Have this use itself of the pipelines instead.
		m_active_context = &_context;
		_context.Begin( _camera.getViewport(), _camera.getScissor() );
		_context.Clear( Clear::kAll );
		
		// TODO: Use some spacial indexing for rendering.
		// Idea: Bounding cubes covering the entire world
		// The closer to the camera the more layers the bounding boxes can have.
		cEventManager::get().postEvent( Object::kRender );

#ifdef DEBUG
		cEventManager::get().postEvent( Object::kDebugRender );
#endif // DEBUG

		_context.End( _swap );
	} // render_with
} // sk::