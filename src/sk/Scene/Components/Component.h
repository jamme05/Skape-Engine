/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <sk/Math/Matrix4x4.h>
#include <sk/Math/Transform.h>
#include <sk/Misc/Hashing.h>
#include <sk/Misc/Smart_Ptrs.h>
#include <sk/Misc/UUID.h>
#include <sk/Reflection/RuntimeClass.h>
#include <sk/Scene/Managers/EventManager.h>

namespace sk
{
	class cSceneManager;
} // sk::

namespace sk::Object
{
	class iObject;
} // sk::Object

namespace sk::Object
{
	typedef uint64_t hash_t;

	GENERATE_CLASS( iComponent ), public cShared_from_this< iComponent >
	{
		CREATE_CLASS_IDENTIFIERS( iComponent, runtime_class_iComponent )

		friend class iObject;
		friend class sk::cSceneManager;
	protected:
		iComponent()
		{
			m_self_      = get_weak();
			m_transform_ = sk::make_shared< cTransform >();
		}
	sk_public:

		iComponent( iComponent const& ) = delete;

		~iComponent() override
		{
			m_object_ = nullptr;
			m_children_.clear();
		}

		// TODO: Make them protected
		// Event bases
		virtual void update      (){}
		virtual void render      (){}
		virtual void enabled     (){}
		virtual void disabled    (){}
		virtual void debug_render(){}

		[[ nodiscard ]]
		bool         GetEnabled() const { return m_enabled_; }
		virtual void SetEnabled( bool _is_enabled ) = 0;

		[[ nodiscard ]]
		bool         GetIsInternal() const { return m_internal_; }

		virtual void PostEvent( uint16_t _event ) = 0;

		[[ nodiscard ]]
		auto& GetPosition()       { return m_transform_->GetLocalPosition(); }
		[[ nodiscard ]]
		auto& GetPosition() const { return m_transform_->GetLocalPosition(); }
		void  SetPosition( const cVector3f& _position ){ m_transform_->SetLocalPosition( _position ); }

		[[ nodiscard ]]
		auto& GetRotation()       { return m_transform_->GetLocalRotation(); }
		[[ nodiscard ]]
		auto& GetRotation() const { return m_transform_->GetLocalRotation(); }
		void  SetRotation( const cVector3f& _rotation ){ m_transform_->SetLocalRotation( _rotation ); }

		[[ nodiscard ]]
		auto& GetScale()       { return m_transform_->GetLocalScale(); }
		[[ nodiscard ]]
		auto& GetScale() const { return m_transform_->GetLocalScale(); }
		void  SetScale( const cVector3f& _scale ){ m_transform_->SetLocalScale( _scale ); }

		[[ nodiscard ]]
		auto& GetTransform() const { return *m_transform_; }
		
		[[ nodiscard ]]
		auto& GetSharedTransform() const { return m_transform_; }

		auto& GetChildren() const { return m_children_; }

		auto& GetUUID() const { return m_uuid_; }

		void SetParent( const cShared_ptr< iComponent >& _component )
		{
			if( m_parent_ && !m_parent_.Lock()->m_children_.empty() )
			{
				if( const auto itr = std::ranges::find( m_parent_->m_children_, m_self_.Lock() ); itr != m_parent_->m_children_.end() )
					m_parent_->m_children_.erase( itr );
			}
			m_parent_ = _component;
			if( _component )
				_component->m_children_.emplace_back( m_self_.Lock() );
			m_transform_->SetParent( ( _component != nullptr ) ? _component->m_transform_ : nullptr );
		}

	sk_protected:
		virtual void setEnabled( const bool _is_enabled ){ m_enabled_ = _is_enabled; }
		virtual void registerEvents() = 0;
		
		cShared_ptr< cTransform > m_transform_;
		cWeak_Ptr< iComponent >   m_parent_    = nullptr;
		cWeak_Ptr< iObject >      m_object_    = nullptr;
		// TODO: Have the UUID be able to be loaded from a scene file in the future.

		// TODO: Make the children into an unordered map

	sk_private: // TODO: Move parts to cpp, find way to make actual constexpr

		void registerRecursive()
		{
			registerEvents();
			enabled();
			m_transform_->Update();
			for( auto& child : m_children_ )
				child->registerRecursive();
		}
		void enableRecursive ()
		{
			for( auto& child : m_children_ )
			{
				if( child->m_enabled_ )
					child->enableRecursive();
			}
		}
		void disableRecursive()
		{
			for( auto& child : m_children_ )
				child->disableRecursive();
		}



		std::vector< cShared_ptr< iComponent > > m_children_ = { }; // TODO: Add get children function
		cUUID m_uuid_     = {};
		bool  m_enabled_  = true;
		// Internal will hide it from the editor.
		bool  m_internal_ = false;

		cWeak_Ptr< iComponent > m_self_;
	};

	// TODO: Check if type is necessary
	template< class Ty, uint16_t Events >
	class cComponent : public iComponent, public Event::cEventListener
	{
		friend class sk::cSceneManager;
#define HAS_EVENT( Func, Val ) if constexpr( !std::is_same_v< decltype( &Ty::Func ), decltype( &iComponent::Func ) > ) events |= (Val)
		constexpr static uint16_t detect_events()
		{
			uint16_t events = kNone;

			HAS_EVENT( update,       kUpdate      );
			HAS_EVENT( render,       kRender      );
			HAS_EVENT( enabled,      kEnabled     );
			HAS_EVENT( disabled,     kDisabled    );
			HAS_EVENT( debug_render, kDebugRender );

			return events;
		} // detectEvents
#undef HAS_EVENT
	protected:
		static constexpr uint16_t kEventMask = detect_events() & Events;

		cComponent() = default;
	public:

		void SetEnabled( const bool _is_enabled ) final { setEnabled( _is_enabled ); _is_enabled ? postEvent< kEnabled >() : postEvent< kDisabled >(); }
		
		// Runtime postEvent
		void PostEvent( const uint16_t _event ) override
		{
			switch( _event )
			{
			case kEnabled:     postEvent< kEnabled     >(); break;
			case kDisabled:    postEvent< kDisabled    >(); break;
			case kUpdate:      postEvent< kUpdate      >(); break;
			case kRender:      postEvent< kRender      >(); break;
			case kDebugRender: postEvent< kDebugRender >(); break;
			default: break;
			}
		} // postEvent

		// Compile time postEvent
		template< uint16_t Event >
		constexpr void postEvent() // TODO: Add some sort of event input. Maybe Args?
		{
			constexpr auto mask = Event & kEventMask;
			// Constexpr "switch" as we only expect a single event at a time.
			if constexpr( mask == kUpdate      ) update      ();
			if constexpr( mask == kRender      ) render      ();
			if constexpr( mask == kEnabled     ) enabled     ();
			if constexpr( mask == kDisabled    ) disabled    ();
			if constexpr( mask == kDebugRender ) debug_render();
		} // postEvent

	private:
		void _updateInternal()
		{
			if constexpr( kEventMask & kUpdate )
			{
				update();
			}
			
			if( m_transform_->IsDirty() )
				m_transform_->Update();
		}
#ifdef SKAPE_EDITOR_AVAILABLE
		void _editorUpdateInternal()
		{
			if( m_transform_->IsDirty() )
				m_transform_->Update();
		}
#endif // SKAPE_EDITOR_AVAILABLE

		void registerEvents() final
		{
			RegisterListener( kUpdate, &cComponent::_updateInternal );
#ifdef SKAPE_EDITOR_AVAILABLE
			RegisterListener( kEditorUpdate, &cComponent::_editorUpdateInternal );
#endif // SKAPE_EDITOR_AVAILABLE
			if constexpr( kEventMask & kRender      ) RegisterListener( kRender,      &Ty::render       );
			if constexpr( kEventMask & kDebugRender ) RegisterListener( kDebugRender, &Ty::debug_render );
		} // register_events
	};

} // sk::Object

namespace sk::Object::Component
{
	using class_type = iComponent;
} // sk::Object::Component
SK_DECLARE_CLASS( sk::Object::Component )

#define COMPONENT_PARENT_CLASS( ComponentName, ... ) sk::Object::iComponent
#define COMPONENT_PARENT_VALIDATOR( ComponentName, ... ) std::is_base_of_v< sk::Object::iComponent, __VA_ARGS__ >
#define COMPONENT_PARENT_CREATOR_2( ComponentName, ... ) AFTER_FIRST( __VA_ARGS__ )
#define COMPONENT_PARENT_CREATOR_1( ComponentName, ... ) sk::Object::cComponent< M_CLASS( ComponentName ), sk::Object::kAll >
#define COMPONENT_PARENT_CREATOR( ComponentName, ... ) CONCAT( COMPONENT_PARENT_CREATOR_, VARGS( __VA_ARGS__ ) ) ( ComponentName, __VA_ARGS__ )
#define SK_COMPONENT_CLASS( ComponentName, ... ) QW_RESTRICTED_CLASS( ComponentName, COMPONENT_PARENT_CLASS, COMPONENT_PARENT_CREATOR, COMPONENT_PARENT_VALIDATOR, EMPTY __VA_OPT__( , __VA_ARGS__ ) )
