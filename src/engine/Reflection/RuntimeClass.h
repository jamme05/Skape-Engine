/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include "Misc/Hashing.h"

#include "Macros/Manipulation.h"

#include <Reflection/Types.h>

#include <print>

namespace sk
{
	class iClass;

	class iRuntimeClass
	{
	public:
		[[ deprecated( "Deprecated due to source_location, use overloaded version with it as an input instead." ) ]]
		constexpr iRuntimeClass( const char* _name, const char* _file, const uint32_t _line = 0, const uint64_t& _parent_hash = Hashing::val_64_const )
		: m_hash( _name )
		, m_raw_name( _name )
		, m_file_path( _file )
		, m_line( _line )
		{ } // iRuntimeClass

		consteval iRuntimeClass( const char* _name, const std::source_location& _location = std::source_location::current(), const uint64_t& _parent_hash = Hashing::val_64_const )
		: m_hash( _name )
		, m_raw_name( _name )
		, m_file_path( _location.file_name() )
		, m_line( _location.line() )
		{ } // iRuntimeClass

		consteval iRuntimeClass( const char* _name, type_hash _hash )
		: m_hash( std::move( _hash ) )
		, m_raw_name( _name )
		, m_file_path( nullptr )
		, m_line( 0 )
		{ } // iRuntimeClass
		
		virtual ~iRuntimeClass() = default;

		virtual iClass* create( void )
		{
			// TODO: Create function reflection system to allow for overload selection.
			return nullptr;
		} // create

		constexpr auto& getType    ( void ) const { return m_hash;      }
		constexpr auto  getRawName ( void ) const { return m_raw_name;  }
		constexpr auto  getFileName( void ) const { return m_file_path; }
		constexpr auto  getLine    ( void ) const { return m_line;      }
		auto            getName    ( void ) const { return std::string( m_raw_name ); }

		virtual constexpr bool isDerivedFrom( const iRuntimeClass& _base    ) const { return false; } // Has to be set so iClass isn't a pure virtual
		virtual constexpr bool isBaseOf     ( const iRuntimeClass& _derived ) const { return false; } // Has to be set so iClass isn't a pure virtual

		constexpr bool operator==( const iRuntimeClass& _right ) const { return m_hash == _right.m_hash; }

	private:
		type_hash   m_hash;
		const char* m_raw_name;
		const char* m_file_path;
		size_t      m_line;
	};

	constexpr static iRuntimeClass kInvalidClass{ "Invalid" };

	template< bool Select, class Ty, class Ty2 >
	struct select_class_type{};

	template< class Ty, class Ty2 >
	struct select_class_type< true, Ty, Ty2 >
	{
		typedef std::remove_cvref_t< decltype( Ty::getStaticClass() ) > type;
	};
	template< class Ty, class Ty2 >
	struct select_class_type< false, Ty, Ty2 >
	{
		typedef Ty2 type;
	};

	template< bool Select, class Ty, class Ty2 > // TODO: Move to a more global space, might be liked. Also maybe make an int variant?
	struct select_type{};

	template< class Ty, class Ty2 >
	struct select_type< true, Ty, Ty2 >
	{
		typedef Ty type;
	};
	template< class Ty, class Ty2 >
	struct select_type< false, Ty, Ty2 >
	{
		typedef Ty2 type;
	};

	template< bool Select, class Ty >
	struct select_class_ref{};

	template< class Ty >
	struct select_class_ref< true, Ty >
	{
		static constexpr auto& class_ref = Ty::getStaticClass();
	};
	template< class Ty >
	struct select_class_ref< false, Ty >
	{
		static constexpr auto& class_ref = kInvalidClass;
	};

	template< class Ty >
	struct get_parent_class
	{
	private:
		template< class Ty2 >
		static std::true_type test( decltype( &Ty2::getStaticClass ) );
		template< class >
		static std::false_type test( ... );
	public:
		static constexpr bool has_value = decltype( test< Ty >( 0 ) )::value;
	private:
		typedef typename select_class_type< has_value, Ty, iRuntimeClass >::type pre_type;
	public:
		static constexpr bool is_valid = std::is_base_of_v< iRuntimeClass, pre_type >;
		static constexpr bool is_base  = std::is_same_v< pre_type, iRuntimeClass >;
		static constexpr bool uses_own = is_valid && !is_base;
		typedef typename select_class_type< uses_own, Ty, iRuntimeClass >::type class_type;
		typedef typename select_type< uses_own, Ty, iClass >::type inherits_type;
	};

	template< class Ty >
	constexpr auto& get_class_ref = select_class_ref< get_parent_class< Ty >::uses_own, Ty >::class_ref;

	template< class Ty >
	using get_parent_class_t = typename get_parent_class< Ty >::class_type;
	template< class Ty = iClass >
	using get_inherits_t     = typename get_parent_class< Ty >::inherits_type;

	template< class Ty, class Pa = iRuntimeClass, const get_parent_class_t< Pa >& Parent = get_class_ref< Pa >, bool ForceShared = true >
	requires std::is_base_of_v< iClass, Pa >
	class cRuntimeClass : public get_parent_class_t< Pa >
	{
	public:
		typedef Ty                       value_type;
		typedef get_parent_class_t< Pa > parent_type;

		// TODO: Move
		typedef typename get_parent_class< Pa >::inherits_type inherits_type;

		constexpr cRuntimeClass( const char* _name, const std::source_location& _location = std::source_location::current(), const uint64_t& _parent_hash = Parent.getType().getHash() )
		: parent_type( _name, _location, _parent_hash )
		{} // cRuntimeClass

		[[ deprecated( "Deprecated due to source_location, use overloaded version with it as an input instead." ) ]]
		constexpr cRuntimeClass( const char* _name, const char* _file, const uint32_t _line = 0, const uint64_t& _parent_hash = Parent.getType().getHash() )
		: parent_type( _name, _file, _line, _parent_hash )
		{} // cRuntimeClass

		// Use std::is_base_of / std::is_base_of_v instead of this in case both types are known.
		[[ nodiscard ]] constexpr bool isDerivedFrom( const iRuntimeClass& _base ) const override
		{
			if( *this == _base )
				return true;

			// The placeholder parent will always be of the type iClass
			if constexpr( std::is_same_v< iRuntimeClass, parent_type > )
				return false;
			else
				return Parent.isDerivedFrom( _base );
		} // isDerivedFrom

		constexpr bool isBaseOf( const iRuntimeClass& _derived ) const override
		{
			return _derived.isDerivedFrom( *this );
		} // isBaseOf

		template< class... Args >
		requires ( !ForceShared )
		Ty* create( Args&&... ){ return nullptr; } // TODO: Create function
	};

	class iClass
	{
	public:
		iClass( void ) = default;
		virtual ~iClass( void ) = default;
		virtual constexpr const iRuntimeClass& getClass    ( void ) const = 0;
		virtual constexpr const type_hash&     getClassType( void ) = 0;
		virtual                 std::string    getClassName( void ) = 0;

		// Here for placeholder and template help.
		
		static constexpr auto& kClass = kInvalidClass;
		// Recommended to use Ty::kClass instead.
		static constexpr auto& getStaticClass( void ) { return kClass; }
		static constexpr auto& getStaticType( void ) { return kClass.getType(); }
		static auto getStaticName( void ) { return kClass.getName(); }
	};

	template< class Ty >
	requires std::is_base_of_v< iClass, Ty >
	struct get_type_info< Ty >
	{
		constexpr static auto&     kClass  = Ty::getStaticClass();
		constexpr static type_hash kId     = kClass.getType().getHash();
		constexpr static char      kName[] = kClass.getRawName();
	};

	template< class Ty >
	constexpr static bool is_valid_class_v = std::is_base_of_v< iClass, Ty >;
} // sk::

#define QW_BASE_CLASS( ... ) sk::iClass
#define QW_CLASS_VALID( ClassName, Parent, ... ) sk::is_valid_class_v< Parent >
#define QW_MESSAGE_CLASS_VALID( ClassName, Parent, ... ) sk::is_valid_class_v< Parent >, "Class " #Parent " isn't in the reflection system."

#define CREATE_CLASS_IDENTITY_IDENTIFIERS( RuntimeClass ) \
	typedef decltype( RuntimeClass ) runtime_class_type; \
	constexpr static auto& kClass = RuntimeClass; \
	constexpr const sk::iRuntimeClass& getClass( void ) const override { return kClass;     } \
	constexpr const sk::type_hash& getClassType( void ) override { return kClass.getType(); } \
	std::string                    getClassName( void ) override { return kClass.getName(); } \
	static constexpr auto&  getStaticClass    ( void ){ return kClass;           } \
	static constexpr auto&  getStaticType( void ){ return kClass.getType(); } \
	static auto             getStaticName( void ){ return kClass.getName(); } \

#define CREATE_MEMBER_REFLECTION_VALUES( RuntimeClass ) \
	private: \
	consteval auto _extract_type(){ return this; } \
	struct CONCAT( var_tag_ ){}; \
	struct CONCAT( func_tag_ ){}; \
	public: \
	using class_type = std::remove_pointer_t< decltype( _extract_type() ) > ; \
	using var_counter_t  = const_counter< var_tag_ >;   \
	using func_counter_t  = const_counter< func_tag_ >; \
	template< size_t > struct member_registry { \
		static constexpr auto kMembers = sk::cLinked_Array< const sk::Reflection::cMemberVariable* >{}; }; \
	template< size_t > struct function_registry { \
		static constexpr auto kMembers = sk::cLinked_Array< const sk::Reflection::cMemberFunction* >{}; }; \
	private:


// Internal use only.
#define CREATE_CLASS_IDENTIFIERS_0_( RuntimeClass ) public: \
	/* Prepare function to get information about which class it is. */ \
	CREATE_CLASS_IDENTITY_IDENTIFIERS( RuntimeClass ) \
	/* Prepare for member reflection: */ \
	CREATE_MEMBER_REFLECTION_VALUES( RuntimeClass )

// Required to make a runtime class functional.
#define CREATE_CLASS_IDENTIFIERS( RuntimeClass ) \
	CREATE_CLASS_IDENTIFIERS_0_( RuntimeClass )


#define CREATE_CLASS_BODY( Class ) CREATE_CLASS_IDENTIFIERS( runtime_class_ ## Class )

#define CREATE_RUNTIME_CLASS_TYPE( Class, Name, ... ) sk::cRuntimeClass< Class __VA_OPT__(,) FORWARD( __VA_ARGS__ ) >
#define CREATE_RUNTIME_CLASS_VALUE( Class, Name, ... ) static constexpr auto CONCAT( runtime_class_, Name ) = CREATE_RUNTIME_CLASS_TYPE( Class, Name __VA_OPT__(, __VA_ARGS__) ) ( #Name );

// Requires you to manually add CREATE_CLASS_IDENTIFIERS inside the body. But gives greater freedom. First inheritance will always have to be public. Unable to function with templated classes.
// Deprecated
#define GENERATE_CLASS( Class, ... ) \
class Class ; \
CREATE_RUNTIME_CLASS_VALUE( Class, Class, __VA_ARGS__ ) \
class Class : public sk::get_inherits_t< FIRST( __VA_ARGS__ ) > \

// Generates both runtime info and start of body, but removes most of your freedom. Unable to function with templated classes.
// Deprecated
#define GENERATE_ALL_CLASS( Class, ... ) GENERATE_CLASS( Class __VA_OPT__(,) __VA_ARGS__ ) AFTER_FIRST( __VA_ARGS__ ) { CREATE_CLASS_BODY( Class )

#define TRUE_MAC( ... ) true

#define DEFAULT_CLASS_CREATOR_2( ... ) SECOND( __VA_ARGS__ )
#define DEFAULT_CLASS_CREATOR_1( ... ) FIRST( __VA_ARGS__ )
#define DEFAULT_CLASS_CREATOR( ClassName, ... ) CONCAT( DEFAULT_CLASS_CREATOR_, VARGS( __VA_ARGS__ ) )( __VA_ARGS__ )

#define PICK_CLASS_3( ParentMacro, ParentCreator, CustomClass ) ParentCreator
#define PICK_CLASS_2( ParentMacro, ParentCreator ) ParentMacro
#define PICK_CLASS( ... ) CONCAT( PICK_CLASS_, VARGS( __VA_ARGS__ ) )( __VA_ARGS__ )

// TODO: Use AFTER_FIRST to parse away the parent class?
/**
 * Not recommended to be used directly.
 * @param ClassName The name of the class
 * @param ClassType The type of the class, aka name after naming convention.
 * @param ParentValidator Has a constexpr bool deciding if the parent is valid or not.
 * @param ParentCreator Creates the final parent class. Includes said parent class if it isn't default.
 * @param ExtrasMacro Extra reflection data within the namespace.
 * @param ParentClass The parent class.
 * @param ... Extra info for ExtrasMacro.
 */
#define SK_CLASS_INTERNAL( Type, ClassName, ClassType, ParentValidator, ExtrasMacro, ParentCreator, ParentClass, ... ) \
	Type ClassType; \
	namespace ClassName { \
		static_assert( ParentValidator( ClassName, ParentClass ) ); \
		typedef sk::cShared_ptr< ClassType > ptr_t; \
		typedef sk::cWeak_Ptr< ClassType >   weak_t; \
		typedef sk::cShared_Ref< ClassType > ref_t; \
		CREATE_RUNTIME_CLASS_VALUE( ClassType, ClassName, ParentClass ) \
		ExtrasMacro( ClassName __VA_OPT__( , ) __VA_ARGS__ ) \
		typedef CREATE_RUNTIME_CLASS_TYPE( ClassType, ClassName, ParentClass ) runtime_class_t; \
	} \
	class ClassType : public ParentCreator( ClassName, ParentClass __VA_OPT__(, ParentClass ) )

#define PICK_VALIDATOR( A, B, ... ) PICK_CLASS( A, B __VA_OPT__( , FIRST( __VA_ARGS__ ) ) )
#define PICK_PARENT_MAC( A, B, ... ) PICK_CLASS( A, B __VA_OPT__( , FIRST( __VA_ARGS__ ) ) )

/**
 * Used below another macro to add requirements for class inheritance.
 * Creates a class with a wider range of customization and restrictions.
 * Have: QW_CLASS_BODY( ClassName ) inside the class body to complete the reflection.
 * @param ClassName The name of the class
 * @param ParentMacro Macro for getting the default parent class. Args: ClassName, ...
 * @param ParentCreator Post-processing of the parent class. Will send the parent class again to allow for customization. Args: ClassName, ParentClass, [ParentClass] ...
 * @param ParentValidator A macro that returns a bool deciding if the parent class is valid. Args: ClassName, Parent, ...
 * @param ExtrasMacro In case there's demand for making anymore metadata. Args: ClassName, ...
 * @param ... First argument is an optional Parent class. ParentMacro won't be called in this scenario. It and the rest will be forwarded into the macros.
 */
#define QW_RESTRICTED_CLASS( ClassName, ParentMacro, ParentCreator, ParentValidator, ExtrasMacro, ... ) \
	SK_CLASS_INTERNAL( class, ClassName, M_CLASS( ClassName ), PICK_VALIDATOR( TRUE_MAC, ParentValidator __VA_OPT__(,) __VA_ARGS__ ), ExtrasMacro, ParentCreator, PICK_CLASS( ParentMacro, SECOND __VA_OPT__( , FIRST( __VA_ARGS__ ) ) ) ( ClassName, __VA_ARGS__ ) __VA_OPT__(,) __VA_ARGS__ )

/**
 * Used below another macro to add requirements for class inheritance.
 * Creates a class with a wider range of customization and restrictions.
 * Have: QW_CLASS_BODY( ClassName ) inside the class body to complete the reflection.
 * @param ClassName The name of the class
 * @param ParentMacro Macro for getting the default parent class. Args: ClassName, ...
 * @param ParentCreator Post-processing of the parent class. Will send the parent class again to allow for customization. Args: ClassName, ParentClass, [ParentClass] ...
 * @param ParentValidator A macro that returns a bool deciding if the parent class is valid. Args: ClassName, Parent, ...
 * @param ExtrasMacro In case there's demand for making anymore metadata. Args: ClassName, ...
 * @param ... First argument is an optional Parent class. ParentMacro won't be called in this scenario. It and the rest will be forwarded into the macros.
 */
#define QW_RESTRICTED_STRUCT( ClassName, ParentMacro, ParentCreator, ParentValidator, ExtrasMacro, ... ) \
	SK_CLASS_INTERNAL( struct, ClassName, M_CLASS( ClassName ), PICK_VALIDATOR( TRUE_MAC, ParentValidator __VA_OPT__(,) __VA_ARGS__ ), ExtrasMacro, ParentCreator, PICK_CLASS( ParentMacro, SECOND __VA_OPT__( , FIRST( __VA_ARGS__ ) ) ) ( ClassName, __VA_ARGS__ ) __VA_OPT__(,) __VA_ARGS__ )

/**
 * Creates classes with extra reflection metadata.
 * Have: QW_CLASS_BODY( ClassName ) inside the class body to complete the reflection.
 * @param Type class/struct
 * @param ClassName The name of the class
 * @param ExtrasMacro In case there's demand for making anymore metadata. Args: ClassName, ...
 * @param ... First argument is an optional Parent class. ParentMacro won't be called in this scenario. It and the rest will be forwarded into the macros.
 */
#define SK_CLASS_EX( Type, ClassName, ExtrasMacro, ... ) \
	SK_CLASS_INTERNAL( Type, ClassName, M_CLASS( ClassName ), QW_MESSAGE_CLASS_VALID, ExtrasMacro, DEFAULT_CLASS_CREATOR, PICK_CLASS( QW_BASE_CLASS, SECOND __VA_OPT__( , FIRST( __VA_ARGS__ ) ) ) ( ClassName, __VA_ARGS__ ), __VA_ARGS__ )

/**
 * Creates classes reflected with the default metadata.
 * Have: `QW_CLASS_BODY( ClassName )` inside the class body to complete the reflection.
 * @param ClassName The name of the class
 * @param Parent Optional parent class
 * @param ... ParentMacro won't be called in this scenario. It and the rest will be forwarded into the macros.
 */
#define SK_CLASS( ClassName, ... ) \
	SK_CLASS_EX( class, ClassName, EMPTY, __VA_ARGS__ )

/**
 * Creates classes reflected with the default metadata.
 * Have: `QW_CLASS_BODY( ClassName )` inside the class body to complete the reflection.
 * @param ClassName The name of the class
 * @param Parent Optional parent class
 * @param ... ParentMacro won't be called in this scenario. It and the rest will be forwarded into the macros.
 */
#define SK_STRUCT( ClassName, ... ) \
	SK_CLASS_EX( struct, ClassName, EMPTY, __VA_ARGS__ )

/**
 * Creates everything required to get the class functional. Used in combination with QW_CLASS
 * @param ClassName The name for the class to create the body for.
 */
#define SK_CLASS_BODY( ClassName ) CREATE_CLASS_IDENTIFIERS( ClassName :: CONCAT( runtime_class_, ClassName ) )

namespace sk::Reflection
{
	template< class Ty, const sType_Info& Info >
	concept is_same = &get_type_info< Ty >::kInfo == &Info;

	class cMember
	{
	public:
		struct eType
		{
			enum eRawType : uint8_t
			{
				kNone     = 0x00,

				kVariable = 0x00,
				kFunction = 0x01,
				kTypeMask = kFunction,

				kPublic     = 0x00,
				kProtected  = 0x02,
				kPrivate    = 0x04,
				kVisibility = kPublic | kProtected | kPrivate,

				// Modifiers
				kStatic    = 0x08,
				kVirtual   = 0x10,
				kConst     = 0x20,
				// Alias for Const
				kReadOnly  = 0x20,
			};
			static constexpr auto getType      ( const uint8_t _type ){ return static_cast< eRawType >( kTypeMask & _type ); }
			static constexpr auto getVisibility( const uint8_t _type ){ return static_cast< eRawType >( kVisibility & _type ); }
			static constexpr bool getIsStatic  ( const uint8_t _type ){ return kStatic & _type; }
			static constexpr bool getIsVirtual ( const uint8_t _type ){ return kVirtual & _type; }
			static constexpr bool getIsReadOnly( const uint8_t _type ){ return kReadOnly & _type; }
		};

		consteval cMember( const char* _name, const uint8_t& _type )
		: m_name( _name ), m_flags( _type ){}
		constexpr cMember( const cMember& _other ) = default;
		virtual constexpr ~cMember( void ) = default;

		constexpr bool operator==( const cMember& _other ) const
		{
			return m_name == _other.m_name;
		}

		constexpr [[ nodiscard ]] auto getName() const { return m_name; }

		constexpr [[ nodiscard ]] auto getFlags     ( void ) const { return m_flags; }
		constexpr [[ nodiscard ]] auto getMemberType( void ) const { return eType::getType( m_flags ); }
		constexpr [[ nodiscard ]] auto getIsStatic  ( void ) const { return eType::getIsStatic( m_flags ); }
		constexpr [[ nodiscard ]] auto getIsVirtual ( void ) const { return eType::getIsVirtual( m_flags ); }
		constexpr [[ nodiscard ]] auto getIsReadOnly( void ) const { return eType::getIsReadOnly( m_flags ); }
		constexpr [[ nodiscard ]] auto getVisibility( void ) const { return eType::getVisibility( m_flags ); }



	private:
		const char* m_name;
		uint8_t     m_flags;
	};

	template< class Ty >
	class cMemberVariableInstance;

	class iMemberVariableHolder
	{
	public:
		virtual constexpr ~iMemberVariableHolder( void ) = default;

	protected:
		// Getters return nullptr on fail, whilst setters return false on fail.
		[[ nodiscard ]] virtual void* get() const = 0;
		[[ nodiscard ]] virtual bool  set( void* _src ) const = 0;
		[[ nodiscard ]] virtual void* get( void* _inst ) const = 0;
		[[ nodiscard ]] virtual bool  set( void* _inst, void* _src ) const = 0;

		friend class cMemberVariable;
		template< class Ty >
		friend class cMemberVariableInstance< Ty >;
	};

	// Non-static Member variable container
	template< class Ty, class Me >
	class cMemberVariableHolder final : public iMemberVariableHolder
	{
	public:
		explicit consteval cMemberVariableHolder( Me Ty::* _member )
		: m_member_( _member )
		{}

		using enum cMember::eType::eRawType;
		static constexpr uint8_t kFlags = kVariable | std::is_const_v< Me > * kConst;

	private:
		Me Ty::* m_member_;
	protected:

		// Unsupported. Requires class instance.
		void* get() const override
		{
			return nullptr;
		}

		// Unsupported. Requires class instance.
		bool set( void* _src ) const override { return false; }

		void* get( void* _inst ) const override
		{
			if( _inst == nullptr )
				return nullptr;

			Ty*    inst = static_cast< Ty* >( _inst );
			auto   var  = &inst->*m_member_;
			return var;
		}

		bool set( void* _inst, void* _src ) const override
		{
			if( _inst == nullptr || _src == nullptr )
				return false;

			Ty*   inst = static_cast< Ty* >( _inst );
			auto& var  = inst->*m_member_;
			var = *static_cast< Me* >( _src );
			return true;
		}
	};

	// Static member variable container
	template< class Ty >
	class cMemberVariableHolder< void, Ty > final : public iMemberVariableHolder
	{
	public:
		explicit consteval cMemberVariableHolder( Ty* _member )
		: m_value_( _member )
		{}

		using enum cMember::eType::eRawType;
		static constexpr uint8_t kFlags = kVariable | kStatic | std::is_const_v< Ty > * kConst;

	protected:
		[[ nodiscard ]] void* get() const override
		{
			return m_value_;
		}
		bool set( void* _src ) const override
		{
			*m_value_ = *static_cast< Ty* >( _src );
			return true;
		}

		// Static doesn't require instance so simply use the static getter/setter.
		[[ nodiscard ]] void* get( void* _inst ) const override { return get(); }
		[[ nodiscard ]] bool  set( void* _inst, void* _src ) const override { return set( _src ); }

	private:
		Ty* m_value_;
	};

	// Can modify static variables or variables linked with a member instance.
	class cMemberVariable : public cMember
	{
	protected:
		typedef const iMemberVariableHolder* holder_t;
		typedef const iRuntimeClass* class_t;
		const sType_Info* m_member_type_info_;
		class_t           m_class_;
		holder_t          m_holder_;
	public:
		// Non-Static
		template< class Ty = iClass, class Me >
		requires std::is_base_of_v< iClass, Ty >
		consteval cMemberVariable( const char* _name, const cMemberVariableHolder< Ty, Me >& _holder, const uint8_t& _extras )
		: cMember( _name, _extras | _holder.kFlags )
		, m_member_type_info_( &get_type_info< Me >::kInfo )
		, m_class_( &Ty::kClass )
		, m_holder_( &_holder )
		{}

		// Static
		template< class Ty >
		consteval cMemberVariable( const char* _name, const cMemberVariableHolder< void, Ty >& _holder, const uint8_t& _extras )
		: cMember( _name, _extras | _holder.kFlags )
		, m_member_type_info_( &get_type_info< Ty >::kInfo )
		, m_class_( nullptr )
		, m_holder_( &_holder )
		{}

		// Static or instance get/set.
		template< class Ty >
		 Ty* get() const;
		template< class Ty >
		bool set( Ty* _src ) const;

		// Direct member get/set.
		template< class Ty, class Me >
		requires std::is_base_of_v< iClass, Ty >
		Me* get( Ty* _inst ) const;
		template< class Ty, class Me >
		requires std::is_base_of_v< iClass, Ty >
		bool set( Ty* _inst, Me* _src ) const;

	protected:
		// Direct access getter and setter.
		virtual void* get_() const { return m_holder_->get(); }
		virtual bool set_( void* _src ) const { return m_holder_->set( _src ); }
	};

	template< class Ty >
	Ty* cMemberVariable::get() const
	{
		constexpr static auto& other_type = get_type_info< Ty >::kInfo;
		if( *m_member_type_info_ != other_type )
		{
			// TODO: Add error.
			std::print( "Member {} is of type {} and not type {} which was provided.", getName(), m_member_type_info_->raw_name, other_type.raw_name );
			return nullptr;
		}
		return static_cast< Ty* >( get_() );
	} // get

	template< class Ty >
	bool cMemberVariable::set( Ty* _src ) const
	{
		if( getIsReadOnly() )
		{
			std::print( "Member {} is read-only.", getName() );
			return false;
		}
		constexpr static auto& other_type = get_type_info< Ty >::kInfo;
		if( *m_member_type_info_ != other_type )
		{
			// TODO: Add error.
			std::print( "Error: Member {} is of type {} and not type {} which was provided.", getName(), m_member_type_info_->raw_name, other_type.raw_name );
			return nullptr;
		}
		return set_( static_cast< void* >( _src ) );
	} // set

	template< class Ty = iClass, class Me > requires std::is_base_of_v< iClass, Ty >
	Me* cMemberVariable::get( Ty* _inst ) const
	{
		if( getIsStatic() )
		{
			std::println( "Warning: Accessing static value through instance." );
			return get< Me >();
		}
		if( *m_class_ != Ty::kClass )
		{
			// TODO: Support a hierarchy.
			std::print( "Error: Tried accessing value through class {} when it required class {}.", Ty::getClassName(), m_class_->getName() );
			return nullptr;
		}
		constexpr static auto& other_type = get_type_info< Ty >::kInfo;
		if( *m_member_type_info_ != other_type )
		{
			// TODO: Add error.
			std::print( "Error: Member {} is of type {} and not type {} which was provided.", getName(), m_member_type_info_->raw_name, other_type.raw_name );
			return nullptr;
		}
		return static_cast< Me* >( m_holder_->get( _inst ) );
	} // get

	template< class Ty = iClass, class Me > requires std::is_base_of_v< iClass, Ty >
	bool cMemberVariable::set( Ty* _inst, Me* _src ) const
	{
		if( m_class_ == nullptr )
		{
			std::println( "Warning: Accessing static value through instance." );
			return get< Me >();
		}
		if( *m_class_ != Ty::kClass )
		{
			// TODO: Support a class hierarchy. Class.AllMyMembers = Class.MyMembers + ParentClass.MyMembers
			// TODO: Add error as well.
			std::print( "Error: Tried accessing value through class {} when it required class {}.", Ty::getClassName(), m_class_->getName() );
			return nullptr;
		}
		constexpr static auto& other_type = get_type_info< Ty >::kInfo;
		if( *m_member_type_info_ != other_type )
		{
			// TODO: Add error.
			std::print( "Error: Member {} is of type {} and not type {} which was provided.", getName(), m_member_type_info_->raw_name, other_type.raw_name );
			return nullptr;
		}
		return m_holder_->set( _inst, _src );
	} // set

	// Non-static Member variable
	template< class Ty >
	class cMemberVariableInstance final : public cMemberVariable
	{
	public:
		constexpr cMemberVariableInstance( const cMemberVariable& _other, Ty* _instance )
		: cMemberVariable( _other )
		, m_instance_( _instance )
		{}

	protected:
		void* get_() const override
		{
			return m_holder_->get( m_instance_ );
		}
		bool set_( void* _src ) const override
		{
			return m_holder_->set( m_instance_, _src );
		}

	private:
		Ty* m_instance_;
	};

	template< class Ty, class Me >
	cMemberVariableHolder( Me Ty::* ) -> cMemberVariableHolder< Ty, Me >;
	template< class Ty >
	cMemberVariableHolder( Ty* ) -> cMemberVariableHolder< void, Ty >;

	class cMemberFunction
	{
			
	};

	template< size_t >
	struct testing
	{
			
	};

	using member_var_pair_t = std::pair< str_hash, cMember* >;
	using member_func_pair_t = std::pair< str_hash, cMember* >;

}

// Internal macro. Do not use.
#define REGISTER_MEMBER_DIRECT_1_( Member, Counter, Visibility ) \
	static constexpr auto Counter = var_counter_t::next(); \
	template<> struct member_registry< Counter > { \
	using prev_t = member_registry< (Counter) - 1 >; \
	static constexpr auto kMemberHolder = sk::Reflection::cMemberVariableHolder{ &class_type:: Member }; \
	static constexpr auto kMember       = sk::Reflection::cMemberVariable{ #Member, kMemberHolder, sk::Reflection::cMember::eType:: Visibility }; \
	static constexpr auto kMembers      = sk::cLinked_Array< const sk::Reflection::cMemberVariable* >{ &kMember, prev_t::kMembers }; };
			
// Internal macro. Do not use.
#define REGISTER_MEMBER_DIRECT_0_( Member, Visibility ) public: \
	REGISTER_MEMBER_DIRECT_1_( Member, CONCAT( member_id_,__COUNTER__ ), Visibility )

// Internal macro. Do not use.
#define REGISTER_FUNCTION_DIRECT_1_( Member, Counter, Visibility ) \
	static constexpr auto Counter = func_counter_t::next(); \
	template<> struct member_registry< Counter > { \
	using prev_t = member_registry< (Counter) - 1 >;  \
	static constexpr auto kMember  = sk::Reflection::cMemberFunction{ #Member, &class_type:: Member, sk::Reflection::cMember::eType:: Visibility }; \
	static constexpr auto kMembers = sk::cLinked_Array< const sk::Reflection::cMemberFunction* >{ &kMember, prev_t::kMembers }; };
			
// Internal macro. Do not use.
#define REGISTER_FUNCTION_DIRECT_0_( Member, Visibility ) public: \
	REGISTER_MEMBER_DIRECT_1_( Member, CONCAT( member_id_,__COUNTER__ ), Visibility )

// TODO: Add allow for extra reflection info for members.
// TODO: Add check so that a single member isn't registered multiple times.
#define SK_REGISTER_PRIVATE_MEMBER( Member, ... )   REGISTER_MEMBER_DIRECT_0_( Member, kPrivate   ) private:
#define SK_REGISTER_PROTECTED_MEMBER( Member, ... ) REGISTER_MEMBER_DIRECT_0_( Member, kProtected ) protected:
#define SK_REGISTER_PUBLIC_MEMBER( Member, ... )    REGISTER_MEMBER_DIRECT_0_( Member, kPublic    ) public:

#define SK_REGISTER_PRIVATE_FUNCTION( Member, ... )   REGISTER_MEMBER_DIRECT_0_( Member, kPrivate ) private:
#define SK_REGISTER_PROTECTED_FUNCTION( Member, ... ) REGISTER_MEMBER_DIRECT_0_( Member, kProtected ) protected:
#define SK_REGISTER_PUBLIC_FUNCTION( Member, ... )    REGISTER_MEMBER_DIRECT_0_( Member, kPublic ) public:

#define SK_REGISTER_PRIVATE_OVERLOADED_FUNCTION( Function, ... )   REGISTER_MEMBER_DIRECT_0_( Member, kPrivate ) private:
#define SK_REGISTER_PROTECTED_OVERLOADED_FUNCTION( Function, ... ) REGISTER_MEMBER_DIRECT_0_( Member, kProtected ) protected:
#define SK_REGISTER_PUBLIC_OVERLOADED_FUNCTION( Function, ... )    REGISTER_MEMBER_DIRECT_0_( Member, kPublic ) public:

#define REGISTER_CLASS( Class ) namespace Class { } 

SK_CLASS( Test )
{
	SK_CLASS_BODY( Test )

	uint64_t member_0 = 0; SK_REGISTER_PRIVATE_MEMBER( member_0 )
	uint32_t member_1 = 0; SK_REGISTER_PRIVATE_MEMBER( member_1 )
	uint32_t member_2 = 0; SK_REGISTER_PRIVATE_MEMBER( member_2 )

	static uint32_t member_3;
	SK_REGISTER_PRIVATE_MEMBER( member_3 )
	static constexpr uint32_t member_4 = 10;
	SK_REGISTER_PRIVATE_MEMBER( member_4 )
};
static constexpr auto  member_id_ = cTest::var_counter_t::next();
static constexpr auto& Test_members = cTest::member_registry< member_id_ - 1 >::kMembers;
