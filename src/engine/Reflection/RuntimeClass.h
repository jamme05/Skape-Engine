/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Misc/Hashing.h>

#include <Debugging/Debugging.h>
#include <Macros/Manipulation.h>

#include <Reflection/Types.h>
#include <Misc/Smart_Ptrs.h>

#include <print>
#include <functional>

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

	template< class Ty, class Pa = iClass, const get_parent_class_t< Pa >& Parent = get_class_ref< Pa >, bool ForceShared = true >
	requires std::is_base_of_v< iClass, Pa >
	class cRuntimeClass : public get_parent_class_t< Pa >
	{
	public:
		typedef Ty                       value_type;
		typedef get_parent_class_t< Pa > parent_type;

		// TODO: Move
		typedef typename get_parent_class< Pa >::inherits_type inherits_type;

		constexpr cRuntimeClass( const char* _name, const std::source_location& _location = std::source_location::current(), const uint64_t& _parent_hash = Parent.getType().getValue() )
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
	template< class Ty >
	concept sk_class = is_valid_class_v< Ty >;
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
	struct var_tag_{}; \
	struct func_tag_{}; \
	template< class > struct member_registry { \
		static constexpr auto kMembers = sk::cLinked_Array< sk::Reflection::member_var_pair_t >{}; }; \
	template< class > struct function_registry { \
		static constexpr auto kMembers = sk::cLinked_Array< sk::Reflection::member_func_pair_t >{}; }; \
	public: \
	using class_type = Test::class_type; \
	using var_counter_t  = const_counter< var_tag_ >;   \
	using func_counter_t  = const_counter< func_tag_ >; \
	template< int N > struct member_extractor : member_registry< std::integral_constant< int, N > >{};\
	template< int N > struct function_extractor : function_registry< std::integral_constant< int, N > >{};\

#define CREATE_MEMBER_REFLECTION_FUNCTIONS( RuntimeClass ) \
	public: \
	static constexpr auto getVariables() -> sk::Reflection::member_var_map_ref_t; \
	static constexpr auto getFunctions() -> sk::Reflection::member_func_map_ref_t; \
	/* static constexpr auto getConstructors() -> sk::Reflection::member_func_map_ref_t; WIP */ \
	static constexpr auto getVariable( const sk::str_hash& _hash ) -> sk::Reflection::member_var_t; \
	static constexpr auto getFunction( const sk::str_hash& _hash ) -> sk::Reflection::member_func_t; \
	static constexpr auto getFunctionOverloads( const sk::str_hash& _hash ) -> sk::Reflection::member_func_range_t; \
	static constexpr auto getFunction( const sk::str_hash& _hash, const sk::type_hash& _args ) -> sk::Reflection::member_func_t; \
	template< class... Args > static constexpr auto getFunction( const sk::str_hash& _hash ) -> sk::Reflection::member_func_t; \
	/* static constexpr auto getConstructor( const sk::str_hash& _hash ) -> sk::Reflection::cMemberFunction* WIP */; \
	auto getBoundVariable( const sk::str_hash& _hash ) -> std::optional< sk::Reflection::cMemberVariableInstance< class_type > >; \
	auto getBoundFunction( const sk::str_hash& _hash ) -> std::optional< sk::Reflection::cMemberFunctionInstance< class_type > >; \
	private:

// Internal use only.
#define CREATE_CLASS_IDENTIFIERS_0_( RuntimeClass ) public: \
	/* Prepare function to get information about which class it is. */ \
	CREATE_CLASS_IDENTITY_IDENTIFIERS( RuntimeClass ) \
	/* Prepare for member reflection: */ \
	CREATE_MEMBER_REFLECTION_VALUES( RuntimeClass ) \
	/* Create incomplete functions so that I remember and to allow all variables to be ready upon usage. */ \
	CREATE_MEMBER_REFLECTION_FUNCTIONS( RuntimeClass )

// Required to make a runtime class functional.
#define CREATE_CLASS_IDENTIFIERS( RuntimeClass ) \
	CREATE_CLASS_IDENTIFIERS_0_( RuntimeClass )


#define CREATE_CLASS_BODY( Class ) CREATE_CLASS_IDENTIFIERS( runtime_class_ ## Class )

#define CREATE_RUNTIME_CLASS_TYPE( Class, Name, ... ) sk::cRuntimeClass< Class __VA_OPT__(, FORWARD( __VA_ARGS__ ) ) >
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
// TODO: Add display name option.
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
		using class_type = ClassType; \
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
			enum eRaw : uint8_t
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
				kStatic      = 0x08,
				kVirtual     = 0x10,
				kConst       = 0x20,
				// Alias for Const
				kReadOnly    = 0x20,
				kConstructor = 0x40,
			};
			static constexpr auto getType      ( const uint8_t _type ){ return static_cast< eRaw >( kTypeMask & _type ); }
			static constexpr auto getVisibility( const uint8_t _type ){ return static_cast< eRaw >( kVisibility & _type ); }
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

		[[ nodiscard ]] constexpr auto getName() const { return m_name; }

		[[ nodiscard ]] constexpr auto getFlags     ( void ) const { return m_flags; }
		[[ nodiscard ]] constexpr auto getMemberType( void ) const { return eType::getType( m_flags ); }
		[[ nodiscard ]] constexpr auto getIsStatic  ( void ) const { return eType::getIsStatic( m_flags ); }
		[[ nodiscard ]] constexpr auto getIsVirtual ( void ) const { return eType::getIsVirtual( m_flags ); }
		[[ nodiscard ]] constexpr auto getIsReadOnly( void ) const { return eType::getIsReadOnly( m_flags ); }
		[[ nodiscard ]] constexpr auto getVisibility( void ) const { return eType::getVisibility( m_flags ); }

	private:
		const char* m_name;
		uint8_t     m_flags;
	};

	template< class Ty >
	class cMemberVariableInstance;
	template< class Ty >
	class cMemberFunctionInstance;

	class iHolder
	{
	public:
		virtual ~iHolder() = default;
		virtual constexpr uint8_t getFlags() = 0;
	};

	class iMemberVariableHolder
	{
	protected:
		virtual ~iMemberVariableHolder() = default;
		// Getters return nullptr on fail, whilst setters return false on fail.
		[[ nodiscard ]] virtual const void* unsafe_get() const = 0;
		[[ nodiscard ]] virtual bool  unsafe_set( void* _src ) const = 0;
		[[ nodiscard ]] virtual const void* unsafe_get( void* _inst ) const = 0;
		[[ nodiscard ]] virtual bool  unsafe_set( void* _inst, void* _src ) const = 0;

		friend class cMemberVariable;
		template< class Ty >
		friend class cMemberVariableInstance;
	};

	// Non-static Member variable container
	template< class Ty, class Me >
	class cMemberVariableHolder final : public iMemberVariableHolder
	{
	public:
		explicit consteval cMemberVariableHolder( Me Ty::* _member )
		: m_member_( _member )
		{}

		using enum cMember::eType::eRaw;
		static constexpr uint8_t kFlags = kVariable | std::is_const_v< Me > * kConst;

	private:
		Me Ty::* m_member_;
	protected:

		// Unsupported. Requires class instance.
		const void* unsafe_get() const override
		{
			return nullptr;
		}

		// Unsupported. Requires class instance.
		bool unsafe_set( void* _src ) const override { return false; }

		const void* unsafe_get( void* _inst ) const override
		{
			if( _inst == nullptr )
				return nullptr;

			Ty*    inst = static_cast< Ty* >( _inst );
			auto   var  = &( inst->*m_member_ );
			return var;
		}

		bool unsafe_set( void* _inst, void* _src ) const override
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

		using enum cMember::eType::eRaw;
		static constexpr uint8_t kFlags = kVariable | kStatic | std::is_const_v< Ty > * kConst;

	protected:
		[[ nodiscard ]] const void* unsafe_get() const override
		{
			return m_value_;
		}
		bool unsafe_set( void* _src ) const override
		{
			if constexpr( !std::is_const_v< Ty > )
			{
				*m_value_ = *static_cast< Ty* >( _src );
				return true;
			}
			return false;
		}

		// Static doesn't require instance so simply use the static getter/setter.
		[[ nodiscard ]] const void* unsafe_get( void* ) const override { return unsafe_get(); }
		[[ nodiscard ]] bool unsafe_set( void*, void* _src ) const override { return unsafe_set( _src ); }
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
		constexpr cMemberVariable( const char* _name, const cMemberVariableHolder< Ty, Me >& _holder, const uint8_t& _extras )
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

		template< sk_class Ty >
		constexpr auto bind( Ty* _instance ) -> cMemberVariableInstance< Ty >;

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
		virtual const void* get_() const { return m_holder_->unsafe_get(); }
		virtual bool set_( void* _src ) const { return m_holder_->unsafe_set( _src ); }
	};

	template< class Ty >
	Ty* cMemberVariable::get() const
	{
		constexpr static auto& other_type = get_type_info< Ty >::kInfo;
		if( *m_member_type_info_ != other_type )
		{
			// TODO: Add error.
			std::println( "Member {} is of type {} and not type {} which was provided.", getName(), m_member_type_info_->raw_name, other_type.raw_name );
			return nullptr;
		}
		return static_cast< Ty* >( get_() );
	} // get

	template< class Ty >
	bool cMemberVariable::set( Ty* _src ) const
	{
		if( getIsReadOnly() )
		{
			std::println( "Member {} is read-only.", getName() );
			return false;
		}
		constexpr static auto& other_type = get_type_info< Ty >::kInfo;
		if( *m_member_type_info_ != other_type )
		{
			// TODO: Add error.
			std::println( "Error: Member {} is of type {} and not type {} which was provided.", getName(), m_member_type_info_->raw_name, other_type.raw_name );
			return false;
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
			std::println( "Error: Tried accessing value through class {} when it required class {}.", Ty::getClassName(), m_class_->getName() );
			return nullptr;
		}
		constexpr static auto& other_type = get_type_info< Ty >::kInfo;
		if( *m_member_type_info_ != other_type )
		{
			// TODO: Add error.
			std::println( "Error: Member {} is of type {} and not type {} which was provided.", getName(), m_member_type_info_->raw_name, other_type.raw_name );
			return nullptr;
		}
		return static_cast< Me* >( m_holder_->unsafe_get( _inst ) );
	} // get

	template< class Ty = iClass, class Me > requires std::is_base_of_v< iClass, Ty >
	bool cMemberVariable::set( Ty* _inst, Me* _src ) const
	{
		if( getIsStatic() )
		{
			std::println( "Warning: Accessing static value through instance." );
			return set< Me >( _src );
		}
		if( *m_class_ != Ty::kClass )
		{
			// TODO: Support a class hierarchy. Class.AllMyMembers = Class.MyMembers + ParentClass.MyMembers
			// TODO: Add error as well.
			std::println( "Error: Tried accessing value through class {} when it required class {}.", Ty::getClassName(), m_class_->getName() );
			return false;
		}
		constexpr static auto& other_type = get_type_info< Ty >::kInfo;
		if( *m_member_type_info_ != other_type )
		{
			// TODO: Add error.
			std::println( "Error: Member {} is of type {} and not type {} which was provided.", getName(), m_member_type_info_->raw_name, other_type.raw_name );
			return false;
		}
		return m_holder_->unsafe_set( _inst, _src );
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
		[[ nodiscard ]] const void* get_() const override
		{
			return m_holder_->unsafe_get( m_instance_ );
		}
		bool set_( void* _src ) const override
		{
			return m_holder_->unsafe_set( m_instance_, _src );
		}

	private:
		Ty* m_instance_;
	};

	template< class Ty, class Me >
	cMemberVariableHolder( Me Ty::* ) -> cMemberVariableHolder< Ty, Me >;
	template< class Ty >
	cMemberVariableHolder( Ty* ) -> cMemberVariableHolder< void, Ty >;

	// Declare bind here to have access to the type of instance.
	template< sk_class Ty = iClass >
	constexpr auto cMemberVariable::bind( Ty* _instance )->cMemberVariableInstance< Ty >
	{
		if( *m_class_ != Ty::kClass )
		{
			// TODO: Support a class hierarchy. Class.AllMyMembers = Class.MyMembers + ParentClass.MyMembers
			// TODO: Add error as well.
			std::println( "Error: Tried accessing value through class {} when it required class {}.", Ty::getClassName(), m_class_->getName() );
			return nullptr;
		}
		return cMemberVariableInstance< Ty >{ *this, _instance };
	}

	class iMemberFunctionHolder
	{
	protected:
		virtual ~iMemberFunctionHolder() = default;
		// Static/Instance call
		virtual void unsafe_invoke( void* _input, void* _return ) const = 0;
		// Targeted Instance call
		virtual void unsafe_invoke( void* _instance, void* _input, void* _return ) const = 0;

		friend class cMemberFunction;
		template< class Ty >
		friend class cMemberFunctionInstance;
	};

	// Normal member function holder
	template< class Ty, class Re, class... Args >
	class cMemberFunctionHolder : public iMemberFunctionHolder
	{
		typedef Re( Ty::*func_t )( Args... );
		typedef std::tuple< Args... > tuple_t;
		func_t m_func_;
	public:
		explicit consteval cMemberFunctionHolder( const func_t _func )
		: m_func_( _func )
		{}

		using enum cMember::eType::eRaw;
		static constexpr uint8_t kFlags = kFunction;

	protected:
		// Static member function.
		void unsafe_invoke( void*, void* ) const override
		{
			// TODO: Throw error as this shouldn't happen.
		}
		// Instanced member function
		void unsafe_invoke( void* _instance, void* _input, void* _return ) const override
		{
			Ty* inst = static_cast< Ty* >( _instance );
			if constexpr( !std::is_same_v< Re, void > )
			{
				if( _return )
				{
					auto ret = static_cast< Re* >( _return );
					*ret = std::apply( std::bind_front( m_func_, inst ), *static_cast< tuple_t* >( _return ) );
					return;
				}
			}

			// Fallback to no return.
			std::apply( std::bind_front( m_func_, inst ), *static_cast< tuple_t* >( _return ) );
		}
	};

	// Const member function holder
	template< class Ty, class Re, class... Args >
	class cMemberFunctionHolder< const Ty, Re, Args... > : public iMemberFunctionHolder
	{
		typedef std::remove_const_t< Ty > class_t;
		typedef Re( class_t::*func_t )( Args... ) const;
		typedef std::tuple< Args... > tuple_t;
		func_t m_func_;
	public:
		explicit consteval cMemberFunctionHolder( const func_t _func )
		: m_func_( _func )
		{}

		using enum cMember::eType::eRaw;
		static constexpr uint8_t kFlags = kFunction | kConst;

	protected:
		void unsafe_invoke( void*, void* ) const override
		{
			// TODO: Throw error as this shouldn't happen.
		}
		// Instanced member function
		void unsafe_invoke( void* _instance, void* _input, void* _return ) const override
		{
			Ty* inst = static_cast< Ty* >( _instance );
			if constexpr( !std::is_same_v< Re, void > )
			{
				if( _return )
				{
					auto ret = static_cast< Re* >( _return );
					*ret = std::apply( std::bind_front( m_func_, inst ), *static_cast< tuple_t* >( _return ) );
					return;
				}
			}

			// Fallback to no return.
			std::apply( std::bind_front( m_func_, inst ), *static_cast< tuple_t* >( _return ) );
		}
	};

	// Static member function holder
	template< class Re, class... Args >
	class cMemberFunctionHolder< void, Re, Args... > : public iMemberFunctionHolder
	{
		typedef Re( *func_t )( Args... );
		typedef std::tuple< Args... > tuple_t;
		func_t m_func_;

	public:
		explicit consteval cMemberFunctionHolder( const func_t _func )
		: m_func_( _func )
		{}

		using enum cMember::eType::eRaw;
		static constexpr uint8_t kFlags = kFunction | kStatic;

	protected:
		void unsafe_invoke( void* _input, void* _return ) const override
		{
			if constexpr( !std::is_same_v< Re, void > )
			{
				if( _return )
				{
					auto ret = static_cast< Re* >( _return );
					*ret = std::apply( std::bind_front( m_func_ ), *static_cast< tuple_t* >( _return ) );
					return;
				}
			}

			// Fallback to no return.
			std::apply( std::bind_front( m_func_ ), *static_cast< tuple_t* >( _return ) );
		}
		// Instanced member function
		void unsafe_invoke( void*, void* _input, void* _return ) const override
		{
			// Simply ignore instance if static.
			unsafe_invoke( _input, _return );
		}
	};

	// Normal member function
	template< class Ty, class Re, class... Args >
	cMemberFunctionHolder( const Re( Ty::* )( Args... ) ) -> cMemberFunctionHolder< Ty, Re, Args... >;
	// Const member function
	template< class Ty, class Re, class... Args >
	cMemberFunctionHolder( Re( Ty::* )( Args... ) const ) -> cMemberFunctionHolder< const Ty, Re, Args... >;
	// Static member function
	template< class Re, class... Args >
	cMemberFunctionHolder( Re( * )( Args... ) ) -> cMemberFunctionHolder< void, Re, Args... >;

	class cMemberFunction : public cMember
	{
	protected:
		typedef const iMemberFunctionHolder* holder_t;
		typedef const iRuntimeClass* class_t;
		type_info_t m_return_type_;
		args_info_t m_args_info_;
		class_t     m_class_;
		holder_t    m_holder_;
	public:
		// TODO: Declare function hash.
		// Non-Static
		template< class Ty = iClass, class Re, class... Args >
		constexpr cMemberFunction( const char* _name, const cMemberFunctionHolder< Ty, Re, Args... >& _holder, const uint8_t& _extras )
		: cMember( _name, _extras | _holder.kFlags )
		, m_return_type_( &get_type_info< Re >::kInfo )
		, m_args_info_( &args_hash< Args... >::kInfo )
		, m_class_( &Ty::kClass )
		, m_holder_( &_holder )
		{}

		// Static
		template< class Re, class... Args >
		consteval cMemberFunction( const char* _name, const cMemberFunctionHolder< void, Re, Args... >& _holder, const uint8_t& _extras )
		: cMember( _name, _extras | _holder.kFlags )
		, m_return_type_( &get_type_info< Re >::kInfo )
		, m_args_info_( &args_hash< Args... >::kInfo )
		, m_class_( nullptr )
		, m_holder_( &_holder )
		{}

		// Call static or Instance function. Check out calli in case an instance needs to be provided.
		template< class Re, class... Args >
		bool call( Re* _return, Args&&... _args ) const
		{
			// Verify args.
			typedef args_hash< Args... > args_t;
			SK_WARN_RET_IF( args_t::kHash != m_args_info_->hash,
				FORMAT( "Error: Wrong types provided! Provided types: {}. Required types: {}.", args_t::kInfo.to_string(), m_args_info_->to_string() ),
				false )

			// Verify return type. But only if it's required.
			constexpr static auto& other_type = get_type_info< Re >::kInfo;
			SK_WARN_RET_IF( _return != nullptr && *m_return_type_ != other_type,
				FORMAT( "Error: Wrong return type provided. Provided type: {}. Required type: {}.", other_type.raw_name, m_return_type_->raw_name ),
				false )

			// Prepare args.
			auto args = std::forward_as_tuple( std::forward< Args >( _args )... );

			return call_( nullptr, _return, &args );
		}

		// Call function on specific instance.
		template< sk_class Ty = iClass, class Re, class... Args >
		bool calli( Ty& _instance, Re* _return, Args&&... _args ) const
		{
			// Static check.
			SK_WARN_RET_IF( getIsStatic(),
				TEXT( "Warning: Calling static function through instance." ),
				call< Re, Args... >( _return, std::forward< Args >( _args )... ) )

			// Const check. If getting called by a const ptr.
			if constexpr( !std::is_const_v< Ty > )
				SK_WARN_RET_IF( !getIsReadOnly(), TEXT( "Error: Calling a non-const function with a constant instance." ), false )

			// Verify class.
			SK_WARN_RET_IF( Ty::kClass != *m_class_,
				FORMAT( "Error: Tried calling function through class {} when it required class {}.", Ty::getClassName(), m_class_->getName() ),
				false )

			// Verify args.
			typedef args_hash< Args... > args_t;
			SK_WARN_RET_IF( args_t::kHash != m_args_info_->hash,
				FORMAT( "Error: Wrong types provided! Provided types: {}. Required types: {}.", args_t::kInfo.to_string(), m_args_info_->to_string() ),
				false )

			// Verify return type. But only if it's required.
			constexpr static auto& other_type = get_type_info< Re >::kInfo;
			SK_WARN_RET_IF( _return != nullptr && *m_return_type_ != other_type,
				FORMAT( "Error: Wrong return type provided. Provided type: {}. Required type: {}.", other_type.raw_name, m_return_type_->raw_name ),
				false )

			// Add member to front of tuple.
			auto args = std::forward_as_tuple( std::forward< Args >( _args )... );

			// Const cast the instance to allow its conversion to a void*
			return call_( const_cast< std::remove_const_t< Ty > >( &_instance ), _return, &args );
		}

		template< sk_class Ty = iClass >
		constexpr auto bind( Ty& _instance ) const -> std::optional< cMemberFunctionInstance< Ty > >;

		constexpr bool hasArgs( const type_hash& _args_hash ) const noexcept
		{
			return m_args_info_->hash == _args_hash;
		} // hasArgs
		template< class... Args >
		constexpr bool hasArgs() const noexcept
		{
			return hasArgs( args_hash< Args... >::kHash );
		} // hasArgs

	protected:
		virtual bool call_( void* _instance, void* _return, void* _args ) const
		{
			SK_WARN_RET_IF( _instance == nullptr && !getIsStatic(),
				TEXT( "Error: Calling non-static member function without instance." ),
				false )

			if( _instance )
				m_holder_->unsafe_invoke( _instance, _args, _return );
			else
				m_holder_->unsafe_invoke( _args, _return );
			return true;
		}
	};

	template< class Ty >
	class cMemberFunctionInstance : public cMemberFunction
	{
		Ty* m_instance_;
	public:
		cMemberFunctionInstance( const cMemberFunction& _function, Ty* _instance )
		: cMemberFunction( _function )
		, m_instance_( _instance )
		{}

		bool call_( void* _instance, void* _return, void* _args ) const override
		{
			if( _instance )
				m_holder_->unsafe_invoke( _instance, _args, _return );
			else
				m_holder_->unsafe_invoke( m_instance_, _args, _return );

			return true;
		}
	};

	template< sk_class Ty >
	constexpr auto cMemberFunction::bind( Ty& _instance ) const -> std::optional< cMemberFunctionInstance< Ty > >
	{
		// Verify class.
		if( Ty::kClass != *m_class_ )
		{
			std::println( "Error: Tried binding function to class {} when it required class {}.", Ty::getClassName(), m_class_->getName() );
			return {};
		}
		return cMemberFunctionInstance< Ty >( static_cast< class_t >( &_instance ) );
	}

	typedef const cMemberVariable*               member_var_t;
	typedef const cMemberFunction*               member_func_t;
	typedef std::pair< str_hash, member_var_t >  member_var_pair_t;
	typedef std::pair< str_hash, member_func_t > member_func_pair_t;
	typedef map_ref  < str_hash, member_var_t >  member_var_map_ref_t;
	typedef map_ref  < str_hash, member_func_t > member_func_map_ref_t;
	typedef std::pair< const member_var_pair_t*, const member_var_pair_t* > member_var_range_t;
	typedef std::pair< const member_func_pair_t*, const member_func_pair_t* > member_func_range_t;
} // sk::Reflection::

// Internal macro. Do not use.
#define REGISTER_MEMBER_DIRECT_1_( Member, Counter, Visibility ) \
	static constexpr auto Counter = var_counter_t::next(); \
	template< class Ty > struct member_registry< std::integral_constant< Ty, Counter > > { \
	using enum sk::Reflection::cMember::eType::eRaw; \
	using prev_t = member_registry< std::integral_constant< Ty, (Counter) -1 > >; \
	using pair_t = sk::Reflection::member_var_pair_t; \
	static constexpr auto kMemberHolder = sk::Reflection::cMemberVariableHolder{ &class_type:: Member }; \
	static constexpr auto kMember       = sk::Reflection::cMemberVariable{ #Member, kMemberHolder, Visibility }; \
	static constexpr auto kMembers      = sk::cLinked_Array< pair_t >{ pair_t{ #Member, &kMember }, prev_t::kMembers }; };
			
// Internal macro. Do not use.
#define REGISTER_MEMBER_DIRECT_0_( Member, Visibility ) public: \
	REGISTER_MEMBER_DIRECT_1_( Member, CONCAT( member_id_,__COUNTER__ ), Visibility )

// Internal macro. Do not use.
#define REGISTER_FUNCTION_DIRECT_1_( Member, Counter, Visibility ) \
	static constexpr auto Counter = func_counter_t::next(); \
	public: \
	template< class Ty > struct function_registry< std::integral_constant< Ty, Counter > > { \
	using enum sk::Reflection::cMember::eType::eRaw; \
	using prev_t = function_registry< std::integral_constant< Ty, (Counter) - 1 > >; \
	using pair_t = sk::Reflection::member_func_pair_t; \
	static constexpr auto kMemberHolder = sk::Reflection::cMemberFunctionHolder{ &class_type:: Member }; \
	static constexpr auto kMember       = sk::Reflection::cMemberFunction{ #Member, kMemberHolder, Visibility }; \
	static constexpr auto kMembers      = sk::cLinked_Array< pair_t >{ pair_t{ #Member, &kMember }, prev_t::kMembers }; };
			
// Internal macro. Do not use.
#define REGISTER_FUNCTION_DIRECT_0_( Member, Visibility ) private: \
	REGISTER_FUNCTION_DIRECT_1_( Member, CONCAT( member_id_,__COUNTER__ ), Visibility )

// TODO: Add allow for extra reflection info for members.
// TODO: Add check so that a single member isn't registered multiple times.
#define SK_PRIVATE_MEMBER( Member, ... )   REGISTER_MEMBER_DIRECT_0_( Member, kPrivate   ) private:
#define SK_PROTECTED_MEMBER( Member, ... ) REGISTER_MEMBER_DIRECT_0_( Member, kProtected ) protected:
#define SK_PUBLIC_MEMBER( Member, ... )    REGISTER_MEMBER_DIRECT_0_( Member, kPublic    ) public:

#define SK_PRIVATE_FUNCTION( Function, ... )   REGISTER_FUNCTION_DIRECT_0_( Function, kPrivate ) private:
#define SK_PROTECTED_FUNCTION( Function, ... ) REGISTER_FUNCTION_DIRECT_0_( Function, kProtected ) protected:
#define SK_PUBLIC_FUNCTION( Function, ... )    REGISTER_FUNCTION_DIRECT_0_( Function, kPublic ) public:

// WIP, Currently not functional
#define SK_PRIVATE_OVERLOADED_FUNCTION( Function, ... )   REGISTER_FUNCTION_DIRECT_0_( Function, kPrivate ) private:
// WIP, Currently not functional
#define SK_PROTECTED_OVERLOADED_FUNCTION( Function, ... ) REGISTER_FUNCTION_DIRECT_0_( Function, kProtected ) protected:
// WIP, Currently not functional
#define SK_PUBLIC_OVERLOADED_FUNCTION( Function, ... )    REGISTER_FUNCTION_DIRECT_0_( Function, kPublic ) public:

#define SK_CONSTRUCTOR( Function, ... )
#define SK_OVERLOADED_CONSTRUCTOR( Function, ... )

// Add final class requirements and register it as a type in the global namespace.

#define BUILD_CLASS_MEMBER_EXTRACTION_0_( Class, Counter ) namespace Class { namespace { \
	static constexpr auto CONCAT( func_, Counter ) = class_type::func_counter_t::next() - 1; \
	static constexpr auto CONCAT( var_, Counter ) = class_type::var_counter_t::next() - 1; \
	} }

#define BUILD_CLASS_MEMBER_EXTRACTION( Class ) \
	BUILD_CLASS_MEMBER_EXTRACTION_0_( Class, CONCAT( counter_, __COUNTER__ ) )

// Member variable getters.
#define BUILD_CLASS_GET_VARIABLES( Class ) \
	constexpr auto Class ::class_type::getVariables() -> sk::map_ref< sk::str_hash, sk::Reflection::cMemberVariable* >{ \
	} /* TODO: Add find logic (use const map?) */

#define BUILD_CLASS_VARIABLE_GETTER( Class ) \
	constexpr auto Class ::class_type::getVariable( const sk::str_hash& _hash ) -> sk::Reflection::cMemberVariable*{ \
	} /* TODO: Add find logic (use const map?) */

#define BUILD_CLASS_BOUND_VARIABLE_GETTER( Class ) \
	inline auto Class ::class_type::getBoundVariable( const sk::str_hash& _hash ) -> sk::Reflection::cMemberVariableInstance< class_type >{ \
	} /* TODO: Add find logic (use const map?) */

// Member function getters
#define BUILD_CLASS_GET_FUNCTIONS( Class ) \
	constexpr auto Class ::class_type::getFunctions() -> sk::map_ref< sk::str_hash, sk::Reflection::cMemberFunction* > { \
	} /* TODO: Add function reflection, and then work more on this. */

#define BUILD_CLASS_FUNCTION_GETTER( Class ) \
	constexpr auto Class ::class_type::getFunction( const sk::str_hash& _hash ) -> sk::Reflection::cMemberFunction*{ \
	} /* TODO: Add function reflection, and then work more on this. */

#define BUILD_CLASS_FUNCTION_OVERLOADS_GETTER( Class ) \
	constexpr auto Class ::class_type::getFunctionOverloads( const sk::str_hash& _hash ) \
	-> std::pair< sk::Reflection::member_func_pair_t*, sk::Reflection::member_func_pair_t* >{ \
	} /* TODO: Add function reflection, and then work more on this. */

#define BUILD_CLASS_FUNCTION_OVERLOAD_RAW_GETTER( Class ) \
	constexpr auto Class ::class_type::getFunction( const sk::str_hash& _hash, const sk::type_hash& _args ) -> sk::Reflection::cMemberFunction*{ \
	} /* TODO: Add function reflection, and then work more on this. */

#define BUILD_CLASS_FUNCTION_OVERLOAD_TEMPLATE_GETTER( Class ) \
	template< class... Args > \
	constexpr auto Class ::class_type::getFunction( const sk::str_hash& _hash ) -> sk::Reflection::cMemberFunction*{ \
	return getFunction( _hash, sk::args_hash< Args... >::kHash ); } /* TODO: Add function reflection, and then work more on this. */

#define BUILD_CLASS_BOUND_FUNCTION_GETTER( Class ) \
	inline auto Class ::class_type::getBoundFunction( const sk::str_hash& _hash ) -> sk::Reflection::cMemberFunctionInstance< class_type >{ \
	} /* TODO: Add function reflection, and then work more on this. */

// Build type_info
#define BUILD_CLASS_REFLECTION_INFO( Class ) \
	template<> struct sk::get_type_info< Class ::class_type >{ \
	constexpr static auto& kClass = Class ::class_type::kClass; \
	constexpr static sType_Info kInfo = { \
		.type = sType_Info::eType::kClass, \
		.hash = kClass.getType(), \
		.size = sizeof( Class ::class_type ), \
		.name = kClass.getRawName(), \
		.raw_name = kClass.getRawName() }; \
	constexpr static bool kValid = true; };

/**
 * HAS to be used in the global namespace (aka not within any namespace)
 * @param Class Namespace and name of class.
 */
#define REGISTER_CLASS( Class ) \
	BUILD_CLASS_MEMBER_EXTRACTION( Class ) \
	/* Build Member variable getters */ \
	BUILD_CLASS_GET_VARIABLES( Class ) \
	BUILD_CLASS_VARIABLE_GETTER( Class ) \
	BUILD_CLASS_BOUND_VARIABLE_GETTER( Class ) \
	/* Build Member variable getters */ \
	BUILD_CLASS_GET_FUNCTIONS( Class ) \
	BUILD_CLASS_FUNCTION_GETTER( Class ) \
	BUILD_CLASS_FUNCTION_OVERLOADS_GETTER( Class ) \
	BUILD_CLASS_FUNCTION_OVERLOAD_RAW_GETTER( Class ) \
	BUILD_CLASS_FUNCTION_OVERLOAD_TEMPLATE_GETTER( Class ) \
	BUILD_CLASS_BOUND_FUNCTION_GETTER( Class ) \
	/* Build Reflection and register */ \
	BUILD_CLASS_REFLECTION_INFO( Class ) \
	REGISTER_TYPE_INTERNAL( M_CLASS( Class ) )

SK_CLASS( Test )
{
	SK_CLASS_BODY( Test )

	uint64_t member_0 = 0; SK_PRIVATE_MEMBER( member_0 )
	uint32_t member_1 = 0; SK_PRIVATE_MEMBER( member_1 )
	uint32_t member_2 = 0; SK_PRIVATE_MEMBER( member_2 )

public:
	static uint32_t member_3; SK_PUBLIC_MEMBER( member_3 )
	static constexpr uint32_t member_4 = 10; SK_PUBLIC_MEMBER( member_4 )

	cTest( uint64_t _mem0, uint32_t _mem1 )
	: member_0( _mem0 ), member_1( _mem1 )
	{
		// Look at std::apply and std::make_index_sequence and std::index_sequence to succeed with construction reflection.
	}
	SK_CONSTRUCTOR( cTest )

	void test_func( const uint32_t _val ) { member_1 = _val; }
	uint32_t test_func2( void ) const { return member_2; }
	static void test_func3( const uint32_t _val ){ member_3 = _val; }
	// TODO: Figure out how to handle virtual functions? Some custom virtual table or member functions?
	virtual uint8_t test_func4( void ) const { return member_2; }

	// TODO: Figure out a way to register virtual functions. Overridable tag?
	SK_PUBLIC_FUNCTION( test_func )
	SK_PUBLIC_FUNCTION( test_func2 )
	SK_PUBLIC_FUNCTION( test_func3 )
};

namespace Test {
	namespace Internal
	{
		// Register functions:
		constexpr auto  func_counter_680239913 = class_type::func_counter_t::next();
		constexpr auto& last_func_member       = class_type::function_extractor< func_counter_680239913 - 1 >::kMembers;
		using func_t = sk::Reflection::member_func_pair_t;
		using func_map_t = sk::const_map< func_t::first_type, func_t::second_type, last_func_member.size() >;

		// Register variables:
		constexpr auto  var_counter_680239913 = class_type::var_counter_t::next();
		constexpr auto& last_var_member       = class_type::member_extractor< var_counter_680239913 - 1 >::kMembers;
		using var_t  = sk::Reflection::member_var_pair_t;
		using var_map_t = sk::const_map< var_t::first_type, var_t::second_type, last_var_member.size() >;
	}
	static constexpr auto kFunctions = Internal::func_map_t{ Internal::last_func_member.begin(), Internal::last_func_member.end() };
	static constexpr auto kVariables = Internal::var_map_t { Internal::last_var_member .begin(), Internal::last_var_member .end() };
}
constexpr auto Test ::class_type::getVariables() -> sk::Reflection::member_var_map_ref_t{
	return Test ::kVariables;
} constexpr auto Test ::class_type::getVariable( const sk::str_hash& _hash ) -> sk::Reflection::member_var_t{
	const auto itr = Test ::kVariables.find( _hash ); return itr == nullptr ? nullptr : itr->second;
}constexpr auto Test ::class_type::getFunctions() -> sk::Reflection::member_func_map_ref_t{
	return Test ::kFunctions;
} constexpr auto Test ::class_type::getFunction( const sk::str_hash& _hash ) -> sk::Reflection::member_func_t{
	const auto itr = Test ::kFunctions.find( _hash ); return itr == nullptr ? nullptr : itr->second;
} constexpr auto Test ::class_type::getFunctionOverloads( const sk::str_hash& _hash ) -> sk::Reflection::member_func_range_t{
	return Test ::kFunctions.range( _hash );
} constexpr auto Test ::class_type::getFunction( const sk::str_hash& _hash, const sk::type_hash& _args ) -> sk::Reflection::member_func_t{
	for( auto [ fst, lst ] = Test ::kFunctions.range( _hash ); fst != lst; ++fst )
	{
		if( fst->second->hasArgs( _args ) )
			return fst->second;
	}
	return nullptr;
} template< class... Args > constexpr auto Test ::class_type::getFunction( const sk::str_hash& _hash ) -> sk::Reflection::member_func_t{
	return getFunction( _hash, sk::args_hash< Args... >::kHash );
} inline auto Test ::class_type::getBoundVariable( const sk::str_hash& _hash ) -> std::optional< sk::Reflection::cMemberVariableInstance< class_type > >{
	if( const auto member = getVariable( _hash ); member == nullptr )
		return {};
	else
		return sk::Reflection::cMemberVariableInstance{ *member, this };
} inline auto Test ::class_type::getBoundFunction( const sk::str_hash& _hash ) -> std::optional< sk::Reflection::cMemberFunctionInstance< class_type > >{
	if( const auto member = getFunction( _hash ); member == nullptr )
		return {};
	else
		return sk::Reflection::cMemberFunctionInstance{ *member, this };
} template<> struct sk::get_type_info< Test ::class_type >{ constexpr static auto& kClass = Test ::class_type::kClass; constexpr static sType_Info kInfo = { .type = sType_Info::eType::kClass, .hash = kClass.getType(), .size = sizeof( Test ::class_type ), .name = kClass.getRawName(), .raw_name = kClass.getRawName() }; constexpr static bool kValid = true; }; constexpr static auto type_registry_680239914 = sk::registry::counter::next(); template<> struct sk::registry::type_registry< type_registry_680239914 >{ typedef type_registry< type_registry_680239914 - 1 > previous_t; constexpr static auto registered = cLinked_Array{ static_cast< const sk::sType_Info* >( &get_type_info< cTest >::kInfo ), previous_t::registered }; constexpr static bool valid = true; };
// REGISTER_CLASS( Test )
