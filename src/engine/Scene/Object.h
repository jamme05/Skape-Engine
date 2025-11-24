/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Misc/Smart_Ptrs.h"

#include <ranges>
#include "Containers/Vector.h"
#include "Containers/Map.h"

#include "Components/Component.h"
#include "Components/TransformComponent.h"
#include <Reflection/RuntimeClass.h>

#include "Misc/StringID.h"

namespace sk::Object::Components
{
	class cMeshComponent;
}

namespace sk
{
	class cScene;
} // sk::

namespace sk::Object
{
// TODO: Check if class shit is needed? Like with Assets and Components.
	GENERATE_CLASS( iObject ), public Event::cEventListener, public cShared_from_this< iObject >
	{
		CREATE_CLASS_BODY( iObject )
	sk_public:
		// TODO: Create templated constructor with root type + parameters
		explicit iObject( const std::string& _name )
		: m_root( sk::make_shared< Components::cTransformComponent >() )
		, m_name( _name )
		{
			SetLayer( 0 );
		} // iObject

		template< class Ty = iComponent, class... Args >
		explicit iObject( const std::string& _name, Args... _args )
		: m_root( sk::make_shared< Ty >( _args... ) )
		, m_name( _name )
		{} // iObject

		~iObject() override
		{
			m_children_.clear();
			m_components_.clear();
			m_root = nullptr;
		}

		template< class Ty, class... Args >
		requires std::is_base_of_v< iComponent, Ty >
		auto AddComponent( Args&&... _args ) -> cShared_ptr< Ty >
		{
			auto component = sk::make_shared< Ty >( std::forward< Args >( _args )... );

			component->m_object = get_weak();
			component->SetParent( m_root );

			if constexpr( std::is_base_of_v< Components::cMeshComponent, Ty > )
				m_mesh_components_.emplace_back( component );
			else
				m_components_.insert( std::pair{ Ty::getStaticType(), component } );

			return component;
		} // addComponent

		template< class Ty, class... Args >
		requires std::is_base_of_v< iComponent, Ty >
		auto GetComponent( Args&&... _args ) -> cShared_ptr< Ty >
		{
			auto component = sk::make_shared< Ty >( std::forward< Args >( _args )... );

			component->m_object = get_weak();
			component->setParent( m_root );

			if constexpr( std::is_base_of_v< Components::cMeshComponent, Ty > )
				m_mesh_components_.emplace_back( component );
			else
				m_components_.insert( std::pair{ Ty::getStaticType(), component } );

			return component;
		} // addComponent
		
		// TODO: Add a way to remove the component

		// TODO: Reformat this comment.
		// Internal components are hidden by the editor and are mainly used to contain additional data on the object for systems.
		// You are only allowed to have a single internal component of a type at a time.
		// Due to being able to get the component without reconstructing it, it will be limited to using the default constructor.
		// Along with this you will have to verify that all values are valid.
		// ALSO, it isn't recommended to reuse the same internal component between multiple systems as it can cause collisions.
		// Boolean value says if it was created during this call or not.
		template< class Ty >
		requires ( std::is_base_of_v< iComponent, Ty > && std::is_default_constructible_v< Ty >)
		auto AddOrGetInternalComponent() -> std::pair< bool, cShared_ptr< Ty > >
		{
			if( const auto itr = m_internal_components_.find( Ty::getStaticType() ); itr != m_internal_components_.end() )
				return { false, itr->second };
			
			auto component = sk::make_shared< Ty >();

			component->m_object = get_weak();
			component->SetParent( m_root );
			component->m_internal = true;
			
			m_internal_components_.insert( std::pair{ Ty::getStaticType(), component } );

			return { true, component };
		} // addComponent

		// TODO: Deprecate?
		virtual void render()
		{
			// TODO: Add actual event vector or something.
			for( auto& val : m_components_ | std::views::values )
			{
				val->PostEvent( kRender );
				val->PostEvent( kDebugRender );
			}
		} // render

		virtual void update()
		{
			for( auto& val : m_components_ | std::views::values )
				val->PostEvent( kUpdate );
		} // update

		void SetLayer( uint64_t _layer );
		[[ nodiscard ]]
		auto  GetLayer() const { return m_layer_; }

		[[ nodiscard ]] auto& GetRoot()       { return m_root; }
		[[ nodiscard ]] auto& GetRoot() const { return m_root; }

		[[ nodiscard ]] auto& GetMeshComponents() const { return m_mesh_components_; }

		[[ nodiscard ]] auto& GetPosition()       { return m_root->GetPosition(); }
		[[ nodiscard ]] auto& GetPosition() const { return m_root->GetPosition(); }

		[[ nodiscard ]] auto& GetRotation()       { return m_root->GetRotation(); }
		[[ nodiscard ]] auto& GetRotation() const { return m_root->GetRotation(); }

		[[ nodiscard ]] auto& GetScale()       { return m_root->GetScale(); }
		[[ nodiscard ]] auto& GetScale() const { return m_root->GetScale(); }

		[[ nodiscard ]] auto& GetTransform()       { return m_root->GetTransform(); }
		[[ nodiscard ]] auto& GetTransform() const { return m_root->GetTransform(); }

		[[ nodiscard ]] auto& GetName() const { return m_name; }
		

	sk_protected:
		cShared_ptr< iComponent > m_root;

	sk_private:
		
		// TODO: Use typedefs/using
		vector             < cShared_ptr< iObject > >              m_children_   = { };
		unordered_multimap< type_hash, cShared_ptr< iComponent > > m_components_ = { };
		// Internal components are stored separately as there can only be one of a type.
		unordered_map< type_hash, cShared_ptr< iComponent >  >     m_internal_components_ = { };
		// We have the mesh components separately to allow for a faster lookup.
		vector< cShared_ptr< Components::cMeshComponent > >        m_mesh_components_;

		cStringID m_name;

		// TODO: Make this into a weak ptr.
		cScene* m_parent_scene = nullptr;

		// TODO: Actually implament these. Follow Unity's design when it comes to layers. But do allow multiple tags.
		std::vector< str_hash > m_tags_;
		uint64_t                m_layer_ = 1;

		friend class sk::cScene;
	};

	namespace Object
	{
		using class_type = iObject;
	}

	template< class Ty, class ClassTy, const ClassTy& ClassRef >
	class cObject : public iObject
	{
	public:
		explicit cObject( std::string _name ) : iObject( std::move( _name ) ){}
	};

} // sk::Object::

DECLARE_CLASS( sk::Object::Object )

#define OBJECT_PARENT_CLASS( ObjectName, ... ) sk::Object::iObject
#define OBJECT_PARENT_VALIDATOR( ObjectName, ... ) std::is_base_of_v< sk::Object::iObject, __VA_ARGS__ >
#define OBJECT_PARENT_CREATOR_2( ObjectName, ... ) AFTER_FIRST( __VA_ARGS__ )
#define OBJECT_PARENT_CREATOR_1( ObjectName, ... ) cObject< M_CLASS( ObjectName ), ObjectName :: runtime_class_t, ObjectName :: CONCAT( runtime_class_, ObjectName ) >
#define OBJECT_PARENT_CREATOR( ObjectName, ... ) CONCAT( OBJECT_PARENT_CREATOR_, VARGS( __VA_ARGS__ ) ) ( ObjectName, __VA_ARGS__ )
#define QW_OBJECT_CLASS( ObjectName, ... ) QW_RESTRICTED_CLASS( ObjectName, OBJECT_PARENT_CLASS, OBJECT_PARENT_CREATOR, OBJECT_PARENT_VALIDATOR, EMPTY __VA_OPT__( , __VA_ARGS__ ) )
// TODO: Rename to SK_OBJECT_CLASS

#define OBJECT_CLASS_PARENT( ObjectName )
//#define QW_OBJECT_CLASS( ObjectName ) QW_CLASS( ObjectName, OBJECT_CLASS_PARENT, EMPTY )