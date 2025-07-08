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
#include <Misc/Offsetof.h>

#include <Reflection/ClassMacros.h>

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
		static std::true_type test( decltype( &Ty2::getStaticClass ) ){ return {}; }
		template< class >
		static std::false_type test( ... ){ return {}; }
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

	class iClass;

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

				kPublic     = 0x02,
				kProtected  = 0x04,
				kPrivate    = 0x08,
				kVisibility = kPublic | kProtected | kPrivate,

				// Modifiers
				kStatic      = 0x10,
				kVirtual     = 0x20,
				kConst       = 0x40,
				// Alias for Const
				kReadOnly    = 0x40,
				kConstructor = 0x80,
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

		consteval auto get_raw() const { return m_member_; }

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

		consteval auto get_raw() const { return m_value_; }

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
		constexpr cMemberVariable()
		: cMember( nullptr, 0 )
		, m_member_type_info_( nullptr )
		, m_class_( nullptr )
		, m_holder_( nullptr )
		{}

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
	constexpr auto cMemberVariable::bind( Ty* _instance ) -> cMemberVariableInstance< Ty >
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

		consteval auto get_raw() const { return m_func_; }

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

		consteval auto get_raw() const { return m_func_; }

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

		consteval auto get_raw() const { return m_func_; }

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
		constexpr cMemberFunction()
		: cMember( nullptr, 0 )
		, m_return_type_{ nullptr }
		, m_args_info_{ nullptr }
		, m_class_{ nullptr }
		, m_holder_{ nullptr }
		{}

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
			SK_WARN_IF_RET( sk::Severity::kReflection, args_t::kHash != m_args_info_->hash,
				TEXT( "Error: Wrong types provided! Provided types: {}. Required types: {}.", args_t::kInfo.to_string(), m_args_info_->to_string() ),
				false )

			// Verify return type. But only if it's required.
			constexpr static auto& other_type = get_type_info< Re >::kInfo;
			SK_WARN_IF_RET( sk::Severity::kReflection, _return != nullptr && *m_return_type_ != other_type,
				TEXT( "Error: Wrong return type provided. Provided type: {}. Required type: {}.", other_type.raw_name, m_return_type_->raw_name ),
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
			SK_WARN_IF_RET( sk::Severity::kReflection, getIsStatic(),
				TEXT( "Warning: Calling static function through instance." ),
				call< Re, Args... >( _return, std::forward< Args >( _args )... ) )

			// Const check. If getting called by a const ptr.
			if constexpr( !std::is_const_v< Ty > )
				SK_WARN_IF_RET( sk::Severity::kReflection,
					!getIsReadOnly(), TEXT( "Error: Calling a non-const function with a constant instance." ), false )

			// Verify class.
			SK_WARN_IF_RET( sk::Severity::kReflection, Ty::kClass != *m_class_,
				TEXT( "Error: Tried calling function through class {} when it required class {}.", Ty::getClassName(), m_class_->getName() ),
				false )

			// Verify args.
			typedef args_hash< Args... > args_t;
			SK_WARN_IF_RET( sk::Severity::kReflection, args_t::kHash != m_args_info_->hash,
				TEXT( "Error: Wrong types provided! Provided types: {}. Required types: {}.", args_t::kInfo.to_string(), m_args_info_->to_string() ),
				false )

			// Verify return type. But only if it's required.
			constexpr static auto& other_type = get_type_info< Re >::kInfo;
			SK_WARN_IF_RET( sk::Severity::kReflection, _return != nullptr && *m_return_type_ != other_type,
				TEXT( "Error: Wrong return type provided. Provided type: {}. Required type: {}.", other_type.raw_name, m_return_type_->raw_name ),
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
			SK_WARN_IF_RET( sk::Severity::kReflection, _instance == nullptr && !getIsStatic(),
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
		} // call_

	}; // cMemberFunctionInstance

	template< sk_class Ty >
	constexpr auto cMemberFunction::bind( Ty& _instance ) const -> std::optional< cMemberFunctionInstance< Ty > >
	{
		// Verify class.
		if( Ty::kClass != *m_class_ )
		{
			// TODO: Use SK_WARN_IF_RET macro.
			std::println( "Error: Tried binding function to class {} when it required class {}.", Ty::getClassName(), m_class_->getName() );
			return {};
		}
		return cMemberFunctionInstance< Ty >( static_cast< class_t >( &_instance ) );

	} // cMemberFunction::bind

	template< class Ty >
	struct access_point
	{
		using counter_t = const_counter< access_point >;
		struct point{};

		uint32_t    m_access;
		point Ty::* m_point;
	};

	template< class Ty, class Holder >
	struct sPartial_Member_Variable
	{
		using point_t  = typename access_point< Ty >::point;
		const char*   name;
		Holder*       holder;
		point_t Ty::* point;
	};

	template< class Ty, class Holder >
	struct sPartial_Member_Function
	{
		using point_t  = typename access_point< Ty >::point;
		const char*   name;
		Holder*       holder;
		point_t Ty::* point;
	};

	typedef const cMemberVariable*                 member_var_ptr_t;
	typedef const cMemberFunction*                 member_func_ptr_t;
	typedef std::pair< str_hash, cMemberVariable > member_var_pair_t;
	typedef std::pair< str_hash, cMemberFunction > member_func_pair_t;
	typedef std::pair< str_hash, cMemberVariable >    member_var_map_pair_t;
	typedef std::pair< str_hash, member_func_ptr_t >  member_func_map_pair_t;
	typedef map_ref  < str_hash, member_var_ptr_t  >  member_var_map_ref_t;
	typedef map_ref  < str_hash, member_func_ptr_t >  member_func_map_ref_t;
	typedef std::pair< const member_var_map_pair_t*,  const member_var_map_pair_t* >  member_var_range_t;
	typedef std::pair< const member_func_map_pair_t*, const member_func_map_pair_t* > member_func_range_t;

	template< class Ty, size_t Layer = 0 >
	static consteval uint8_t _xxx_sk_get_offset_access( const int _offset, const uint8_t _previous_access = 0 )
	{
		// 0 Will always be defined. Hence, it'll never begin with there being nothing.
		constexpr auto& point = Ty::template _xxx_sk_access< std::integral_constant< int, Layer > >::kAccess;

		if( _offset < offset_of< Ty, point.m_point >() )
			return _previous_access;

		if constexpr( point.m_access == 0 )
			return _previous_access;
		else
			return _xxx_sk_get_offset_access< Ty, Layer + 1 >( _offset, point.m_access );

	} // _xxx_sk_get_offset_access

	template< class Ty, size_t Size, size_t Layer = Size == 0 ? 0 : Size - 1  >
	consteval auto _xxx_process_member_functions( array< member_func_pair_t, Size >* _array = nullptr ) -> array< member_func_pair_t, Size >
	{
		using array_t = array< member_func_pair_t, Size >;
		if constexpr( Size == 0 ) return array_t{};

		using extractor_t = typename Ty:: template function_extractor< Layer >;
		constexpr auto& extraction = extractor_t::kMember;
		if( _array )
		{
			auto access = _xxx_sk_get_offset_access< Ty >( offset_of< Ty, extraction.point >() );
			( *_array )[ Layer ] = member_func_pair_t{
				extraction.name, cMemberFunction{ extraction.name, *extraction.holder, access }
			};
		}
		if constexpr( Layer == 0 )
		{
			if( _array ) return *_array;
			return array_t{};
		}
		else
		{
			if( _array ) return _xxx_process_member_functions< Ty, Size, Layer - 1 >( _array );

			auto access = _xxx_sk_get_offset_access< Ty >( offset_of< Ty, extraction.point >() );
			array_t array{};
			array[ Layer ] = member_func_pair_t{
				extraction.name, cMemberFunction{ extraction.name, *extraction.holder, access }
			};
			return _xxx_process_member_functions< Ty, Size, Layer - 1 >( &array );
		}

	} // _xxx_process_member_functions

	template< class Ty, size_t Size, size_t Layer = Size == 0 ? 0 : Size - 1  >
	consteval auto _xxx_process_member_variables( array< member_var_pair_t, Size >* _array = nullptr ) -> array< member_var_pair_t, Size >
	{
		using array_t = array< member_var_pair_t, Size >;
		if constexpr( Size == 0 ) return array_t{};

		using extractor_t = typename Ty:: template variable_extractor< Layer >;
		static constexpr auto& extraction = extractor_t::kMember;
		if( _array )
		{
			uint8_t access;
			if constexpr( extraction.holder->kFlags & cMember::eType::kStatic )
				access = _xxx_sk_get_offset_access< Ty >( offset_of< Ty, extraction.point >() );
			else
				access = _xxx_sk_get_offset_access< Ty >( offset_of< Ty, extraction.holder->get_raw() >() );

			( *_array )[ Layer ] = member_var_pair_t{
				extraction.name, cMemberVariable{ extraction.name, *extraction.holder, access }
			};
		}
		if constexpr( Layer == 0 )
		{
			if( _array ) return *_array;
			return array_t{};
		}
		else
		{
			if( _array )
				return _xxx_process_member_variables< Ty, Size, Layer - 1 >( _array );

			uint8_t access;
			if constexpr( extraction.holder->kFlags & cMember::eType::kStatic )
				access = _xxx_sk_get_offset_access< Ty >( offset_of< Ty, extraction.point >() );
			else
				access = _xxx_sk_get_offset_access< Ty >( offset_of< Ty, extraction.holder->get_raw() >() );

			array_t array{};
			array[ Layer ] = member_var_pair_t{
				extraction.name, cMemberVariable{ extraction.name, *extraction.holder, access }
			};
			return _xxx_process_member_variables< Ty, Size, Layer - 1 >( &array );
		}
	} // _xxx_process_member_variables

} // sk::Reflection::

namespace sk
{
	class iClass
	{
	public:
		iClass( void ) = default;
		virtual ~iClass( void ) = default;
		virtual const iRuntimeClass& getClass    ( void ) const = 0;
		virtual const type_hash&     getClassType( void ) = 0;
		virtual       std::string    getClassName( void ) = 0;

		// Virtual member reflection
		virtual auto getVariables() const
			-> Reflection::member_var_map_ref_t{ return {}; }
		virtual auto getFunctions() const
			-> Reflection::member_func_map_ref_t = 0;
		virtual auto getVariable( const str_hash& _hash ) const
			-> Reflection::member_var_ptr_t = 0;
		virtual auto getFunction( const str_hash& _hash ) const
			-> Reflection::member_func_ptr_t = 0;
		virtual auto getFunctionOverloads( const str_hash& _hash ) const
			-> Reflection::member_func_range_t = 0;
		virtual auto getFunction( const str_hash& _hash, const type_hash& _args ) const
			-> Reflection::member_func_ptr_t = 0;

		// Here for placeholder and template help.
		
		static constexpr auto& kClass = kInvalidClass;
		// Recommended to use Ty::kClass instead.
		static constexpr auto& getStaticClass( void ) { return kClass; }
		static constexpr auto& getStaticType( void ) { return kClass.getType(); }
		static auto getStaticName( void ) { return kClass.getName(); }
	};
} // sk::

SK_CLASS( Test )
{
	SK_CLASS_BODY( Test )
	
	uint64_t member_0 = 0; SK_VARIABLE( member_0 )
	uint32_t member_1 = 0; SK_VARIABLE( member_1 )
	uint32_t member_2 = 0; SK_VARIABLE( member_2 )

sk_public:

sk_protected:

	static uint32_t member_3; SK_VARIABLE( member_3 )
	static constexpr uint32_t member_4 = 10; SK_VARIABLE( member_4 )
	uint32_t member_5 = 0; SK_VARIABLE( member_2 )

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

	virtual auto test_func4( void ) -> uint8_t { return member_4; }

	// TODO: Figure out a way to register virtual functions. Overridable tag?
	SK_FUNCTION( test_func  )
	SK_FUNCTION( test_func2 )
	SK_FUNCTION( test_func3 )

	// Future example: SK_FUNCTION( test_func, Visibility=Private )
};

REGISTER_CLASS( Test )
