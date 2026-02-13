

#include "SerializedObject.h"

#include <sk/Assets/Asset.h>
#include <sk/Assets/Management/Asset_Manager.h>
#include <sk/Containers/String.h>


using namespace sk;

cSerializedObject::cSerializedObject( const type_info_t _serialized_type, const size_t _element_count )
: m_serialized_type_( _serialized_type )
, m_element_count_( _element_count )
, m_json_builder_( Math::ceilToPow2( m_serialized_type_->size ) )
{}

cSerializedObject::cSerializedObject() = default;

cSerializedObject::cSerializedObject( const simdjson::dom::object& _object )
{
    auto& types = Reflection::cType_Manager::get().GetTypes();

    if( const auto header_value = _object.at_key( "header" ); header_value.has_value() )
    {
        // Fill header.

        if( const auto header = header_value.get_object(); header[ "valid_type" ].get_bool() )
        {
            const auto type = type_hash{ header[ "type_id" ].get_uint64() };
            if( const auto itr = types.find( type ); itr != types.end() )
                m_serialized_type_ = itr->second;
        }
    }

    if( const auto bases_value = _object.at_key( "bases" ); bases_value.has_value() )
    {
        // Fill bases.
        for( auto base : bases_value.get_array() )
            m_bases_.emplace_back( base.get_object() );
    }

    if( const auto data_object = _object.at_key( "data" ).get_object(); data_object.has_value() )
    {
        // Fill data.
        for( auto [ key, value ] : data_object )
        {
            sValueInfo info{};
            info.name = info.json_safe_name = key;
            info.value_index = m_values_.size();

            handle_json_element( value );

            m_info_.emplace_back( std::move( info ) );
        }
    }

    auto member_value  = _object.at_key( "members" );
    if( member_value.has_value() )
    {
        // Fill real members.
        // TODO: Handle real members
        auto members = member_value.get_array();
    }
}

namespace
{
    template< class Ty >
    cSerializedObject::array_t handle_json_array( const simdjson::dom::array& _array )
    {
        auto buffer = SK_NEW( Ty, _array.size() );
        for( auto val = buffer; auto element : _array )
        {
            if constexpr( std::is_same_v< Ty, cSerializedObject > )
            {
                if( element.type() == simdjson::dom::element_type::ARRAY )
                    *val++ = cSerializedObject( element.get_array() );
                else if( element.type() == simdjson::dom::element_type::OBJECT )
                    *val++ = cSerializedObject( element.get_object() );
            }
            else if constexpr( std::is_same_v< Ty, std::string > )
                *val++ = std::string{ element.get< std::string_view >().value() };
            else
                *val++ = element.get< Ty >();
        }

        return buffer;
    }
} // ::

cSerializedObject::cSerializedObject( const simdjson::dom::array& _array )
{
    using element_type = simdjson::dom::element_type;

    if( _array.size() == 0 )
        m_element_count_ = 0;
    else
    {
        m_element_count_ = _array.size();
        switch( _array.at( 0 ).type().value() )
        {
        case element_type::ARRAY:  m_element_data_ = handle_json_array< cSerializedObject >( _array ); break;
            // TODO: Handle if it's a vector or asset metadata
        case element_type::OBJECT: m_element_data_ = handle_json_array< cSerializedObject >( _array ); break;
        case element_type::INT64:  m_element_data_ = handle_json_array< int64_t  >( _array ); break;
        case element_type::UINT64: m_element_data_ = handle_json_array< uint64_t >( _array ); break;
        case element_type::DOUBLE: m_element_data_ = handle_json_array< double   >( _array ); break;
        case element_type::STRING: m_element_data_ = handle_json_array< std::string >( _array ); break;
        case element_type::BOOL:   m_element_data_ = handle_json_array< bool >( _array ); break;
        case element_type::NULL_VALUE: break;
        }
    }
}

cSerializedObject::cSerializedObject( const cSerializedObject& _other )
: m_serialized_type_( _other.m_serialized_type_ )
, m_element_count_( _other.m_element_count_ )
, m_json_builder_( _other.has_completed_json() ? _other.m_json_builder_.size() : json_builder_t::DEFAULT_INITIAL_CAPACITY )
{
    if( _other.IsArray() )
    {
        std::visit( [&]< class V >( const V& _array )
        {
            if constexpr( !std::is_same_v< V, std::monostate > )
            {
                using element_t = std::remove_cvref_t< std::remove_pointer_t< V > >;

                auto ptr = SK_NEW( element_t, m_element_count_ );
                m_element_data_ = ptr;
                std::copy_n( _array, m_element_count_, ptr );
            }
        }, _other.m_element_data_ );
    }
    else
    {
        m_info_   = _other.m_info_;
        m_values_ = _other.m_values_;
        m_bases_  = _other.m_bases_;
    }
    
    if( _other.has_completed_json() )
        m_json_builder_.append_raw( _other.m_json_builder_ );
}

cSerializedObject::cSerializedObject( cSerializedObject&& _other ) noexcept
: m_serialized_type_( _other.m_serialized_type_ )
, m_element_count_( _other.m_element_count_ )
, m_element_data_( _other.m_element_data_ )
, m_json_builder_( std::move( _other.m_json_builder_ ) )
, m_bases_( std::move( _other.m_bases_ ) )
, m_info_( std::move( _other.m_info_ ) )
, m_values_( std::move( _other.m_values_ ) )
{
    _other.m_element_data_  = std::monostate{};
    _other.m_element_count_ = 0;
}

cSerializedObject::~cSerializedObject()
{
    Reset();
}

cSerializedObject& cSerializedObject::operator=( const cSerializedObject& _other )
{
    if( &_other == this )
        return *this;
    
    Reset();
    
    m_serialized_type_ = _other.m_serialized_type_;
    m_element_count_   = _other.m_element_count_;
    
    if( _other.IsArray() )
    {
        std::visit( [&]< class V >( const V& _array )
        {
            if constexpr( !std::is_same_v< V, std::monostate > )
            {
                using element_t = std::remove_pointer_t< V >;

                auto ptr = SK_NEW( element_t, m_element_count_ );
                m_element_data_ = ptr;
                std::copy_n( _array, m_element_count_, ptr );
            }
        }, _other.m_element_data_ );
    }
    else
    {
        m_info_   = _other.m_info_;
        m_values_ = _other.m_values_;
        m_bases_  = _other.m_bases_;
    }
    
    if( _other.has_completed_json() )
        m_json_builder_.append_raw( _other.m_json_builder_ );
    
    return *this;
}

cSerializedObject& cSerializedObject::operator=( cSerializedObject&& _other ) noexcept
{
    Reset();
    
    m_serialized_type_ = _other.m_serialized_type_;
    m_element_count_   = _other.m_element_count_;
    m_element_data_    = _other.m_element_data_ ;
    m_json_builder_    = std::move( _other.m_json_builder_ );
    m_info_            = std::move( _other.m_info_ );
    m_values_          = std::move( _other.m_values_ );
    m_bases_           = std::move( _other.m_bases_ );

    _other.m_element_data_  = std::monostate{};
    _other.m_element_count_ = 0;
    
    return *this;

}

bool cSerializedObject::IsArray() const
{
    return m_element_data_.index() != 0;
}

void cSerializedObject::Reset()
{
    ClearCache();
    
    m_serialized_type_ = nullptr;
    if( IsArray() )
    {
        std::visit( []< class V >( V& _array ){
            if constexpr( !std::is_same_v< V, std::monostate > )
                SK_DELETE( _array );
        }, m_element_data_ );
        m_element_count_ = 0;
        m_element_data_  = std::monostate{};
    }
    else
    {
        m_info_.clear();
        m_values_.clear();
        m_bases_.clear();
    }
}

auto cSerializedObject::CreateJSON() -> std::string_view
{
    CreateJSON( m_json_builder_ );

    return m_json_builder_;
}

void cSerializedObject::CreateJSON( json_builder_t& _builder )
{
    if( !has_completed_json() )
    {
        if( !IsArray() )
        {
            m_json_builder_.start_object();

            // Write type header.
            m_json_builder_.escape_and_append_with_quotes( "header" );
            m_json_builder_.append_colon();
            m_json_builder_.start_object(); // Header body start

            if( m_serialized_type_ != nullptr )
            {
                m_json_builder_.append_key_value( "valid_type", true );
                m_json_builder_.append_comma();
                m_json_builder_.append_key_value( "type_name",  m_serialized_type_->name );
                m_json_builder_.append_comma();
                m_json_builder_.append_key_value( "type_id",    m_serialized_type_->hash.value() );
            }
            else
                m_json_builder_.append_key_value( "valid_type", false );

            m_json_builder_.end_object(); // Header body end
            m_json_builder_.append_comma();

            if( m_serialized_type_ != nullptr && !m_bases_.empty() )
            {
                m_json_builder_.escape_and_append_with_quotes( "bases" );
                m_json_builder_.append_colon();
                m_json_builder_.start_array(); // Bases body start

                for( size_t i = 0; i < m_bases_.size(); i++ )
                {
                    m_bases_[ i ].CreateJSON( m_json_builder_ );
                    if( i != m_bases_.size() - 1 )
                        m_json_builder_.append_comma();
                }

                m_json_builder_.end_array(); // Bases body end
                m_json_builder_.append_comma();
            }

            m_json_builder_.escape_and_append_with_quotes( "data" );
            m_json_builder_.append_colon();

            create_json_object( m_json_builder_ );

            m_json_builder_.end_object();
        }
        else
            create_json_array( m_json_builder_ );
    }

    if( &_builder != &m_json_builder_ )
        _builder.append_raw( m_json_builder_ );
}

auto cSerializedObject::CreateBinary() -> std::span< std::byte >
{
    // TODO: Binary export.
    return m_binary_cache_;
}

void cSerializedObject::ClearCache()
{
    m_json_builder_.clear();
}

auto cSerializedObject::GetType() const -> type_info_t
{
    return m_serialized_type_;
}

auto cSerializedObject::GetRuntimeClass() const -> class_info_t
{
    return m_serialized_type_->as_class_info()->runtime_class;
}

auto cSerializedObject::GetBase( type_info_t _type ) -> std::optional< std::reference_wrapper< cSerializedObject > >
{
    if( const auto itr = std::ranges::find_if( m_bases_, [ &_type ]( const auto& _base ) -> bool{ return _base.m_serialized_type_ == _type; } );
        itr != m_bases_.end() )
        return std::ref( *itr );

    return std::nullopt;
}

auto cSerializedObject::ConstructClass() -> iClass*
{
    return GetRuntimeClass()->CreateSerialized( *this );
}

auto cSerializedObject::ConstructSharedClass() -> cShared_ptr< iClass >
{
    return GetRuntimeClass()->CreateSharedSerialized( *this );
}

void cSerializedObject::BeginRead( iClass* _this )
{
    m_this_ = _this;
}

auto cSerializedObject::ReadDataRaw( const cStringID& _name ) -> std::optional< std::reference_wrapper< value_t > >
{
    // TODO: Print error
    auto pretty_name = cStringID{ MakeJsonSafeName( _name ) };
    auto itr = std::ranges::find_if( m_info_, [&_name]( auto& _info ){ return _info.json_safe_name == _name; } );
    if( itr != m_info_.end() )
        return std::ref( m_values_[ itr->value_index ] );

    return std::nullopt;
}

auto cSerializedObject::GetArraySize() const -> size_t
{
    return m_element_count_;
}

void cSerializedObject::EndRead()
{
    m_this_ = nullptr;
}

void cSerializedObject::BeginWrite( iClass* _this, const bool _reset )
{
    m_this_ = _this;
    
    if( _reset )
        Reset();
}

void cSerializedObject::AddBase( cSerializedObject&& _base_info )
{
    m_bases_.emplace_back( std::move( _base_info ) );
}

void cSerializedObject::EndWrite()
{
    ClearCache();
    
    m_this_ = nullptr;
    
    for( auto& info : m_info_ )
    {
        if( ( info.flags & sValueInfo::kRealMember ) == 0 )
            continue;

        info.offset += m_raw_offset_;
    }
}

std::string cSerializedObject::MakeJsonSafeName( const std::string_view& _name )
{
    std::stringstream ss;

    for( auto c : _name )
    {
        bool valid = true;
        switch( c )
        {
        case '_':
        case '-':
        case '.':
            valid = true; break;
        case ' ':
            c = '_'; break;
        default:
            valid = std::isalnum( c );
        }

        if( !valid )
            continue;

        ss << c;
    }

    return ss.str();
}

void cSerializedObject::create_json_object( json_builder_t& _builder )
{
    // TODO: Store member variables.
    auto& builder = _builder;
    
    builder.start_object();

    for( auto& info : m_info_ )
    {
        builder.escape_and_append_with_quotes( info.json_safe_name );
        builder.append_colon();
        
        handle_info( builder, info );
        
        if( &info != &m_info_.back() )
            builder.append_comma();
    }
    
    builder.end_object();
}

namespace
{
        void handle_value( cSerializedObject::json_builder_t& _builder, auto& _value )
    {
        auto& builder = _builder;

        sVisitor{
            [&]( std::monostate& )
            {
                builder.append_null();
            },
            [&]( cSerializedObject& _val )
            {
                _val.CreateJSON( builder );
            },
            [&]( const bool& _val )
            {
                builder.append( _val );
            },
            [&]( const int64_t& _val )
            {
                builder.append( _val );
            },
            [&]( const uint64_t& _val )
            {
                builder.append( _val );
            },
            [&]( const double& _val )
            {
                builder.append( _val );
            },
            [&]( const std::string& _val )
            {
                builder.escape_and_append_with_quotes( _val );
            },
            [&]< class Ty >( const Math::cVector< 2, Ty >& _vector )
            {
                builder.start_object();
                builder.append_key_value( "x", _vector[ 0 ] );
                builder.append_comma();
                builder.append_key_value( "y", _vector[ 1 ] );
                builder.end_object();
            },
            [&]< class Ty >( const Math::cVector< 3, Ty >& _vector )
            {
                builder.start_object();
                builder.append_key_value( "x", _vector[ 0 ] );
                builder.append_comma();
                builder.append_key_value( "y", _vector[ 1 ] );
                builder.append_comma();
                builder.append_key_value( "z", _vector[ 2 ] );
                builder.end_object();
            },
            [&]< class Ty >( const Math::cVector< 4, Ty >& _vector )
            {
                builder.start_object();
                builder.append_key_value( "x", _vector[ 0 ] );
                builder.append_comma();
                builder.append_key_value( "y", _vector[ 1 ] );
                builder.append_comma();
                builder.append_key_value( "z", _vector[ 2 ] );
                builder.append_comma();
                builder.append_key_value( "w", _vector[ 3 ] );
                builder.end_object();
            },
            [&]( const cWeak_Ptr< cAsset_Meta >& _meta )
            {
                builder.start_object();
                builder.append_key_value( "asset_uuid", _meta.is_valid() ? _meta->GetUUID().ToString() : "" );
                builder.append_comma();
                builder.append_key_value( "asset_path", _meta.is_valid() ? _meta->GetPath().string() : "" );
                builder.end_object();
            }
        }( _value );
    }
} // ::

void cSerializedObject::create_json_array( json_builder_t& _builder )
{
    auto& builder = _builder;
    
    builder.start_array();

    if( m_element_count_ > 0 )
    {
        std::visit( [&]< class V >( V& _array ){
            if constexpr( !std::is_same_v< V, std::monostate > )
            {
                for( size_t i = 0; i < m_element_count_; i++ )
                {
                    handle_value( _builder, _array[ i ] );
                    if( i != m_element_count_ - 1 )
                        builder.append_comma();
                }
            }
        }, m_element_data_ );
    }

    builder.end_array();
}

void cSerializedObject::handle_info( json_builder_t& _builder, const sValueInfo& _info )
{
    auto& info    = _info;
    auto& element = m_values_[ info.value_index ];

    sVisitor visitor{
        [&]( auto& _value )
        {
            handle_value( _builder, _value );
        }
    };

    std::visit( visitor, element );
}

namespace
{
    template< class Ty >
    auto get_vector_value( const simdjson::dom::object& _object ) -> cSerializedObject::value_t
    {
        switch( _object.size() )
        {
        case 2: return Math::cVector2< Ty >{ _object.at_key( "x" ).get< Ty >().value(), _object.at_key( "y" ).get< Ty >().value() };
        case 3: return Math::cVector3< Ty >{ _object.at_key( "x" ).get< Ty >().value(), _object.at_key( "y" ).get< Ty >().value(), _object.at_key( "z" ).get< Ty >().value() };
        case 4: return Math::cVector4< Ty >{ _object.at_key( "x" ).get< Ty >().value(), _object.at_key( "y" ).get< Ty >().value(), _object.at_key( "z" ).get< Ty >().value(), _object.at_key( "w" ).get< Ty >().value() };
        default: return {};
        }
    }

    auto get_meta_value( const simdjson::dom::object& _object ) -> cSerializedObject::value_t
    {
        auto& manager = cAsset_Manager::get();

        auto uuid_str = _object.at_key( "asset_uuid" ).get_string();
        cUUID uuid;
        if( uuid_str.has_value() && uuid_str.value().size() > 32 )
            uuid = cUUID::FromString( uuid_str.value() );
        else
            uuid = cUUID::kInvalid;

        cWeak_Ptr< cAsset_Meta > meta;
        if( uuid == cUUID::kInvalid )
            meta = nullptr;
        else
            meta = manager.getAsset( uuid );

        if( meta == nullptr )
            meta = manager.GetAssetByPath( std::filesystem::path{ _object.at_key( "asset_path" ).get_string().value() } );

        return meta;
    }
} // ::

void cSerializedObject::handle_json_element( const simdjson::dom::element& _element )
{
    using element_type = simdjson::dom::element_type;

    switch( _element.type() )
    {
    case element_type::ARRAY:
        m_values_.emplace_back( cSerializedObject( _element.get_array() ) );
        break;
    case element_type::OBJECT:
        {
            auto object = _element.get_object();
            if( auto x = object.at_key( "x" ); x.has_value() )
            {
                // It's a vector type, so we handle it separately.
                // TODO: Actually handle the types
                switch( x.type() )
                {
                case element_type::DOUBLE: m_values_.emplace_back( get_vector_value< double   >( object.value() ) );
                case element_type::INT64:  m_values_.emplace_back( get_vector_value< double  >( object.value() ) );
                case element_type::UINT64: m_values_.emplace_back( get_vector_value< double >( object.value() ) );
                default: break;
                }
            }
            else if( object.begin().key().starts_with("asset" ) )
                m_values_.emplace_back( get_meta_value( object.value() ) );
            else
                m_values_.emplace_back( cSerializedObject( object.value() ) );
        }
        break;
    case element_type::INT64:
        m_values_.emplace_back( _element.get_int64() );
        break;
    case element_type::UINT64:
        m_values_.emplace_back( _element.get_uint64() );
        break;
    case element_type::DOUBLE:
        m_values_.emplace_back( _element.get_double() );
        break;
    case element_type::STRING:
        m_values_.emplace_back( std::string{ _element.get_string().value() } );
        break;
    case element_type::BOOL:
        m_values_.emplace_back( _element.get_bool() );
        break;
    case element_type::NULL_VALUE:
        // TODO: Actually handle null
        m_values_.emplace_back( false );
        break;
    }
}

void cSerializedObject::_writeData( const cStringID& _name, value_t&& _value )
{
    const auto json_safe_name = MakeJsonSafeName( _name.view() );

    auto index = m_info_.size();
    m_values_.emplace_back( std::move( _value ) );

    sValueInfo info{
        .name = _name.view(),
        .json_safe_name = std::string_view{ json_safe_name },
        .offset = 0,
        .value_index = static_cast< uint32_t >( index ),
        .flags  = 0,
        .type   = nullptr,
    };

    m_info_.emplace_back( std::move( info ) );
}

auto cSerializedObject::get_value_at_offset( type_info_t _type, size_t _offset ) -> std::optional< value_t >
{
    _offset += m_raw_offset_;
    const auto itr = std::ranges::find_if( m_info_, [&]( auto& _info )
    {
        if( _info.offset != _offset )
            return false;
        
        SK_BREAK_RET_IF( sk::Severity::kEngine,
            _info.type != _type, "Error: The type at this offset doesn't have this value", false )
        return true;
    } );
    
    SK_BREAK_RET_IF( sk::Severity::kEngine,
        itr == m_info_.end(), "Error: No value at this offset.", std::nullopt )
    
    auto& value = m_values_[ itr->value_index ];
    
    return value;
}

bool cSerializedObject::has_completed_json() const
{
    // The JSON builder will never have any data unless it's completed.
    return m_json_builder_.size() > 0;
}
