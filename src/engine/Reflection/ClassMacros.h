
#pragma once

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

#define CREATE_MEMBER_REFLECTION_VALUES( ClassName, RuntimeClass ) \
	using class_type = runtime_class_type::value_type; \
	private: \
	struct var_tag_{}; \
	struct func_tag_{}; \
	using access_point = sk::Reflection::access_point< class_type >; \
	public: \
	template< class > struct _xxx_sk_access{ \
		static constexpr auto kAccess = access_point{ .m_access = sk::Reflection::cMember::eType::kNone, .m_point = nullptr }; }; \
	static constexpr auto _xxx_sk_access_counter_0 = access_point::counter_t::next(); \
	template< class Ty > struct _xxx_sk_access< std::integral_constant< Ty, _xxx_sk_access_counter_0 > >{ \
		static constexpr auto kAccess = access_point{ \
			.m_access = sk::Reflection::cMember::eType::kPrivate, .m_point = nullptr }; }; \
	[[ msvc::no_unique_address, maybe_unused ]] access_point::point _xxx_sk_point_placeholder; \
	template< class > struct variable_registry { \
		static constexpr auto kMemberHolder = sk::Reflection::cMemberVariableHolder{ &class_type::_xxx_sk_point_placeholder };\
		static constexpr auto kMember = sk::Reflection::sPartial_Member_Variable< class_type, decltype( kMemberHolder ) >{ \
			.name = nullptr, .holder = &kMemberHolder }; }; \
	template< class > struct function_registry { \
		static void _xxx_sk_placeholder_function(){} \
		static constexpr auto kMemberHolder = sk::Reflection::cMemberFunctionHolder{ &_xxx_sk_placeholder_function };\
		static constexpr auto kMember = sk::Reflection::sPartial_Member_Function< class_type, decltype( kMemberHolder ) >{ \
			.name = nullptr, .holder = &kMemberHolder, .point = &class_type::_xxx_sk_point_placeholder }; }; \
	using var_counter_t  = const_counter< var_tag_ >;   \
	using func_counter_t  = const_counter< func_tag_ >; \
	template< int N > struct variable_extractor : variable_registry< std::integral_constant< int, N > >{};\
	template< int N > struct function_extractor : function_registry< std::integral_constant< int, N > >{};\

#define CREATE_MEMBER_REFLECTION_FUNCTIONS( RuntimeClass ) \
	/* static constexpr auto getConstructors() -> sk::Reflection::member_func_map_ref_t; WIP */ \
	auto getVariables() const -> sk::Reflection::member_var_map_ref_t override; \
	auto getFunctions() const -> sk::Reflection::member_func_map_ref_t override; \
	auto getVariable( const sk::str_hash& _hash ) const -> sk::Reflection::member_var_ptr_t override; \
	auto getFunction( const sk::str_hash& _hash ) const -> sk::Reflection::member_func_ptr_t override;\
	auto getFunctionOverloads( const sk::str_hash& _hash ) const -> sk::Reflection::member_func_range_t override; \
	auto getFunction( const sk::str_hash& _hash, const sk::type_hash& _args ) const -> sk::Reflection::member_func_ptr_t override; \
	template< class Ty, class... Args > auto getFunction( const sk::str_hash& _hash ) -> sk::Reflection::member_func_ptr_t; \
	/* static constexpr auto staticGetConstructors() -> sk::Reflection::member_func_map_ref_t; WIP */ \
	static constexpr auto staticGetVariables() -> sk::Reflection::member_var_map_ref_t; \
	static constexpr auto staticGetFunctions() -> sk::Reflection::member_func_map_ref_t; \
	static constexpr auto staticGetVariable( const sk::str_hash& _hash ) -> sk::Reflection::member_var_ptr_t; \
	static constexpr auto staticGetFunction( const sk::str_hash& _hash ) -> sk::Reflection::member_func_ptr_t; \
	static constexpr auto staticGetFunctionOverloads( const sk::str_hash& _hash ) -> sk::Reflection::member_func_range_t; \
	static constexpr auto staticGetFunction( const sk::str_hash& _hash, const sk::type_hash& _args ) -> sk::Reflection::member_func_ptr_t; \
	template< class Ty, class... Args > static constexpr auto staticGetFunction( const sk::str_hash& _hash ) -> sk::Reflection::member_func_ptr_t; \
	auto getBoundVariable( const sk::str_hash& _hash ) -> std::optional< sk::Reflection::cMemberVariableInstance< class_type > >; \
	auto getBoundFunction( const sk::str_hash& _hash ) -> std::optional< sk::Reflection::cMemberFunctionInstance< class_type > >; \

// Internal use only.
#define CREATE_CLASS_IDENTIFIERS_0_( ClassName, RuntimeClass ) public: \
	/* Prepare function to get information about which class it is. */ \
	CREATE_CLASS_IDENTITY_IDENTIFIERS( RuntimeClass ) \
	/* Prepare for member reflection: */ \
	CREATE_MEMBER_REFLECTION_VALUES( ClassName, RuntimeClass ) \
	/* Create incomplete functions so that I remember and to allow all variables to be ready upon usage. */ \
	CREATE_MEMBER_REFLECTION_FUNCTIONS( RuntimeClass ) \
	sk_private: /* Set the first access as private. */

// Required to make a runtime class functional.
#define CREATE_CLASS_IDENTIFIERS( ClassName, RuntimeClass ) \
	CREATE_CLASS_IDENTIFIERS_0_( ClassName, RuntimeClass )


#define CREATE_CLASS_BODY( Class ) CREATE_CLASS_IDENTIFIERS( Class, runtime_class_ ## Class )

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
#define GENERATE_ALL_CLASS( Class, ... ) \
	GENERATE_CLASS( Class __VA_OPT__(,) __VA_ARGS__ ) AFTER_FIRST( __VA_ARGS__ ) \
	{ CREATE_CLASS_BODY( Class ) sk_private:

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
#define SK_CLASS_BODY( ClassName ) CREATE_CLASS_IDENTIFIERS( ClassName, ClassName :: CONCAT( runtime_class_, ClassName ) )

// Add final class requirements and register it as a type in the global namespace.

#define MEMBER_PROCESSOR_TYPES \
	using func_array_t = sk::array< func_t, func_map_t::kSize >; \
	using var_array_t  = sk::array< var_t,  var_map_t ::kSize >;

// Register functions.
#define BUILD_CLASS_MEMBER_EXTRACTION_1_func_( Class, Counter ) \
	static constexpr auto CONCAT( _xxx_sk_func_, Counter ) = class_type::func_counter_t::next(); \
	using func_t = std::pair< sk::str_hash, sk::Reflection::cMemberFunction >; \
	using func_map_t = sk::const_map< func_t::first_type, const func_t::second_type*, CONCAT( _xxx_sk_func_, Counter ) >;

// Register variables.
#define BUILD_CLASS_MEMBER_EXTRACTION_1_var_( Class, Counter ) \
	static constexpr auto CONCAT( _xxx_sk_var_, Counter ) = class_type::var_counter_t::next(); \
	using var_t = sk::Reflection::member_var_pair_t; \
	using var_map_t = sk::const_map< var_t::first_type, const var_t::second_type*, CONCAT( _xxx_sk_var_, Counter ) >;

#define BUILD_CLASS_MEMBER_EXTRACTION_0_( Class, Counter ) namespace Class { namespace Internal { \
	BUILD_CLASS_MEMBER_EXTRACTION_1_func_( Class, Counter )  \
	BUILD_CLASS_MEMBER_EXTRACTION_1_var_( Class, Counter ) \
	MEMBER_PROCESSOR_TYPES \
	} \
	static constexpr auto kRawFunctions = \
		sk::Reflection::_xxx_process_member_functions< class_type, Internal:: CONCAT( _xxx_sk_func_, Counter ) >(); \
	static constexpr auto kRawVariables = \
		sk::Reflection::_xxx_process_member_variables< class_type, Internal:: CONCAT( _xxx_sk_var_, Counter ) >(); \
	static constexpr auto kFunctions = Internal::func_map_t{ sk::make_ptr_array_pair( kRawFunctions ) }; \
	static constexpr auto kVariables = Internal::var_map_t { sk::make_ptr_array_pair( kRawVariables ) }; \
	}

#define BUILD_CLASS_MEMBER_EXTRACTION( Class ) \
	BUILD_CLASS_MEMBER_EXTRACTION_0_( Class, CONCAT( counter_, __COUNTER__ ) )

// Member variable getters.
#define BUILD_CLASS_GET_VARIABLES( Class ) \
	constexpr auto Class ::class_type::staticGetVariables() -> sk::Reflection::member_var_map_ref_t{ \
	return Class ::kVariables; } \
	inline auto Class ::class_type::getVariables() const -> sk::Reflection::member_var_map_ref_t{ \
	return Class ::kVariables; }

#define BUILD_CLASS_VARIABLE_GETTER( Class ) \
	constexpr auto Class ::class_type::staticGetVariable( const sk::str_hash& _hash ) -> sk::Reflection::member_var_ptr_t{ \
	const auto itr = Test ::kVariables.find( _hash ); return itr == nullptr ? nullptr : itr->second; } \
	inline auto Class ::class_type::getVariable( const sk::str_hash& _hash ) const -> sk::Reflection::member_var_ptr_t{ \
	return staticGetVariable( _hash ); }

#define BUILD_CLASS_BOUND_VARIABLE_GETTER( Class ) \
	inline auto Class ::class_type::getBoundVariable( const sk::str_hash& _hash ) -> std::optional< sk::Reflection::cMemberVariableInstance< class_type > >{ \
	if( const auto member = getVariable( _hash ); member == nullptr ) return {}; \
	else return sk::Reflection::cMemberVariableInstance{ *member, this }; }

// Member function getters
#define BUILD_CLASS_GET_FUNCTIONS( Class ) \
	constexpr auto Class ::class_type::staticGetFunctions() -> sk::Reflection::member_func_map_ref_t{ \
	return Class ::kFunctions; } \
	inline auto Class ::class_type::getFunctions() const -> sk::Reflection::member_func_map_ref_t{ \
	return Class ::kFunctions; }

#define BUILD_CLASS_FUNCTION_GETTER( Class ) \
	constexpr auto Class ::class_type::staticGetFunction( const sk::str_hash& _hash ) -> sk::Reflection::member_func_ptr_t{ \
	const auto itr = Test ::kFunctions.find( _hash ); return itr == nullptr ? nullptr : itr->second; } \
	inline auto Class ::class_type::getFunction( const sk::str_hash& _hash ) const -> sk::Reflection::member_func_ptr_t{ \
	return staticGetFunction( _hash ); }

#define BUILD_CLASS_FUNCTION_OVERLOADS_GETTER( Class ) \
	constexpr auto Class ::class_type::staticGetFunctionOverloads( const sk::str_hash& _hash ) -> sk::Reflection::member_func_range_t{ \
	return Class ::kFunctions.range( _hash ); } \
	inline auto Class ::class_type::getFunctionOverloads( const sk::str_hash& _hash ) const -> sk::Reflection::member_func_range_t{ \
	return staticGetFunctionOverloads( _hash ); }

#define BUILD_CLASS_FUNCTION_OVERLOAD_RAW_GETTER( Class ) \
	constexpr auto Class ::class_type::staticGetFunction( const sk::str_hash& _hash, const sk::type_hash& _args ) -> sk::Reflection::member_func_ptr_t{ \
	for( auto [ fst, lst ] = Test ::kFunctions.range( _hash ); fst != lst; ++fst ) \
	{ if( fst->second->hasArgs( _args ) ) return fst->second; } return nullptr; } \
	inline auto Class ::class_type::getFunction( const sk::str_hash& _hash, const sk::type_hash& _args ) const -> sk::Reflection::member_func_ptr_t{ \
	return staticGetFunction( _hash, _args ); }

#define BUILD_CLASS_FUNCTION_OVERLOAD_TEMPLATE_GETTER( Class ) \
	template< class Ty, class... Args > constexpr auto Class ::class_type::staticGetFunction( const sk::str_hash& _hash ) -> sk::Reflection::member_func_ptr_t{ \
	return staticGetFunction( _hash, sk::args_hash< Ty, Args... >::kHash ); } \
	template< class Ty, class... Args > auto Class ::class_type::getFunction( const sk::str_hash& _hash ) -> sk::Reflection::member_func_ptr_t{ \
	return getFunction( _hash, sk::args_hash< Ty, Args... >::kHash ); }

#define BUILD_CLASS_BOUND_FUNCTION_GETTER( Class ) \
	inline auto Class ::class_type::getBoundFunction( const sk::str_hash& _hash ) -> std::optional< sk::Reflection::cMemberFunctionInstance< class_type > >{ \
	if( const auto member = getFunction( _hash ); member == nullptr ) return {}; \
	else return sk::Reflection::cMemberFunctionInstance{ *member, this }; }

// Build type_info
#define BUILD_CLASS_REFLECTION_INFO( Class ) \
	template<> struct sk::get_type_info< Class ::class_type >{ \
	constexpr static auto& kClass = Class ::class_type::kClass; \
	constexpr static sClass_Type_Info kInfo = { { \
		.type = sType_Info::eType::kClass, \
		.hash = kClass.getType(), \
		.size = sizeof( Class ::class_type ), \
		.name = kClass.getRawName(), \
		.raw_name = kClass.getRawName() }, &Class::class_type::getStaticClass() }; \
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
	REGISTER_TYPE_INTERNAL( Class::class_type )



// Internal macro. Do not use.
#define REGISTER_MEMBER_DIRECT_1_( Member, Counter ) \
	static constexpr auto CONCAT( _xxx_sk_member_id_, Counter ) = var_counter_t::next(); \
	[[ msvc::no_unique_address, maybe_unused ]] access_point::point CONCAT( _xxx_sk_point_, Counter ); \
	template< class Ty > struct variable_registry< std::integral_constant< Ty, CONCAT( _xxx_sk_member_id_, Counter ) > > { \
	using enum sk::Reflection::cMember::eType::eRaw; \
	using prev_t = variable_registry< std::integral_constant< Ty, CONCAT( _xxx_sk_member_id_, Counter ) -1 > >; \
	using pair_t = sk::Reflection::member_var_pair_t; \
	static constexpr auto kMemberHolder = sk::Reflection::cMemberVariableHolder{ &class_type:: Member }; \
	static constexpr auto kMember       = sk::Reflection::sPartial_Member_Variable< class_type, decltype( kMemberHolder ) > \
	{ .name = #Member, .holder = &kMemberHolder, .point = &class_type:: CONCAT( _xxx_sk_point_, Counter ) }; };
			
// Internal macro. Do not use.
#define REGISTER_MEMBER_DIRECT_0_( Member ) \
	REGISTER_MEMBER_DIRECT_1_( Member, __COUNTER__ )

//  sk::Reflection::sPartial_Member_Function
//  { .name = nullptr, .holder = sk::Reflection::cMemberFunctionHolder{ &_xxx_sk_placeholder_function }
// Internal macro. Do not use.
#define REGISTER_FUNCTION_DIRECT_1_( Member, Counter ) \
	static constexpr auto CONCAT( _xxx_sk_member_id_, Counter ) = func_counter_t::next(); \
	[[ msvc::no_unique_address, maybe_unused ]] access_point::point CONCAT( _xxx_sk_point_, Counter ); \
	template< class Ty > struct function_registry< std::integral_constant< Ty, CONCAT( _xxx_sk_member_id_, Counter ) > > { \
	using enum sk::Reflection::cMember::eType::eRaw; \
	using prev_t = function_registry< std::integral_constant< Ty, CONCAT( _xxx_sk_member_id_, Counter ) - 1 > >; \
	using pair_t = sk::Reflection::member_func_pair_t; \
	static constexpr auto kMemberHolder = sk::Reflection::cMemberFunctionHolder{ &class_type:: Member }; \
	static constexpr auto kMember       = sk::Reflection::sPartial_Member_Function< class_type, decltype( kMemberHolder ) > \
	{ .name = #Member, .holder = &kMemberHolder, .point = &class_type:: CONCAT( _xxx_sk_point_, Counter ) }; };
			
// Internal macro. Do not use.
#define REGISTER_FUNCTION_DIRECT_0_( Member ) private: \
	REGISTER_FUNCTION_DIRECT_1_( Member, __COUNTER__ )

/**
 * NOTE: The static variables will have the visibility that this macro is placed in.
 * @param Member Name of the member variable.
 */
#define SK_VARIABLE( Member, ... )     REGISTER_MEMBER_DIRECT_0_( Member )
/**
 * NOTE: The function will have the visibility that this macro is placed in.
 * @param Function Name of the member function
 */
#define SK_FUNCTION( Function, ... ) REGISTER_FUNCTION_DIRECT_0_( Function )

// WIP, Currently not functional
#define SK_PRIVATE_OVERLOADED_FUNCTION( Function, ... )   REGISTER_FUNCTION_DIRECT_0_( Function, kPrivate ) private:
// WIP, Currently not functional
#define SK_PROTECTED_OVERLOADED_FUNCTION( Function, ... ) REGISTER_FUNCTION_DIRECT_0_( Function, kProtected ) protected:
// WIP, Currently not functional
#define SK_PUBLIC_OVERLOADED_FUNCTION( Function, ... )    REGISTER_FUNCTION_DIRECT_0_( Function, kPublic ) public:

#define SK_CONSTRUCTOR( Function, ... )
#define SK_OVERLOADED_CONSTRUCTOR( Function, ... )

#define SK_ACCESS_CREATOR( Instance, Access ) \
	[[ msvc::no_unique_address, maybe_unused ]] access_point::point CONCAT( _xxx_sk_point_, Instance ); \
	static constexpr auto CONCAT( _xxx_sk_access_counter_, Instance ) = access_point::counter_t::next(); \
	template< class Ty > struct _xxx_sk_access< std::integral_constant< Ty, CONCAT( _xxx_sk_access_counter_, Instance ) > >{ \
		static constexpr auto kAccess = access_point{ \
			.m_access = sk::Reflection::cMember::eType:: Access, .m_point = &class_type::CONCAT( _xxx_sk_point_, Instance ) }; \
	};

#define sk_public \
	public: SK_ACCESS_CREATOR( __COUNTER__, kPublic ) public

#define sk_protected \
	public: SK_ACCESS_CREATOR( __COUNTER__, kProtected ) protected

#define sk_private \
	public: SK_ACCESS_CREATOR( __COUNTER__, kPrivate ) private