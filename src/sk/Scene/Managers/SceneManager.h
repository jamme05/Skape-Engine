/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Assets/Asset.h>
#include <sk/Assets/Access/Asset_Ref.h>
#include <sk/Misc/Singleton.h>
#include <sk/Misc/Smart_Ptrs.h>
#include <sk/Misc/UUID.h>
#include <sk/Scene/Scene.h>

#include <queue>

namespace sk
{
	class cSceneManager : public cSingleton< cSceneManager >
	{
	public:
		struct sScene
		{
			cAsset_Ref< cScene > scene;
		};

		cSceneManager ();
		~cSceneManager() override;

		void  registerScene( const cShared_ptr< cAsset_Meta >& _scene_meta );
		auto& GetScenes() const { return m_scenes_; }

		void        update();
		static void render();

	private:
		static void on_scene_loaded( Assets::eEventType _action, cAsset_Ref< cScene >& _ref );

		using scene_map_t = unordered_map< hash< cUUID >, cAsset_Ref< cScene > >;

		std::mutex m_scene_mutex_;
		std::queue< cScene* > m_recently_loaded_scenes_ = {};
		scene_map_t           m_scenes_ = {};
	};
} // sk::
