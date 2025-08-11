/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Graphics/Buffer/Unsafe_Buffer.h>

#include <Reflection/Types.h>

// TYPE UNSAFE BUFFER

namespace sk::Graphics
{
    class cDynamic_Buffer
    {
        // TODO: Combine the functionallity of these in some way.
        template< class Itr >
        cDynamic_Buffer( const std::string& _name, Buffer::eType _type, const size_t _size, Itr _begin, Itr _end )
        : m_buffer_type_( _type )
        , m_item_type_( nullptr )
        {
            using type = std::iter_value_t< Itr >;
            static constexpr auto size = sizeof( type );
            m_byte_size_ = size * _size;
            m_type_size_ = size;
            m_buffer_ = SK_SINGLE( cUnsafe_Buffer, _name, m_byte_size_, _type, false );

            // Initialize buffer.
            auto ptr = static_cast< type* >( m_buffer_->Get() );
            for( size_t i = 0; _begin != _end; ++i, ++_begin, ++i )
                ::new( ptr + i ) type( *_begin );
        } // cDynamic_Buffer

        template< class Itr >
        requires kValidType< std::iter_value_t< Itr > >
        cDynamic_Buffer( const std::string& _name, Buffer::eType _type, const size_t _size, Itr _begin, Itr _end )
        : m_buffer_type_( _type )
        {
            using type = std::iter_value_t< Itr >;
            static constexpr auto size = sizeof( type );
            m_byte_size_ = size * _size;
            m_type_size_ = size;
            m_item_type_ = &kTypeInfo< type >;
            m_buffer_ = SK_SINGLE( cUnsafe_Buffer, _name, m_byte_size_, _type, false );

            // Initialize buffer.
            auto ptr = static_cast< type* >( m_buffer_->Get() );
            for( size_t i = 0; _begin != _end; ++i, ++_begin, ++i )
                ::new( ptr + i ) type( *_begin );
        } // cDynamic_Buffer

    public:
        cDynamic_Buffer();

        cDynamic_Buffer( const std::string& _name, const Buffer::eType _type ); // cDynamic_Buffer

        template< class Ty >
        cDynamic_Buffer( const std::string& _name, const Buffer::eType _type, const size_t _size, const Ty& _element )
        : cDynamic_Buffer( _name, _type, sizeof( Ty ), _size * sizeof( Ty ), _size, &_element )
        {} // cDynamic_Buffer

        template< class Itr >
        cDynamic_Buffer( const std::string& _name, Buffer::eType _type, Itr _begin, Itr _end )
        : cDynamic_Buffer( _name, _type, std::distance( _begin, _end ), _begin, _end )
        {} // cDynamic_Buffer

        template< class Ty >
        cDynamic_Buffer( const std::string& _name, Buffer::eType _type, std::initializer_list< Ty > _list )
        : cDynamic_Buffer( _name, _type, _list.size(), _list.size(), _list.begin() )
        {} // cDynamic_Buffer

        cDynamic_Buffer( const cDynamic_Buffer& _other );
        cDynamic_Buffer( cDynamic_Buffer&& _other ) noexcept;

        ~cDynamic_Buffer() = default;

        void AlignAs( size_t _align );

        template< class Ty >
        void AlignAs();

        template< reflected Ty > requires ( !std::is_abstract_v< Ty > )
        void AlignAs();

        // TODO: Add more ways to set data.
        template< class Ty >
        void Set( size_t _index, Ty&& _element );

        template< reflected Ty >
        void Set( size_t _index, Ty&& _element );

        void Resize( size_t _size );

    private:
        // The outwards size of this buffer. Get the size from the unsafe buffer for the capacity.
        size_t          m_byte_size_;
        // Uses both.
        Buffer::eType   m_buffer_type_;
        size_t          m_type_size_;
        type_info_t     m_item_type_;

        iUnsafe_Buffer* m_buffer_;
    };

    template< class Ty >
    void cDynamic_Buffer::AlignAs()
    {
        m_item_type_ = nullptr;
        AlignAs( sizeof( Ty ) );
    }

    template< reflected Ty > requires ( !std::is_abstract_v< Ty > )
    void cDynamic_Buffer::AlignAs()
    {
        m_item_type_ = &kTypeInfo< Ty >;
        AlignAs( sizeof( Ty ) );
    }

    template< class Ty >
    void cDynamic_Buffer::Set( const size_t _index, Ty&& _element )
    {
        SK_ERR_IF( m_buffer_ == nullptr,
            "ERROR: Trying to set element in uninitialized buffer." )

        SK_BREAK_IF( sk::Severity::kGraphics | 50,
            m_item_type_ != nullptr, TEXT( "WARNING: Buffer {} is currently set to use type {}. But type an unreflected type was provided instead.", m_buffer_->GetName(), m_item_type_->name ) )

        SK_BREAK_IF_RET( sk::Severity::kGraphics | 20,
            m_type_size_ != sizeof( Ty ), TEXT( "ERROR: Buffer {} is currently set to use types with size {}. But a type with size of {} was provided instead,", m_buffer_->GetName(), m_type_size_, sizeof( Ty ) ) )

        m_buffer_->UpdateSeg( std::addressof( std::forward< Ty >( _element ) ), _index * m_type_size_, m_type_size_ );
    }

    template< reflected Ty >
    void cDynamic_Buffer::Set( const size_t _index, Ty&& _element )
    {
        SK_ERR_IF( m_buffer_ == nullptr,
            "ERROR: Trying to set element in uninitialized buffer." )

        SK_BREAK_IF_RET( sk::Severity::kGraphics | 50,
            m_item_type_ == nullptr, TEXT( "WARNING: Buffer {} is currently set to not use reflected types.", m_buffer_->GetName() ) )

        SK_BREAK_IF_RET( sk::Severity::kGraphics | 20, m_item_type_ != &kTypeInfo< Ty >,
            TEXT( "ERROR: Buffer {} is currently set to use type {}. But type {} was provided instead.",
                m_buffer_->GetName(), m_item_type_->name, kTypeInfo< Ty >.name ) )

        m_buffer_->UpdateSeg( std::addressof( std::forward< Ty >( _element ) ), _index * m_type_size_, m_type_size_ );
    }
} // sk::Graphics
