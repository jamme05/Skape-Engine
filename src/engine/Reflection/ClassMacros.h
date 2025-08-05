
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
	using class_type  = runtime_class_type::value_type; \
	using parent_type = runtime_class_type::parent_type::value_type; \
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
		static constexpr auto kMember = sk::Reflection::sPartial_Member< class_type, decltype( kMemberHolder ) >{ \
			.name = nullptr, .holder = &kMemberHolder }; }; \
	template< class > struct function_registry { \
		static void _xxx_sk_placeholder_function(){} \
		static constexpr auto kMemberHolder = sk::Reflection::cMemberFunctionHolder{ &_xxx_sk_placeholder_function };\
		static constexpr auto kMember = sk::Reflection::sPartial_Member< class_type, decltype( kMemberHolder ) >{ \
			.name = nullptr, .holder = &kMemberHolder, .point = &class_type::_xxx_sk_point_placeholder }; }; \
	using var_counter_t  = const_counter< var_tag_ >;   \
	using func_counter_t  = const_counter< func_tag_ >; \
	template< int N > struct variable_extractor : variable_registry< std::integral_constant< int, N > >{};\
	template< int N > struct function_extractor : function_registry< std::integral_constant< int, N > >{};\

#define CREATE_MEMBER_REFLECTION_FUNCTIONS( RuntimeClass ) \
	/* static constexpr auto getConstructors() -> sk::Reflection::member_func_map_ref_t; WIP */ \
	[[ nodiscard ]] auto getVariables() const -> sk::Reflection::member_var_map_ref_t override; \
	[[ nodiscard ]] auto getFunctions() const -> sk::Reflection::member_func_map_ref_t override; \
	[[ nodiscard ]] auto getVariable( const sk::str_hash& _hash ) const -> sk::Reflection::member_var_ptr_t override; \
	[[ nodiscard ]] auto getFunction( const sk::str_hash& _hash ) const -> sk::Reflection::member_func_ptr_t override;\
	[[ nodiscard ]] auto getFunctionOverloads( const sk::str_hash& _hash ) const -> sk::Reflection::member_func_range_t override; \
	[[ nodiscard ]] auto getFunction( const sk::str_hash& _hash, const sk::type_hash& _args ) const -> sk::Reflection::member_func_ptr_t override; \
	template< class Ty, class... Args > [[ nodiscard ]] auto getFunction( const sk::str_hash& _hash ) -> sk::Reflection::member_func_ptr_t; \
	/* static constexpr auto staticGetConstructors() -> sk::Reflection::member_func_map_ref_t; WIP */ \
	[[ nodiscard ]] static constexpr auto staticGetVariables() -> sk::Reflection::member_var_map_ref_t; \
	[[ nodiscard ]] static constexpr auto staticGetFunctions() -> sk::Reflection::member_func_map_ref_t; \
	[[ nodiscard ]] static constexpr auto staticGetVariable( const sk::str_hash& _hash ) -> sk::Reflection::member_var_ptr_t; \
	[[ nodiscard ]] static constexpr auto staticGetFunction( const sk::str_hash& _hash ) -> sk::Reflection::member_func_ptr_t; \
	[[ nodiscard ]] static constexpr auto staticGetFunctionOverloads( const sk::str_hash& _hash ) -> sk::Reflection::member_func_range_t; \
	[[ nodiscard ]] static constexpr auto staticGetFunction( const sk::str_hash& _hash, const sk::type_hash& _args ) -> sk::Reflection::member_func_ptr_t; \
	template< class Ty, class... Args > [[ nodiscard ]] static constexpr auto staticGetFunction( const sk::str_hash& _hash ) -> sk::Reflection::member_func_ptr_t; \
	[[ nodiscard ]] auto getBoundVariable( const sk::str_hash& _hash ) -> std::optional< sk::Reflection::cMemberVariableInstance< class_type > >; \
	[[ nodiscard ]] auto getBoundFunction( const sk::str_hash& _hash ) -> std::optional< sk::Reflection::cMemberFunctionInstance< class_type > >; \

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
#define SK_CLASS_BODY( ClassName ) \
	CREATE_CLASS_IDENTIFIERS( ClassName, ClassName :: CONCAT( runtime_class_, ClassName ) )

// Add final class requirements and register it as a type in the global namespace.

#define MEMBER_PROCESSOR_TYPES \
	using func_array_t = sk::array< func_t, func_map_t::kSize >; \
	using var_array_t  = sk::array< var_t,  var_map_t ::kSize >;

// Register functions.
#define BUILD_CLASS_MEMBER_EXTRACTION_1_func_( Class, Counter ) \
	static constexpr auto CONCAT( _xxx_sk_func_, Counter ) = class_type::func_counter_t::next(); \
	using func_t = std::pair< sk::str_hash, sk::Reflection::cMemberFunction >; \
	using func_map_t = sk::const_map< func_t::first_type, const func_t::second_type*, CONCAT( _xxx_sk_func_, Counter ) >; \
	static constexpr auto kParentFunctions = class_type::parent_type::staticGetFunctions(); \
	static constexpr auto kParentFuncCount = kParentFunctions.size();

// Register variables.
#define BUILD_CLASS_MEMBER_EXTRACTION_1_var_( Class, Counter ) \
	static constexpr auto CONCAT( _xxx_sk_var_, Counter ) = class_type::var_counter_t::next(); \
	using var_t = sk::Reflection::member_var_pair_t; \
	using var_map_t = sk::const_map< var_t::first_type, const var_t::second_type*, CONCAT( _xxx_sk_var_, Counter ) >; \
	static constexpr auto kParentVariables = class_type::parent_type::staticGetVariables(); \
	static constexpr auto kParentVarCount  = kParentVariables.size();


#define BUILD_CLASS_MEMBER_EXTRACTION_0_( Class, Counter ) namespace Class { namespace Internal { \
	BUILD_CLASS_MEMBER_EXTRACTION_1_func_( Class, Counter )  \
	BUILD_CLASS_MEMBER_EXTRACTION_1_var_( Class, Counter ) \
	MEMBER_PROCESSOR_TYPES \
	static constexpr auto kRawFunctions = \
		sk::Reflection::process_member_functions< class_type, Internal:: CONCAT( _xxx_sk_func_, Counter ) >(); \
	static constexpr auto kRawVariables = \
		sk::Reflection::process_member_variables< class_type, Internal:: CONCAT( _xxx_sk_var_, Counter ) >(); \
	} \
	static constexpr auto kFunctions = Internal::func_map_t{ \
		sk::Reflection::finalize_member_functions< Internal::kParentFuncCount >( \
					Internal::kRawFunctions, Internal::kParentFunctions ) }; \
	static constexpr auto kVariables = Internal::var_map_t{ \
			sk::Reflection::finalize_member_variables< Internal::kParentVarCount >( \
				Internal::kRawVariables, Internal::kParentVariables ) }; \
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

// TODO: Create a macro that checks if it's included or not.
#define REGISTER_CLASS( Class ) \
	REGISTER_TYPE_INTERNAL( Class::class_type, false )

#define REGISTER_CLASS_INLINE( Class ) \
	REGISTER_TYPE_INTERNAL( Class::class_type, true )

// Build type_info
#define BUILD_CLASS_REFLECTION_INFO( Class ) \
	template<> struct sk::get_type_info< Class ::class_type > : sk::template_type_info{ \
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
#define DECLARE_CLASS( Class ) \
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
	BUILD_CLASS_REFLECTION_INFO( Class )

#define REGISTER_MEMBER_DIRECT_2_( Member, Registry, CounterVar, CounterValue, Point, HolderPack, ... ) \
		static constexpr auto CounterVar = CounterValue; \
		[[ msvc::no_unique_address, maybe_unused ]] access_point::point Point; \
		template< class Ty > struct Registry< std::integral_constant< Ty, CounterVar > > { \
		using enum sk::Reflection::cMember::eType::eRaw; \
		using prev_t = Registry< std::integral_constant< Ty, CounterVar -1 > >; \
		UNWRAP( HolderPack ) \
		static constexpr auto kMemberHolder = __VA_ARGS__ { &class_type:: Member }; \
		static constexpr auto kMember       = sk::Reflection::sPartial_Member< class_type, decltype( kMemberHolder ) > \
		{ .name = #Member, .holder = &kMemberHolder, .point = &class_type:: Point }; };

// Internal macro. Do not use.
#define REGISTER_MEMBER_VARIABLE_DIRECT_1_( Member, Counter ) \
	REGISTER_MEMBER_DIRECT_2_( Member, variable_registry, \
		CONCAT( _xxx_sk_member_id_, Counter ), var_counter_t::next(), \
		CONCAT( _xxx_sk_point_, Counter ), EMPTY, sk::Reflection::cMemberVariableHolder )
			
// Internal macro. Do not use.
#define REGISTER_MEMBER_DIRECT_0_( Member ) \
	REGISTER_MEMBER_VARIABLE_DIRECT_1_( Member, __COUNTER__ )

// Internal macro. Do not use.
#define REGISTER_MEMBER_FUNCTION_DIRECT_1_( Member, Counter, HolderPack, ... ) \
	REGISTER_MEMBER_DIRECT_2_( Member, function_registry, \
		CONCAT( _xxx_sk_member_id_, Counter ), func_counter_t::next(), \
		CONCAT( _xxx_sk_point_, Counter ), HolderPack, __VA_ARGS__ )
			
// Internal macro. Do not use.
#define REGISTER_FUNCTION_DIRECT_0_( Member, ... ) \
	REGISTER_MEMBER_FUNCTION_DIRECT_1_( Member, __COUNTER__, __VA_ARGS__ )

/**
 * NOTE: The variables will have the visibility (Public,Protected,Private) that this macro is placed in.
 * @param Member Name of the member variable.
 */
#define SK_VARIABLE( Member, ... )     REGISTER_MEMBER_DIRECT_0_( Member )

#define IS_RETURN_FUNCTION_RETURN( ... ) ,1
#define IS_ARG_FUNCTION_ARG( ... ) ,1

// Full pack.
#define IS_F_FUNCTION_RETURN( ... ) ,2
#define IS_F_FUNCTION_ARG( ... ) ,2

#define IS_RETURN_FUNCTION_RETURN( ... ) ,1
#define IS_ARG_FUNCTION_ARG( ... ) ,1

#define IS_CONST_FUNCTION_CONST ,1
#define IS_CONST_CONST ,1
#define IS_F_FUNCTION_CONST ,1
#define IS_F_CONST ,1

#define IS_STATIC_FUNCTION_STATIC ,1
#define IS_STATIC_STATIC ,1
#define IS_F_FUNCTION_STATIC ,1
#define IS_F_STATIC ,1

// Partial pack.
#define IS_F_R( ... ) ,1
#define IS_F_A( ... ) ,1

#define FILTER_FUNCTION_PACK_1_2( Pack ) Pack,
#define FILTER_FUNCTION_PACK_1_1( Pack ) CONCAT( FUNCTION, Pack ),
#define FILTER_FUNCTION_PACK_1_0( Pack ) /* Maybe? static_assert( false, "Invalid argument: " #Pack ) */

#define FILTER_FUNCTION_PACK_0( Pack, Value ) \
	CONCAT( FILTER_FUNCTION_PACK_1_, SECOND( Value, 0 ) ) ( Pack )

#define FILTER_FUNCTION_PACK( Pack ) \
	FILTER_FUNCTION_PACK_0( Pack, CONCAT( IS_F, Pack ) )

#define FILTER_FUNCTION_MISC_ARG_1_2( Arg )
#define FILTER_FUNCTION_MISC_ARG_1_1( Arg )
#define FILTER_FUNCTION_MISC_ARG_1_0( Arg ) Arg

#define FILTER_FUNCTION_MISC_ARG_0( Arg, Value ) \
	CONCAT( FILTER_FUNCTION_MISC_ARG_1_, SECOND( Value, 0 ) ) ( Arg )

#define FILTER_FUNCTION_MISC_ARG( Arg ) \
	FILTER_FUNCTION_MISC_ARG_0( Arg, CONCAT( IS_F, Arg ) )

#define FILTER_FUNCTION_PACK_TYPE_2_1( Pack ) Pack,
#define FILTER_FUNCTION_PACK_TYPE_2_0( ... )

#define FILTER_FUNCTION_PACK_TYPE_1( Arg, Value ) \
	CONCAT( FILTER_FUNCTION_PACK_TYPE_2_, SECOND( Value, 0 ) ) ( Arg )

#define FILTER_FUNCTION_PACK_TYPE_0( Arg, Target ) \
	FILTER_FUNCTION_PACK_TYPE_1( Arg, CONCAT( Target, Arg ) )

// "Returns" All FUNCTION_RETURN Packs
#define FILTER_FUNCTION_RETURN( Arg ) \
	FILTER_FUNCTION_PACK_TYPE_0( Arg, IS_RETURN )

// "Returns" All FUNCTION_ARG Packs
#define FILTER_FUNCTION_ARG( Arg ) \
	FILTER_FUNCTION_PACK_TYPE_0( Arg, IS_ARG )

#define IS_CONST_MACRO_1_1( Arg ) 1
#define IS_CONST_MACRO_1_0( Arg )

#define IS_CONST_MACRO_0( Arg, Value ) \
	CONCAT( IS_CONST_MACRO_1_, SECOND( Value, 0 ) ) ( Arg )

#define IS_CONST_MACRO( Arg ) \
	IS_CONST_MACRO_0( Arg, CONCAT( IS_CONST, Arg ) )

#define IS_STATIC_MACRO_1_1( Arg ) 1
#define IS_STATIC_MACRO_1_0( Arg )

#define IS_STATIC_MACRO_0( Arg, Value ) \
	CONCAT( IS_STATIC_MACRO_1_, SECOND( Value, 0 ) ) ( Arg )

#define IS_STATIC_MACRO( Arg ) \
	IS_STATIC_MACRO_0( Arg, CONCAT( IS_STATIC, Arg ) )

// Removes unused comma in the end.
#define CLEAN_ARG( Arg, ... ) \
	Arg __VA_OPT__( , )

// Clean up
#define CLEAN_ARGS( ... ) \
	FOR_EACH_FORWARD( CLEAN_ARG, __VA_ARGS__ )

// First pass with selected macro.
#define FILTER_FUNCTION_ARGUMENTS( Macro, ... ) \
	CLEAN_ARGS( FOR_EACH( Macro, __VA_ARGS__ ) )

// Don't do anything in case no packs were provided.
// Creates a list of arguments made only of valid packs. The packs are used for macro logic.
#define FILTER_FUNCTION_MACRO_PACKS( ... ) \
	__VA_OPT__( FILTER_FUNCTION_ARGUMENTS( FILTER_FUNCTION_PACK, __VA_ARGS__ ) )

// Creates a list of the arguments that aren't valid packs. The diverse arguments are used for reflection info. Such as DisplayName.
#define FILTER_FUNCTION_MISC_ARGS( ... ) \
	__VA_OPT__( FILTER_FUNCTION_ARGUMENTS( FILTER_FUNCTION_MISC_ARG, __VA_ARGS__ ) )

#define FILTER_FUNCTION_ARG_PACKS( ... ) \
	__VA_OPT__( FILTER_FUNCTION_ARGUMENTS( FILTER_FUNCTION_ARG, __VA_ARGS__ ) )

#define CREATE_COMPLEX_RETURN_TYPE( /* Args */ ... ) \
	COMPLEX_TYPE( __VA_ARGS__ )

// Selects the first argument, otherwise defaults to a return with void.
#define FILTER_FUNCTION_RETURN_PACK_0( ArgsPack, ... ) \
	FIRST( __VA_OPT__( __VA_ARGS__, ) CREATE_COMPLEX_RETURN_TYPE( ArgsPack ) )

#define FILTER_FUNCTION_RETURN_PACK( ... ) \
	FILTER_FUNCTION_RETURN_PACK_0( PACK( GET_ARGUMENT_TYPES( __VA_ARGS__ ) ) \
	, FILTER_FUNCTION_ARGUMENTS( FILTER_FUNCTION_RETURN, __VA_ARGS__ ) )

#define IS_CONST_FUNCTION( ... ) \
	HAS_ARGS( FILTER_FUNCTION_ARGUMENTS( IS_CONST_MACRO, __VA_ARGS__ ) )

#define IS_STATIC_FUNCTION( ... ) \
	HAS_ARGS( FILTER_FUNCTION_ARGUMENTS( IS_STATIC_MACRO, __VA_ARGS__ ) )

#define IS_SPECIALIZED_FUNCTION( ... ) \
	HAS_ARGS( FILTER_FUNCTION_MACRO_PACKS( __VA_ARGS__ ) )

// ( get_return_type< class_type __VA_ARGS__ > )

#define MAKE_DECLVAL_ARG( Type ) std::declval< Type >(),

#define MAKE_DECLVAL_ARGS( ... ) \
	CLEAN_ARGS( FOR_EACH( MAKE_DECLVAL_ARG, __VA_ARGS__ ) )

#define COMPLETE_COMPLEX_TYPE_0( Function, /* Function Args */ ... ) \
	decltype( Function( MAKE_DECLVAL_ARGS( __VA_ARGS__ ) ) )
	
#define COMPLETE_COMPLEX_TYPE( Function, ComplexType ) \
	COMPLETE_COMPLEX_TYPE_0( Function, UNPACK( CONCAT( TYPE, ComplexType ) ) )

#define IS_COMPLEX_COMPLEX_TYPE( ... ) ,1
#define TYPE_COMPLEX_TYPE( ... ) __VA_ARGS__
#define COMPLEX_TYPE( ... ) _COMPLEX_TYPE( __VA_ARGS__ )

#define TYPE_FUNCTION_RETURN( Type, ... ) Type
#define META_FUNCTION_RETURN( Type, ... ) __VA_ARGS__
#define TYPE_FUNCTION_ARG( Type, ... ) Type
#define META_FUNCTION_ARG( Type, ... ) __VA_ARGS__

#define FUNCTION_R( ... ) _FUNCTION_RETURN( __VA_ARGS__ )
#define R( ... ) _R( __VA_ARGS__ )
#define RET( ... ) FUNCTION_R( __VA_ARGS__ )
#define RETURN( ... ) RET( __VA_ARGS__ )

#define FUNCTION_A( ... ) _FUNCTION_ARG( __VA_ARGS__ )
#define A( ... ) _A( __VA_ARGS__ )
#define ARG( ... ) FUNCTION_A( __VA_ARGS__ )

#define FUNCTION_CONST _FUNCTION_CONST
#define CONST _CONST

#define FUNCTION_STATIC _FUNCTION_STATIC
#define STATIC _STATIC

// FILTER_FUNCTION_MACRO_PACKS( H, D, K, L, A( Test1 ), R( void ), RET( IIIInvalid ), ARG( uint32_t, "Count" ), CONST, STATIC )
// Result: _FUNCTION_ARG( Test1 ) , _FUNCTION_RETURN( void ) , _FUNCTION_RETURN( Invalid ) , _FUNCTION_ARG( uint32_t, "Count" )

// FILTER_FUNCTION_MISC_ARGS  ( H, D, K, L, A( Test1 ), R( void ), RET( Invalid ), ARG( uint32_t, "Count" ) )
// Result: H D K L

// FILTER_FUNCTION_RETURN_PACK( FILTER_FUNCTION_MACRO_PACKS( H, D, K, L, A( Test1 ), R( uint32_t ), RET( Invalid ), ARG( uint32_t, "Count" ) ) )
// Result: _FUNCTION_RETURN( uint32_t )

// FILTER_FUNCTION_RETURN_PACK( FILTER_FUNCTION_MACRO_PACKS( H, D, K, L, A( Test1 ), ARG( uint32_t, "Count" ) ) )
// Result: _FUNCTION_RETURN( void ) 

// FILTER_FUNCTION_ARG_PACKS( FILTER_FUNCTION_MACRO_PACKS( H, D, K, L, A( size_t ), ARG( uint32_t, "Count" ) ) )
// Result: _FUNCTION_ARG ( size_t ), _FUNCTION_ARG( uint32_t, "Count" )

// IS_CONST_FUNCTION( H, D, K, L, A( Test1 ), R( void ), RET( Invalid ), ARG( uint32_t, "Count" ), CONST )
// Result: 1

// IS_CONST_FUNCTION( H, D, K, L, A( Test1 ), R( void ), RET( Invalid ), ARG( uint32_t, "Count" ) )
// Result: 0

// IS_STATIC_FUNCTION( H, D, K, L, A( Test1 ), R( void ), RET( Invalid ), ARG( uint32_t, "Count" ), STATIC )
// Result: 1

// IS_STATIC_FUNCTION( H, D, K, L, A( Test1 ), R( void ), RET( Invalid ), ARG( uint32_t, "Count" ) )
// Result: 0

#define SELECT_FUNCTION_CLASS_TYPE_1 void
#define SELECT_FUNCTION_CLASS_TYPE_0 class_type

#define SELECT_FUNCTION_IS_CONST_1 const
#define SELECT_FUNCTION_IS_CONST_0

// Complex return type, requires special handling
#define GET_RETURN_TYPE_1_1( Function, ReturnPack ) \
	COMPLETE_COMPLEX_TYPE( Function, ReturnPack )

#define GET_RETURN_TYPE_1_0( Function, ReturnPack ) \
	CONCAT( TYPE, ReturnPack )

#define GET_RETURN_TYPE_1( Function, ReturnPack, ... ) \
	CONCAT( GET_RETURN_TYPE_1_, SECOND( __VA_ARGS__, 0 ) ) ( Function, ReturnPack ) 

#define GET_RETURN_TYPE_0( Function, ReturnPack ) \
	GET_RETURN_TYPE_1( Function, ReturnPack, CONCAT( IS_COMPLEX, ReturnPack ) )

#define GET_RETURN_TYPE( Function, ... ) \
	GET_RETURN_TYPE_0( Function, FILTER_FUNCTION_RETURN_PACK( __VA_ARGS__ ) )

#define GET_ARGUMENT_TYPE( ArgPack ) \
	CONCAT( TYPE, ArgPack ),

#define GET_ARGUMENT_TYPES_1( ... ) \
	__VA_OPT__( , __VA_ARGS__ )

#define GET_ARGUMENT_TYPES_0( ... ) \
	CLEAN_ARGS( FOR_EACH( GET_ARGUMENT_TYPE, __VA_ARGS__ ) )

#define GET_ARGUMENT_TYPES( ... ) \
	GET_ARGUMENT_TYPES_0( FILTER_FUNCTION_ARG_PACKS( __VA_ARGS__ ) )

#define GET_TEMPLATE_ARGUMENT_TYPES_1_1( ... ) , __VA_ARGS__
	
#define GET_TEMPLATE_ARGUMENT_TYPES_1_0( ... )
	
#define GET_TEMPLATE_ARGUMENT_TYPES_0( ... ) \
	CONCAT( GET_TEMPLATE_ARGUMENT_TYPES_1_, HAS_ARGS( __VA_ARGS__ ) ) ( __VA_ARGS__ )
	
#define GET_TEMPLATE_ARGUMENT_TYPES( ... ) \
	GET_TEMPLATE_ARGUMENT_TYPES_0( GET_ARGUMENT_TYPES( __VA_ARGS__ ) )

// Static
#define CREATE_FUNCTION_HOLDER_TEMPLATE_1_1( Function, Const, ClassType, ... ) \
	using holder_t = \
	sk::Reflection::cMemberFunctionHolder< Const ClassType, \
	GET_RETURN_TYPE( Function, __VA_ARGS__ ) \
	GET_TEMPLATE_ARGUMENT_TYPES( __VA_ARGS__ ) \
	>;

// Non Static
#define CREATE_FUNCTION_HOLDER_TEMPLATE_1_0( Function, Const, ClassType, ... ) \
	template< class Re > using holder_t = \
	sk::Reflection::cMemberFunctionHolder< Const ClassType, Re \
	GET_TEMPLATE_ARGUMENT_TYPES( __VA_ARGS__ ) \
	>;

#define CREATE_FUNCTION_HOLDER_TEMPLATE_0( Function, Const, ClassType, IsStatic, ... ) \
	CONCAT( CREATE_FUNCTION_HOLDER_TEMPLATE_1_, IsStatic ) ( Function, Const, ClassType, __VA_ARGS__ )

#define CREATE_FUNCTION_HOLDER_TEMPLATE( Function, ... ) \
	CREATE_FUNCTION_HOLDER_TEMPLATE_0( Function \
	, CONCAT( SELECT_FUNCTION_IS_CONST_,   IS_CONST_FUNCTION ( __VA_ARGS__ ) ) \
	, CONCAT( SELECT_FUNCTION_CLASS_TYPE_, IS_STATIC_FUNCTION( __VA_ARGS__ ) ) \
	, IS_STATIC_FUNCTION( __VA_ARGS__ ), __VA_ARGS__ )

#define SK_FUNCTION_SPECIALIZED_( Function, SpecializationPack, MiscPack, ... ) \
	REGISTER_FUNCTION_DIRECT_0_( Function \
	, PACK( CREATE_FUNCTION_HOLDER_TEMPLATE( Function, UNWRAP( SpecializationPack ) ) ), \
	holder_t )

// TODO: Create weirder names for SK_FUNCTION_xxx to hide them from the user.

// Specialized Function
#define SK_FUNCTION_1_1( Function, ... ) \
	SK_FUNCTION_SPECIALIZED_( Function \
	, PACK( FILTER_FUNCTION_MACRO_PACKS( __VA_ARGS__ ) ) \
	, PACK( FILTER_FUNCTION_MISC_ARG   ( __VA_ARGS__ ) ) )

// Non-Specialized Function 
#define SK_FUNCTION_1_0( Function, ... ) \
	REGISTER_FUNCTION_DIRECT_0_( Function, EMPTY, sk::Reflection::cMemberFunctionHolder )

#define SK_FUNCTION_0_( Function, HasSpecialization, ... ) \
	CONCAT( SK_FUNCTION_1_, HasSpecialization ) ( Function, __VA_ARGS__ )

/**
 * In case R/RET/RETURN or A/ARG macros are provided, you will be required to fully specify what return value/arguments the function has.
 * Except: If case the return value is void, no RET is required. Or if there's no arguments, no ARG is required.
 * At least one specialization (RETURN/ARG/STATIC/CONST) or the macro SK_FUNCTION_O is required for overloads.
 * NOTE: The function will have the visibility (Public,Protected,Private) that this macro is placed in.
 * @param Function Name of the member function
 */
#define SK_FUNCTION( Function, ... ) \
	SK_FUNCTION_0_( Function, IS_SPECIALIZED_FUNCTION( __VA_ARGS__ ), __VA_ARGS__ )

/**
 * In case R/RET/RETURN or A/ARG macros are provided, you will be required to fully specify what return value/arguments the function has.
 * Except: If case the return value is void, no RET is required. Or if there's no arguments, no ARG is required.
 * NOTE: The function will have the visibility (Public,Protected,Private) that this macro is placed in.
 * This forces the reflection to look at the function in an overloaded scenario.
 * @param Function Name of the member function
 */
#define SK_FUNCTION_O( Function, ... ) \
	SK_FUNCTION_1_1( Function, __VA_ARGS__ )

/**
 * In case R/RET/RETURN or A/ARG macros are provided, you will be required to fully specify what return value/arguments the function has.
 * Except: If case the return value is void, no RET is required. Or if there's no arguments, no ARG is required.
 * NOTE: The function will have the visibility (Public,Protected,Private) that this macro is placed in.
 * This forces the reflection to look at the function in an overloaded scenario.
 * @param Function Name of the member function
 */
#define SK_FUNCTION_C( Function, ... ) \
	SK_FUNCTION( Function, __VA_ARGS__, CONST )

/**
 * NOTE: The function will have the visibility (Public,Protected,Private) that this macro is placed in.
 * NOTE: This macro REQUIRES specialization.
 * Registers both a non-const and const function with the same overload.
 * @param Function Name of the member function
 */
#define SK_FUNCTION_P( Function, ... ) \
	SK_FUNCTION_1_1( Function, __VA_ARGS__ ) \
	SK_FUNCTION_1_1( Function, __VA_ARGS__, CONST )

#define SK_CONSTRUCTOR( Function, ... )
#define SK_CONSTRUCTOR_O( Function, ... )

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
