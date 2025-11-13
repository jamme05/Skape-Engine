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
		explicit iObject( std::string _name )
		: m_root( sk::make_shared< Components::cTransformComponent >() )
		, m_name( std::move( _name ) )
		{
		} // iObject

		template< class Ty = iComponent, class... Args >
		explicit iObject( std::string _name, Args... _args )
		: m_root( sk::make_shared< Ty >( _args... ) )
		, m_name( std::move( _name ) )
		{} // iObject

		~iObject( void ) override
		{
			m_children.clear();
			m_components.clear();
			m_root = nullptr;
		}

		template< class Ty, class... Args >
		requires std::is_base_of_v< iComponent, Ty >
		cShared_ptr< Ty > addComponent( Args&&... _args )
		{
			auto ptr = sk::make_shared< Ty >( std::forward< Args >( _args )... );

			ptr->m_object = get_weak();
			ptr->setParent( m_root );

			m_components.insert( std::pair{ Ty::getStaticType(), ptr } );

			return ptr;
		} // addComponent

		// TODO: Deprecate?
		virtual void render( void )
		{
			// TODO: Add actual event vector or something.
			for( auto& val : m_components | std::views::values )
			{
				val->postEvent( kRender );
				val->postEvent( kDebugRender );
			}
		} // render

		virtual void update( void )
		{
			for( auto& val : m_components | std::views::values )
				val->postEvent( kUpdate );
		} // update

		auto& getRoot( void )       { return m_root; }
		auto& getRoot( void ) const { return m_root; }

		auto& getPosition ( void )       { return m_root->getPosition(); }
		auto& getPosition ( void ) const { return m_root->getPosition(); }

		auto& getRotation ( void )       { return m_root->getRotation(); }
		auto& getRotation ( void ) const { return m_root->getRotation(); }

		auto& getScale    ( void )       { return m_root->getScale(); }
		auto& getScale    ( void ) const { return m_root->getScale(); }

		auto& getTransform( void )       { return m_root->getTransform(); }
		auto& getTransform( void ) const { return m_root->getTransform(); }

		auto& getName     ( void ) const { return m_name; }

	sk_protected:
		cShared_ptr< iComponent > m_root;

	sk_private:
		// TODO: Move types to typedefs
		vector             < cShared_ptr< iObject > >    m_children   = { };
		multimap< type_hash, cShared_ptr< iComponent > > m_components = { };

		std::string m_name;

		// TODO: Make this into a weak ptr.
		cScene* m_parent_scene = nullptr;

		// TODO: Actually implament these. Follow Unity's design when it comes to layers. But do allow multiple tags.
		std::vector< str_hash > m_tags_;
		uint32_t m_layer_ = 0;

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