

#pragma once

#include <sk/Math/Vector2.h>
#include <sk/Math/Vector3.h>
#include <sk/Math/Vector4.h>
#include <sk/Misc/Smart_Ptrs.h>
#include <sk/Misc/StringID.h>
#include <sk/Misc/Visitor.h>
#include <sk/Reflection/RuntimeClass.h>

#include <simdjson.h>

#include <variant>

namespace sk
{
    class cAsset_Meta;

    SK_CLASS( SerializedObject ), public cShared_from_this< cSerializedObject >
    {
        SK_CLASS_BODY( SerializedObject )
        friend class Serializable;
    public:
        explicit cSerializedObject( type_info_t _serialized_type, size_t _element_count = 0 );
        // This is so we can ignore the full on class reflection.

        struct sValueInfo
        {
            enum eFlags : uint8_t
            {
                kNone = 0,
                
                kRealMember = 0x01,
            };
            cStringID   name;
            cStringID   json_safe_name;
            // Binary offset, for member variable lookups.
            size_t      offset;
            uint32_t    value_index;
            uint32_t    flags;
            type_info_t type;
        };
        
        cSerializedObject();
        explicit cSerializedObject( const simdjson::dom::object& _object );
        explicit cSerializedObject( const simdjson::dom::array& _array );
        cSerializedObject( const cSerializedObject& _other );
        cSerializedObject( cSerializedObject&& _other ) noexcept;
        ~cSerializedObject() override;
        
        cSerializedObject& operator=( const cSerializedObject& _other );
        cSerializedObject& operator=( cSerializedObject&& _other ) noexcept;
        
        static auto CreateForWrite() -> cShared_ptr< cSerializedObject >;
        template< sk_class Ty >
        static auto CreateForWrite( Ty* _class ) -> cShared_ptr< cSerializedObject >;
        template< reflected Ty >
        static auto CreateArray   ( Ty* _data, size_t _size )  -> cShared_ptr< cSerializedObject >;
        
        template< class Ty, class Value >
        struct sMemberVariable
        {
            using class_type = Ty;
            using value_type = Value;
            
            consteval sMemberVariable( Value Ty::* _ptr )
            : ptr( _ptr )
            {}
            Value Ty::* ptr;
        };

        using obj_ptr  = cShared_ptr< cSerializedObject >;
        using meta_ptr = cWeak_Ptr< cAsset_Meta >;
        using value_t = std::variant< obj_ptr,  meta_ptr,  bool,  int64_t,  cVector2i64,  cVector3i64,  cVector4i64,  uint64_t,  cVector2u64,  cVector3u64,  cVector4u64,  double,  cVector2d,  cVector3d,  cVector4d,  std::string  >;
        using array_t = std::variant< obj_ptr*, meta_ptr*, bool*, int64_t*, cVector2i64*, cVector3i64*, cVector4i64*, uint64_t*, cVector2u64*, cVector3u64*, cVector4u64*, double*, cVector2d*, cVector3d*, cVector4d*, std::string* >;
        using json_builder_t = simdjson::builder::string_builder;

        [[ nodiscard ]] bool IsArray() const;
        
        void Reset();
        
        // Creators
        [[ nodiscard ]]
        auto CreateJSON  () -> std::string_view;
        void CreateJSON  ( json_builder_t& _builder );
        [[ nodiscard ]]
        auto CreateBinary() -> std::span< std::byte >;
        
        void ClearCache();

        auto GetType() const -> type_info_t;
        auto GetRuntimeClass() const -> class_info_t;

        auto GetBase( type_info_t _type ) const -> const cShared_ptr< cSerializedObject >&;
        template< reflected Ty >
        auto GetBase() const -> const cShared_ptr< cSerializedObject >&;

        auto ConstructClass() -> iClass*;
        auto ConstructSharedClass() -> cShared_ptr< iClass >;

        // Read
        void BeginRead( iClass* _this = nullptr );
        auto ReadDataRaw( const cStringID& _name ) -> std::optional< value_t >;
        template< class Ty >
        auto ReadData( const cStringID& _name ) -> std::optional< Ty >;
        auto GetArraySize() const -> size_t;
        template< class Ty >
        auto GetArray() -> std::span< Ty >;
        void EndRead ();

        template< sMemberVariable Target, reflected Value = decltype( Target )::value_type >
        void ReadIntoThis();
        
        // Write
        void BeginWrite( iClass* _this = nullptr, bool _reset = false );
        void AddBase( const cShared_ptr< cSerializedObject >& _base_info );
        void WriteData( const cStringID& _name, value_t&& _value );
        void EndWrite();

        static std::string MakeJsonSafeName( const std::string_view& _name );
        
    private:
        void create_json_object( json_builder_t& _builder );
        void create_json_array( json_builder_t& _builder );
        void handle_info( json_builder_t& _builder, const sValueInfo& _info );
        void handle_json_element( const simdjson::dom::element& _element );
        
        auto get_value_at_offset( type_info_t _type, size_t _offset ) -> std::optional< value_t >;
        bool has_completed_json () const;
        
        using info_vec_t  = std::vector< sValueInfo >;
        using value_vec_t = std::vector< value_t >;
        using obj_vec_t   = std::vector< obj_ptr >;
        using buffer_t    = std::vector< std::byte >;
        
        // Info
        type_info_t m_serialized_type_;
        // Temporary this
        iClass*     m_this_ = nullptr;
        // The offset to where the class reflection data starts.
        size_t      m_raw_offset_ = 0;
        
        // TODO: Create a separate object for handling arrays.
        // Array info:
        size_t  m_element_count_ = 0; // Zero if the object isn't an array.
        array_t m_element_data_;
        size_t  m_offset_ = 0;
        
        // Cache
        json_builder_t m_json_builder_;
        buffer_t       m_binary_cache_;

        // Base classes
        obj_vec_t m_bases_;

        // Held values
        info_vec_t  m_info_;
        value_vec_t m_values_;
    };

    template< sk_class Ty >
    auto cSerializedObject::CreateForWrite( Ty* _class )->cShared_ptr< cSerializedObject >
    {
        auto object = sk::make_shared< cSerializedObject >( kTypeInfo< Ty > );
        object->BeginWrite( _class );
        
        return object;
    }

    template< reflected Ty >
    auto cSerializedObject::CreateArray( Ty* _data, size_t _size ) -> cShared_ptr< cSerializedObject >
    {
        Ty* data_ptr = nullptr;
        if( _size == 0 )
            _size = std::numeric_limits< size_t >::max();
        else
        {
            data_ptr = SK_NEW( Ty, _size );
            std::copy_n( _data, _size, data_ptr );
        }
        auto object = sk::make_shared< cSerializedObject >( kTypeInfo< Ty >, _size );
        object->m_element_data_ = data_ptr;
        
        return object;
    }

    template< reflected Ty >
    auto cSerializedObject::GetBase() const -> const cShared_ptr< cSerializedObject >&
    {
        return GetBase( kTypeInfo< Ty > );
    }

    template< class Ty >
    auto cSerializedObject::ReadData( const cStringID& _name ) -> std::optional< Ty >
    {
        if( auto data = ReadDataRaw( _name ); data.has_value() )
        {
            return std::visit( []< class V >( V& _value ) -> std::optional< Ty >{
                if constexpr( std::is_same_v< V, Ty > )
                    return _value;
                else if constexpr( std::is_convertible_v< V, Ty > )
                    return static_cast< Ty >( _value );
                else if constexpr( std::is_integral_v< V > && std::is_enum_v< Ty > )
                    return static_cast< Ty >( _value );
                else
                    return std::nullopt;
            }, data.value() );
        }

        return std::nullopt;
    }

    template< class Ty >
    auto cSerializedObject::GetArray() -> std::span< Ty >
    {
        // TODO: Ensure that the type is correct.
        return std::span< Ty >{ std::get< Ty* >( m_element_data_ ), m_element_count_ != std::numeric_limits< size_t >::max() ? m_element_count_ : 0 };
    }

    template< cSerializedObject::sMemberVariable Target, reflected Value >
    void cSerializedObject::ReadIntoThis()
    {
        using class_type = decltype( Target )::class_type;
        
        auto& value = get_value_at_offset( kTypeInfo< Value >, offset_of< Target >() );
        
        SK_ERR_IFN( value.has_value(), "Error: No value found." )
        
        static_cast< class_type* >( m_this_ )->*Target.ptr = value;
    }
} // sk::

SK_DECLARE_CLASS( sk::SerializedObject )
