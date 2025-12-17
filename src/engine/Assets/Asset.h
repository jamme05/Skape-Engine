/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <unordered_map>
#include <filesystem>

#include <Memory/Tracker/Tracker.h>
#include <Reflection/RuntimeClass.h>
#include <Misc/Hashing.h>
#include <Misc/Smart_Ptrs.h>
#include <Misc/StringID.h>
#include <Misc/UUID.h>

#include <Scene/Managers/EventManager.h>

namespace sk
{
	class cAsset_Manager;
	class cAsset_Worker;
	class cAsset;

	// Remember to update macro if changing this.
	constexpr auto kInvalid_Asset_Id = std::numeric_limits< uint64_t >::max();

	// Base Asset interface
	SK_CLASS( Asset_Meta ), public cShared_from_this< cAsset_Meta >
	{
		SK_CLASS_BODY( Asset_Meta )

		friend class cAsset_Manager;
		friend class cAsset_Worker;
		friend class cAsset_Ref;
	public:
		enum class eEventType : uint8_t
		{
			kUnload,
			kLoaded,
			kUpdated
		};

		enum eFlags : uint8_t
		{
			kNone     = 0,
			
			// If the asset is loaded.
			kLoaded   = 1 << 0,
			// If the asset is currently loading
			kLoading  = 1 << 1,
			// If the asset has a metadata file associated with it.
			kMetadata = 1 << 2,
		};

		using dispatcher_t = Event::cEventDispatcher< cAsset_Meta&, void*, eEventType >;
		
		explicit cAsset_Meta( const std::string_view _name )
		: m_name_{ _name }
		{ }
		~cAsset_Meta() override = default;

		auto& GetName() const { return m_name_; }
		auto& GetPath() const { return m_path_; }
		
		// Saves any changes made to the asset.
		void Save  ();
		// Gets weather or not the asset is currently loading
		bool IsLoading() const;
		// Gets weather or not the asset is loaded
		bool IsLoaded() const;
		// Gets weather or not the asset has metadata
		bool HasMetadata() const;
		
		// Gets the asset
		auto GetAsset() const -> cAsset*;
		// Gets the asset if it is of a certain type
		template< reflected Ty >
		auto GetAsset() -> Ty*;
		// Gets the flags this asset has. Check eFlags for details.
		auto GetFlags() const { return m_flags_; }

		// Adds a listener to get events on when the asset changes.
		// NOTE: Listeners do not load or unload the asset, and are only used to receive information.
		size_t AddListener( const dispatcher_t::event_t& _listener );

		// Adds a listener to get events on when the asset changes.
		// NOTE: Listeners do not load or unload the asset, and are only used to receive information.
		size_t AddListener( const dispatcher_t::weak_event_t& _listener );

		// Removes a listener from getting events on when the asset changes.
		// NOTE: Listeners do not load or unload the asset, and are only used to receive information.
		void RemoveListener( const size_t _listener_id );

		// Removes a listener from getting events on when the asset changes.
		// NOTE: Listeners do not load or unload the asset, and are only used to receive information.
		void RemoveListener( const dispatcher_t::event_t& _listener );

		// Removes a listener from getting events on when the asset changes.
		// NOTE: Listeners do not load or unload the asset, and are only used to receive information.
		void RemoveListener( const dispatcher_t::weak_event_t& _listener );

	private:
		// Requests a asset loader to post a asset loaded event to the specified listener.
		void dispatch_if_loaded( const dispatcher_t::listener_t& _listener );

		cUUID     m_uuid_ = cUUID::kInvalid;
		cStringID m_name_ = {};
		cStringID m_path_ = {};
		
		cAsset*   m_asset_ = nullptr;

		std::atomic_uint32_t m_asset_refs_;
		uint32_t             m_flags_ = 0;

		std::mutex   m_dispatcher_mutex_;
		dispatcher_t m_dispatcher_;

		void setPath( const std::filesystem::path& _path );
	};

	namespace Asset_Meta
	{
		using class_type = cAsset_Meta;
	} // Asset
	
	SK_CLASS( Asset ), public cShared_from_this< cAsset >
	{
		SK_CLASS_BODY( Asset )
	public:
		auto& GetMeta() const
		{
			return m_asset_;
		}
	protected:
		cAsset() = default;
	private:
		cWeak_Ptr< cAsset_Meta > m_asset_;
	};

	template< class In, class Out >
	using enable_if_asset_t = std::enable_if_t< std::is_base_of_v< cAsset, In >, Out >;
	template< class Ty >
	static constexpr bool is_valid_asset_v = std::is_base_of_v< cAsset, Ty >;

	template< reflected Ty >
	auto cAsset_Meta::GetAsset() -> Ty*
	{
		if( m_asset_->getClass().isDerivedFrom( Ty::getStaticClass() ) )
			return m_asset_;

		return nullptr;
	}

} // sk::

DECLARE_CLASS( sk::Asset_Meta )
DECLARE_CLASS( sk::Asset )

// Initializes data required for cAsset.
// The class created by this will be using the default naming (ex: Mesh becomes cMesh).
// Raw values can be accessed in sk::Assets::[Name], (ex sk::Assets::Mesh)
#define ASSET_PARENT_CLASS( AssetName, ... ) sk::cAsset
#define ASSET_PARENT_VALIDATOR( AssetName, ... ) sk::is_valid_asset_v< __VA_ARGS__ >
#define ASSET_PARENT_CREATOR_2( AssetName, ... ) AFTER_FIRST( __VA_ARGS__ )
#define ASSET_PARENT_CREATOR_1( AssetName, ... ) sk::cAsset
#define ASSET_PARENT_CREATOR( AssetName, ... ) CONCAT( ASSET_PARENT_CREATOR_, VARGS( __VA_ARGS__ ) ) ( AssetName, __VA_ARGS__ )
#define SK_ASSET_CLASS( AssetName, ... ) QW_RESTRICTED_CLASS( AssetName, ASSET_PARENT_CLASS, ASSET_PARENT_CREATOR, ASSET_PARENT_VALIDATOR, EMPTY __VA_OPT__( , __VA_ARGS__ ) )
