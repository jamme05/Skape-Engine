/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <unordered_map>
#include <filesystem>

#include "Memory/Tracker/Tracker.h"
#include "Reflection/RuntimeClass.h"
#include "Misc/Hashing.h"
#include "Misc/Smart_Ptrs.h"
#include "Misc/StringID.h"
#include "Misc/UUID.h"

namespace sk
{
	class cAsset_Manager;

	// Remember to update macro if changing this.
	constexpr auto kInvalid_Asset_Id = std::numeric_limits< uint64_t >::max();

	// Base Asset interface
	SK_CLASS( PartialAsset )
	{
		SK_CLASS_BODY( PartialAsset )
	public:
		
		explicit cPartialAsset( std::string _name )
		: m_name_( std::move( _name ) )
		{ }
		~cPartialAsset() override = default;

		auto& GetName() const { return m_name_; }
		auto& GetPath() const { return m_path_; }

		// Saves any changes made to the asset.
		virtual void Save  () = 0;
		// Loads and prepares the asset for use.
		virtual void Load  () = 0;
		// Unloads the asset.
		virtual void Unload() = 0;

	private:
		cUUID m_uuid_ = cUUID::kInvalid;

		void setPath( const std::filesystem::path& _path ){ m_path_ = _path.string(); }

	protected:
		cStringID m_name_ = {};
		cStringID m_path_ = {};

	friend class cAsset_Manager;
	};

	namespace PartialAsset
	{
		using class_type = cPartialAsset;
	} // Asset
	
	SK_CLASS( Asset ), public cShared_from_this< cAsset >
	{
		SK_CLASS_BODY( Asset )
	public:
		auto& GetAsset() const
		{
			return m_asset_;
		}
	protected:
		cAsset() = default;
	private:
		cWeak_Ptr< cPartialAsset > m_asset_;
	};

	template< class In, class Out >
	using enable_if_asset_t = std::enable_if_t< std::is_base_of_v< cAsset, In >, Out >;
	template< class Ty >
	static constexpr bool is_valid_asset_v = std::is_base_of_v< cAsset, Ty >;

} // sk::

DECLARE_CLASS( sk::PartialAsset )
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
