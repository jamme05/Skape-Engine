/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Graphics/Buffer/Unsafe_Buffer.h>
#include <Memory/Memory.h>

// TYPE SAFE BUFFER

namespace sk::Graphics
{
    template< class Ty, Buffer::eType Type >
    class cBuffer
    {
    public:
        static constexpr auto kType     = Type;
        static constexpr auto kTypeSize = sizeof( Ty );
        using value_type = Ty;

        explicit cBuffer( const std::string& _name, bool _is_static = false );
        cBuffer( const std::string& _name, size_t _size, bool _is_static = false );
        cBuffer( const std::string& _name, std::initializer_list< Ty > _values, bool _is_static = false );
        template< class Iter >
        requires std::is_same_v< Ty, std::iter_value_t< Iter > >
        cBuffer( const std::string& _name, Iter _begin, Iter _end, bool _is_static = false );

        cBuffer( const cBuffer& _other );
        cBuffer( cBuffer&& _other ) noexcept;
        ~cBuffer();

        // Skip align or use it?
        static constexpr auto getSize( const size_t _count ){ return _count * kTypeSize; }

        auto data() -> Ty*;
        auto data() const -> const Ty*;

        auto operator=( const cBuffer& _other ) -> cBuffer&;
        cBuffer& operator=( cBuffer&& _other ) noexcept;

        auto operator[]( size_t _index ) -> Ty&;
        auto operator[]( size_t _index ) const -> const Ty&;
    private:
        struct sData
        {
            size_t size;
            size_t capacity;
        } m_data_;
        cUnsafe_Buffer m_buffer_;
    };

    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >::cBuffer( const std::string& _name, const bool _is_static )
    : m_buffer_( _name, 0, Type, _is_static )
    {} // cBuffer

    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >::cBuffer( const std::string& _name, const size_t _size, const bool _is_static )
    : m_buffer_( _name, _size, Type, _is_static )
    {} // cBuffer

    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >::cBuffer( const std::string& _name, std::initializer_list< Ty > _values, const bool _is_static )
    : m_buffer_( _name, getSize( _values.size() ), Type, _is_static )
    {
        auto size = getSize( _values.size() );
        m_buffer_.Update( _values.begin(), size );
    } // cBuffer

    template< class Ty, Buffer::eType Type >
    template< class Iter >
    requires std::is_same_v< Ty, std::iter_value_t< Iter > >
    cBuffer< Ty, Type >::cBuffer( const std::string& _name, Iter _begin, Iter _end, bool _is_static )
    {
        const auto dist = std::distance( _begin, _end );
        m_buffer_ = { _name, dist, Type, _is_static };

        std::vector< Ty > copy( _begin, _end );
        m_buffer_.Update( copy.data(), dist * kTypeSize );
    } // cBuffer

    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >::cBuffer( const cBuffer& _other )
    : m_buffer_( _other.m_buffer_ )
    {} // cBuffer ( Copy )

    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >::cBuffer( cBuffer&& _other ) noexcept
    : m_buffer_( std::move( _other.m_buffer_ ) )
    {} // cBuffer ( Move )

    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >::~cBuffer()
    {
        m_buffer_.Destroy();
    } // ~cBuffer

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::data()->Ty*
    {
        return static_cast< Ty* >( m_buffer_.Get() );
    } // data

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::data() const -> const Ty*
    {
        return static_cast< const Ty* >( m_buffer_.Get() );
    } // data

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::operator=( const cBuffer& _other ) -> cBuffer&
    {
        if( this != &_other )
            return *this;

        m_buffer_ = _other.m_buffer_;

        return *this;
    } // operator= ( Copy )

    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >& cBuffer< Ty, Type >::operator=( cBuffer&& _other ) noexcept
    {
        m_buffer_ = std::move( _other.m_buffer_ );

        return *this;
    } // operator= ( Move )
    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::operator[]( size_t _index ) -> Ty&
    {
        
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::operator[]( size_t _index ) const -> const Ty&
    {
    }

    template< class Ty >
    using cConstant_Buffer = cBuffer< Ty, Buffer::eType::kConstant >;
    template< class Ty >
    using cStructed_Buffer = cBuffer< Ty, Buffer::eType::kStructed >;
    template< class Ty >
    using cVertex_Buffer = cBuffer< Ty, Buffer::eType::kVertex >;

} // sk::Graphics::
