

#include "SerializedObject.h"

#include <sk/Containers/String.h>

using namespace sk;

namespace 
{
    // From: https://en.cppreference.com/w/cpp/utility/variant/visit
    template< class... Ts >
    struct sVisitor : Ts... { using Ts::operator()...; };
} // ::

cSerializedObject::cSerializedObject( const type_info_t _serialized_type, const size_t _element_count )
: m_serialized_type_( _serialized_type )
, m_element_count_( _element_count )
, m_json_builder_( Math::ceilToPow2( m_serialized_type_->size ) )
{
}

cSerializedObject::cSerializedObject() = default;

cSerializedObject::cSerializedObject( const simdjson::dom::object& _object )
{
    auto& type_manager = Reflection::cType_Manager::get();

    auto header_object = _object.at_key( "header" ).get_object();
    auto data_object   = _object.at_key( "data" ).get_object();
    auto member_array  = _object.at_key( "members" ).get_array();
}

cSerializedObject::cSerializedObject( const cSerializedObject& _other )
: m_serialized_type_( _other.m_serialized_type_ )
, m_element_count_( _other.m_element_count_ )
, m_json_builder_( _other.has_completed_json() ? _other.m_json_builder_.size() : Math::ceilToPow2( m_serialized_type_->size ) )
{
    if( IsArray() )
    {
        std::visit( [&]< class T0 >( const T0& _array )
        {
            using element_t = std::remove_cvref_t< std::remove_pointer_t< T0 > >;
            
            auto ptr = SK_NEW( element_t, m_element_count_ );
            m_element_data_ = ptr;
            std::copy_n( _array, m_element_count_, ptr );
        }, _other.m_element_data_ );
    }
    else
    {
        m_info_   = _other.m_info_;
        m_values_ = _other.m_values_;
    }
    
    if( _other.has_completed_json() )
        m_json_builder_.append_raw( _other.m_json_builder_ );
}

cSerializedObject::cSerializedObject( cSerializedObject&& _other ) noexcept
: m_serialized_type_( _other.m_serialized_type_ )
, m_element_count_( _other.m_element_count_ )
, m_element_data_( _other.m_element_data_ )
, m_json_builder_( std::move( _other.m_json_builder_ ) )
, m_info_( std::move( _other.m_info_ ) )
, m_values_( std::move( _other.m_values_ ) )
{
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
    
    if( IsArray() )
    {
        std::visit( [&]< class T0 >( const T0& _array )
        {
            using element_t = std::remove_pointer_t< T0 >;
            
            auto ptr = SK_NEW( element_t, m_element_count_ );
            m_element_data_ = ptr;
            std::copy_n( _array, m_element_count_, ptr );
        }, _other.m_element_data_ );
    }
    else
    {
        m_info_   = _other.m_info_;
        m_values_ = _other.m_values_;
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
    m_element_data_    = _other.m_element_data_;
    m_json_builder_    = std::move( _other.m_json_builder_ );
    m_info_            = std::move( _other.m_info_ );
    m_values_          = std::move( _other.m_values_ );
    
    _other.m_element_count_ = 0;
    
    return *this;

}

auto cSerializedObject::CreateForWrite() -> cShared_ptr< cSerializedObject >
{
    auto object = sk::make_shared< cSerializedObject >();
    object->BeginWrite();
    return object;
}

bool cSerializedObject::IsArray() const
{
    return m_element_count_ != 0;
}

void cSerializedObject::Reset()
{
    ClearCache();
    
    m_serialized_type_ = nullptr;
    if( IsArray() )
    {
        std::visit( []( auto& _array ){
            SK_DELETE( _array );
            _array = nullptr;
        }, m_element_data_ );
        m_element_count_ = 0;
    }
    else
    {
        m_info_.clear();
        m_values_.clear();
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
        _builder.start_object();

        // Write type header.
        _builder.escape_and_append_with_quotes( "header" );
        _builder.append_colon();
        _builder.start_object();

        if( m_serialized_type_ != nullptr )
        {
            _builder.append_key_value( "valid", true );
            _builder.append_key_value( "name",  m_serialized_type_->name );
            _builder.append_key_value( "id",    m_serialized_type_->hash.value() );
        }
        else
            _builder.append_key_value( "valid", false );

        _builder.end_object();

        _builder.escape_and_append_with_quotes( "bases" );
        _builder.append_colon();
        _builder.start_array();

        for( auto& base : m_bases_ )
            base->CreateJSON( _builder );

        _builder.end_array();

        _builder.escape_and_append_with_quotes( "data" );
        _builder.append_colon();

        if( IsArray() )
            create_json_array( m_json_builder_ );
        else
            create_json_object( m_json_builder_ );

        _builder.end_object();
    }
    
    _builder.append_raw( m_json_builder_ );
}

auto cSerializedObject::CreateBinary() -> const std::span< std::byte >&
{
    // TODO: Binary export.
    return m_binary_cache_;
}

void cSerializedObject::ClearCache()
{
    m_json_builder_.clear();
}

void cSerializedObject::BeginWrite( iClass* _this, const bool _reset )
{
    m_this_ = _this;
    
    if( _reset )
        Reset();
}

void cSerializedObject::AddBase( const cShared_ptr< cSerializedObject >& _base_info )
{
    m_bases_.emplace_back( _base_info );
}

void cSerializedObject::WriteData( const cStringID& _name, value_t&& _value )
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

void cSerializedObject::EndWrite()
{
    ClearCache();
    
    m_this_ = nullptr;
    
    if( m_raw_offset_ != 0 )
    {
        for( auto& info : m_info_ )
        {
            if( ( info.flags & sValueInfo::kRealMember ) == 0 )
                continue;
            
            info.offset += m_raw_offset_;
        }
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

void cSerializedObject::create_json_array( json_builder_t& _builder )
{
    auto& builder = _builder;
    
    builder.start_array();
    
    auto visitor = sVisitor{
        [&]( cShared_ptr< cSerializedObject >& _value )
        {
            _value->CreateJSON( builder );
        },
        [&]( const bool& _value )
        {
            builder.append( _value );
        },
        [&]( const int64_t& _value )
        {
            builder.append( _value );
        },
        [&]( const uint64_t& _value )
        {
            builder.append( _value );
        },
        [&]( const double& _value )
        {
            builder.append( _value );
        },
        [&]( const std::string& _value )
        {
            builder.escape_and_append_with_quotes( _value );
        },
    };
    
    std::visit( [&]( auto& _array ){
        for( size_t i = 0; i < m_element_count_; i++ )
        {
            visitor( _array[ i ] );
            if( i != m_element_count_ - 1 )
                builder.append_comma();
        }
    }, m_element_data_ ); 
    
    builder.end_array();
}

void cSerializedObject::handle_info( json_builder_t& _builder, const sValueInfo& _info )
{
    auto& builder = _builder;
    auto& info    = _info;
    auto& element = m_values_[ info.value_index ];
    
    auto visitor = sVisitor{
        [&]( cShared_ptr< cSerializedObject >& _value )
        {
            _value->CreateJSON( builder );
        },
        [&]( const bool& _value )
        {
            builder.append( _value );
        },
        [&]( const int64_t& _value )
        {
            builder.append( _value );
        },
        [&]( const uint64_t& _value )
        {
            builder.append( _value );
        },
        [&]( const double& _value )
        {
            builder.append( _value );
        },
        [&]( const std::string& _value )
        {
            builder.escape_and_append_with_quotes( _value );
        },
    };
    
    std::visit( visitor, element ); 
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
