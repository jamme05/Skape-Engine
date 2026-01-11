/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Graphics/Buffer/Unsafe_Buffer.h>

#include <Reflection/Types.h>

#include "Misc/Smart_Ptrs.h"

// TYPE UNSAFE BUFFER

namespace sk::Graphics
{
    class cDynamic_Buffer
    {
        // TODO: Combine the functionality of these in some way.
        template< class Itr >
        cDynamic_Buffer( const std::string& _name, Buffer::eType _type, bool _is_normalized, const size_t _size, Itr _begin, Itr _end )
        : m_buffer_type_( _type )
        {
            using type = std::iter_value_t< Itr >;
            static constexpr auto size = sizeof( type );
            
            m_raw_size_ = size * _size;
            m_type_size_ = size;

            if constexpr( kValidType< type > )
                m_item_type_ = &kTypeInfo< type >;
            else
                m_item_type_ = nullptr;
            
            m_buffer_ = std::make_unique< cUnsafe_Buffer >( _name, m_raw_size_, sizeof( type ), _type, _is_normalized, false );

            // Initialize buffer.
            auto ptr = static_cast< type* >( m_buffer_->Data() );
            for( size_t i = 0; _begin != _end; ++i, ++_begin, ++i )
                ::new( ptr + i ) type( *_begin );
        } // cDynamic_Buffer

        cDynamic_Buffer( const std::string& _name, Buffer::eType _type, bool _is_normalized, size_t _raw_size, size_t _raw_stride ); // cDynamic_Buffer
        
    public:
        cDynamic_Buffer();

        cDynamic_Buffer( const std::string& _name, Buffer::eType _type, bool _is_normalized ); // cDynamic_Buffer

        template< class Ty >
        cDynamic_Buffer( const std::string& _name, const Buffer::eType _type, const bool _is_normalized, const size_t _size, Ty&& _element )
        : cDynamic_Buffer( _name, _type, _is_normalized, _size * sizeof( Ty ), sizeof( Ty ) )
        {
            if constexpr( kValidType< Ty > )
                AlignAs< Ty >( false );
            else
                UnsafeAlignAs< Ty >( false );

            Fill( std::forward< Ty >( _element ) );
        } // cDynamic_Buffer

        template< class Itr >
        cDynamic_Buffer( const std::string& _name, Buffer::eType _type, Itr _begin, Itr _end, bool _is_normalized )
        : cDynamic_Buffer( _name, _type, _is_normalized, std::distance( _begin, _end ), _begin, _end )
        {} // cDynamic_Buffer

        template< class Ty >
        cDynamic_Buffer( const std::string& _name, Buffer::eType _type, bool _is_normalized, std::initializer_list< Ty > _list )
        : cDynamic_Buffer( _name, _type, _is_normalized, _list.size(), _list.begin(), _list.end() )
        {} // cDynamic_Buffer

        cDynamic_Buffer( const cDynamic_Buffer& _other );
        cDynamic_Buffer( cDynamic_Buffer&& _other ) noexcept;

        ~cDynamic_Buffer();
        
        cDynamic_Buffer& operator=( const cDynamic_Buffer& _other );
        cDynamic_Buffer& operator=( cDynamic_Buffer&& _other ) noexcept;

        [[ nodiscard ]] auto GetSize    () const -> size_t;
        [[ nodiscard ]] auto GetType    () const -> Buffer::eType;
        [[ nodiscard ]] auto GetItemSize() const -> size_t;
        [[ nodiscard ]] auto GetItemType() const -> type_info_t;
        [[ nodiscard ]] auto GetBuffer  () const -> iUnsafe_Buffer&;
        [[ nodiscard ]] bool IsValid    () const;

        // Not recommended to be used directly.
        void UnsafeAlignAs( size_t _align, bool _clear );

        // Not recommended to be used.
        template< class Ty >
        void UnsafeAlignAs( bool _clear = true );

        template< reflected Ty > requires ( !std::is_polymorphic_v< Ty > )
        void AlignAs( bool _clear = true );

        // Not recommended to be used directly.
        void AlignAs( type_info_t _type, bool _clear = true );

        void* RawData();
        void* RawData() const;
        
        template< class Ty >
        Ty* Data();
        
        template< class Ty >
        Ty* Data() const;
        
        // TODO: Add more ways to set data.
        template< class Ty >
        void Set( size_t _index, Ty&& _element );

        template< class Ty >
        void Fill( Ty&& _value );

        template< class Ty >
        void Fill( Ty* _first, Ty* _last, Ty&& _value );

        void Resize( size_t _size );

    private:
        template< class Ty >
        void validate_type() const;
        
        using buffer_t = std::unique_ptr< iUnsafe_Buffer >;
        
        // The outwards size of this buffer. Get the size from the unsafe buffer for the capacity.
        size_t         m_raw_size_;
        // Uses both.
        Buffer::eType  m_buffer_type_;
        size_t         m_type_size_;
        type_info_t    m_item_type_;
        
        buffer_t       m_buffer_;
    };

    template< class Ty >
    void cDynamic_Buffer::UnsafeAlignAs( const bool _clear )
    {
        SK_WARNING( sk::Severity::kGraphics | 500,
            "Warning: Setting a non reflected alignment is unsafe." )
        
        m_item_type_ = nullptr;
        
        UnsafeAlignAs( sizeof( Ty ), _clear );
    }

    template< reflected Ty > requires ( !std::is_polymorphic_v< Ty > )
    void cDynamic_Buffer::AlignAs( const bool _clear )
    {
        m_item_type_ = &kTypeInfo< Ty >;
        
        UnsafeAlignAs( sizeof( Ty ), _clear );
    }
    
    template< class Ty >
    void cDynamic_Buffer::validate_type() const
    {
        if constexpr( sk::kValidType< Ty > )
        {
            SK_ERR_IF( m_item_type_ != nullptr && m_item_type_ != &kTypeInfo< Ty >,
                TEXT( "ERROR: Buffer {} is currently set to use type {}. But type {} was provided instead.",
                    m_buffer_->GetName(), m_item_type_->name, kTypeInfo< Ty >.name ) )
        }
        else
        {
            SK_BREAK_IF( sk::Severity::kGraphics | 50, m_item_type_ != nullptr,
                TEXT( "WARNING: Buffer {} is currently set to use type {}. But type an unreflected type was provided instead.",
                    m_buffer_->GetName(), m_item_type_->name ) )

            SK_BREAK_RET_IF( sk::Severity::kGraphics | 20, m_type_size_ != sizeof( Ty ),
                TEXT( "ERROR: Buffer {} is currently set to use types with size {}. But a type with size of {} was provided instead.",
                    m_buffer_->GetName(), m_type_size_, sizeof( Ty ) ) )
        }
    }

    template< class Ty >
    Ty* cDynamic_Buffer::Data()
    {
        SK_ERR_IF( m_buffer_ == nullptr,
            "ERROR: Trying to get data in uninitialized buffer." )

        validate_type< Ty >();

        return static_cast< Ty* >( m_buffer_->Data() );
    }

    template< class Ty >
    Ty* cDynamic_Buffer::Data() const
    {
        SK_ERR_IF( m_buffer_ == nullptr,
        "ERROR: Trying to get data in uninitialized buffer." )

        validate_type< Ty >();

        return static_cast< Ty* >( m_buffer_->Data() );
    }

    template< class Ty >
    void cDynamic_Buffer::Set( const size_t _index, Ty&& _element )
    {
        SK_ERR_IF( m_buffer_ == nullptr,
            "ERROR: Trying to set element in uninitialized buffer." )

        validate_type< Ty >();

        m_buffer_->UpdateSegment( std::addressof( std::forward< Ty >( _element ) ), m_type_size_, _index * m_type_size_ );
    }

    template< class Ty >
    void cDynamic_Buffer::Fill( Ty&& _value )
    {
        auto data = static_cast< Ty* >( m_buffer_->Data() );
        
        Fill( data, data + GetSize(), std::forward< Ty >( _value ) );
    }

    template< class Ty >
    void cDynamic_Buffer::Fill( Ty* _first, Ty* _last, Ty&& _value )
    {
        SK_ERR_IF( m_buffer_ == nullptr,
            "ERROR: Trying to set element in uninitialized buffer." )

        validate_type< Ty >();

        SK_ERR_IF( _first == nullptr || _last == nullptr,
            "Error: Either first or last is null." )

        auto data = static_cast< Ty* >( m_buffer_->Data() );

        SK_ERR_IF( _first < data || _last > data + GetSize() || _first > _last,
            "Error: Invalid iterators provided." )

        std::fill( _first, _last, std::forward< Ty >( _value ) );
    }
} // sk::Graphics
