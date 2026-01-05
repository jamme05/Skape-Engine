

#pragma once

#include <variant>

#include <simdjson/simdjson.h>

#include <Misc/StringID.h>

#include <Reflection/RuntimeClass.h>

namespace sk
{
    SK_CLASS( SerializedObject )
    {
        friend class Serializable;
        
    public:
        cSerializedObject( type_info_t _serialized_type, size_t _element_count = 0 );
        // This is so we can ignore the full on class reflection.
	    CREATE_CLASS_IDENTITY_IDENTIFIERS( SerializedObject::runtime_class_SerializedObject )

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
        
        using value_t = std::variant< cShared_ptr< cSerializedObject >,  bool,  int64_t,  uint64_t,  double,  std::string  >;
        using array_t = std::variant< cShared_ptr< cSerializedObject >*, bool*, int64_t*, uint64_t*, double*, std::string* >;
        using json_builder_t = simdjson::builder::string_builder;

        [[ nodiscard ]] bool IsArray() const;
        
        void Reset();
        
        // Creators
        [[ nodiscard ]]
        auto CreateJSON  () -> std::string_view;
        void CreateJSON  ( json_builder_t& _builder );
        [[ nodiscard ]]
        auto CreateBinary() -> const std::span< std::byte >&;
        
        void ClearCache();
        
        // Read
        void BeginRead( iClass* _this );
        void EndRead  ();
        
        template< sMemberVariable Target, reflected Value = decltype( Target )::value_type >
        void ReadIntoThis();
        
        // Write
        void BeginWrite( iClass* _this = nullptr, bool _reset = false );
        void EndWrite  ();
        
    private:
        void create_json_object( json_builder_t& _builder );
        void create_json_array( json_builder_t& _builder );
        void handle_info( json_builder_t& _builder, const sValueInfo& _info );
        
        auto get_value_at_offset( type_info_t _type, size_t _offset ) -> std::optional< value_t >;
        bool has_completed_json () const;
        
        using info_vec_t  = std::vector< sValueInfo >;
        using value_vec_t = std::vector< value_t >;
        
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
        
        // Cache
        json_builder_t m_json_builder_;
        
        // Held values
        info_vec_t     m_info_;
        value_vec_t    m_values_;
    };

    template< sk_class Ty >
    auto cSerializedObject::CreateForWrite( Ty* _class )->cShared_ptr< cSerializedObject >
    {
        auto object = sk::make_shared< cSerializedObject >( kTypeInfo< Ty > );
        object->BeginWrite( _class );
        
        return object;
    }

    template< reflected Ty >
    auto cSerializedObject::CreateArray( Ty* _data, size_t _size )->cShared_ptr<cSerializedObject>
    {
        auto object = sk::make_shared< cSerializedObject >( kTypeInfo< Ty >, _size );
        auto ptr = SK_NEW( Ty, _size );
        std::copy_n( _data, _size, ptr );
        object->m_element_data_ = ptr;
        
        return object;
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

BUILD_CLASS_REFLECTION_INFO( sk::SerializedObject )
REGISTER_TYPE_INTERNAL( sk::SerializedObject::class_type, false )
