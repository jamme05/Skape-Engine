/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once


#include <sk/Graphics/Buffer/Unsafe_Buffer.h>
#include <sk/Math/Math.h>
#include <sk/Memory/Memory.h>

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

        // Has to be overwritten using a moved Buffer if you're using this.
        cBuffer();
        explicit cBuffer( const std::string& _name, bool _is_static = false );
        cBuffer( const std::string& _name, size_t _size, bool _is_static = false );
        cBuffer( const std::string& _name, std::initializer_list< Ty > _values, bool _is_static = false );
        template< class Iter >
        requires std::is_same_v< Ty, std::iter_value_t< Iter > >
        cBuffer( const std::string& _name, Iter _begin, Iter _end, bool _is_static = false );

        cBuffer( const cBuffer& _other );
        cBuffer( cBuffer&& _other ) noexcept;
        ~cBuffer();
        
        static constexpr auto getByteSize( const size_t _count ){ return Memory::get_aligned( _count * kTypeSize, Memory::kShaderAlign ); }
        
        void MarkDirty();
        bool IsDirty() const;
        void Upload( bool _force = false );

        auto GetBuffer() const -> const iUnsafe_Buffer&;
        
        auto Data() -> Ty*;
        auto Data() const -> const Ty*;
        
        // TODO: Add capacity control once it's been added to the unsafe buffer
        auto Size    () const -> size_t;
        auto Capacity() const -> size_t;
        
        void Resize( size_t _new_size );
        void Reserve( size_t _new_capacity );
        
        // Returns the index of the inserted value
        template< class... Args >
        requires std::constructible_from< Ty, Args... >
        size_t EmplaceBack( Args&&... _value );
        void   PopBack( Ty* _removed_value = nullptr );
        
        auto Front() -> Ty&;
        auto Front() const -> const Ty&;
        
        auto Back() -> Ty&;
        auto Back() const -> const Ty&;

        auto begin() -> Ty*;
        auto begin() const -> const Ty*;

        auto end() -> Ty*;
        auto end() const -> const Ty*;
        
        auto operator->() -> Ty*;
        auto operator->() const -> const Ty*;
        
        auto operator*() -> Ty&;
        auto operator*() const -> const Ty&;
        
        auto operator=( const cBuffer& _other ) -> cBuffer&;
        cBuffer& operator=( cBuffer&& _other ) noexcept;

        auto operator[]( size_t _index ) -> Ty&;
        auto operator[]( size_t _index ) const -> const Ty&;

    private:
        size_t         m_size_;
        cUnsafe_Buffer m_buffer_;
    };

    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >::cBuffer()
    : m_size_( 0 )
    {}

    // TODO: Is normalized support.
    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >::cBuffer( const std::string& _name, const bool _is_static )
    : m_size_( 0 )
    , m_buffer_( _name, 0, kTypeSize, Type, false, _is_static )
    {} // cBuffer

    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >::cBuffer( const std::string& _name, const size_t _size, const bool _is_static )
    : m_size_( _size )
    , m_buffer_( _name, getByteSize( _size ), kTypeSize, Type, false, _is_static )
    {} // cBuffer

    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >::cBuffer( const std::string& _name, std::initializer_list< Ty > _values, const bool _is_static )
    : m_size_( _values.size() )
    , m_buffer_( _name, getByteSize( _values.size() ), Type, _is_static )
    {
        m_buffer_.Update( _values.begin(), _values.size() );
    } // cBuffer

    template< class Ty, Buffer::eType Type >
    template< class Iter >
    requires std::is_same_v< Ty, std::iter_value_t< Iter > >
    cBuffer< Ty, Type >::cBuffer( const std::string& _name, Iter _begin, Iter _end, bool _is_static )
    {
        const auto dist = static_cast< size_t >( std::distance( _begin, _end ) );
        m_buffer_ = { _name, 0, sizeof( Ty ), Type, false, _is_static };
        m_size_   = dist;

        std::vector< Ty > copy( _begin, _end );
        m_buffer_.Update( copy.data(), dist * kTypeSize );
        m_buffer_.Upload( true );
    } // cBuffer

    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >::cBuffer( const cBuffer& _other )
    : m_size_( _other.m_size_ )
    , m_buffer_( _other.m_buffer_ )
    {} // cBuffer ( Copy )

    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >::cBuffer( cBuffer&& _other ) noexcept
    : m_size_( _other.m_size_ )
    , m_buffer_( std::move( _other.m_buffer_ ) )
    {} // cBuffer ( Move )

    template< class Ty, Buffer::eType Type >
    cBuffer< Ty, Type >::~cBuffer()
    {
        m_buffer_.Destroy();
    } // ~cBuffer

    template< class Ty, Buffer::eType Type >
    void cBuffer< Ty, Type >::MarkDirty()
    {
        m_buffer_.SetChanged();
    }

    template< class Ty, Buffer::eType Type >
    bool cBuffer< Ty, Type >::IsDirty() const
    {
        return m_buffer_.GetIsChanged();
    }

    template< class Ty, Buffer::eType Type >
    void cBuffer< Ty, Type >::Upload( const bool _force )
    {
        m_buffer_.Upload( _force );
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::GetBuffer() const -> const iUnsafe_Buffer&
    {
        return m_buffer_;
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::Data() -> Ty*
    {
        return static_cast< Ty* >( m_buffer_.Data() );
    } // data

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::Data() const -> const Ty*
    {
        return static_cast< const Ty* >( m_buffer_.Data() );
    } // data
    
    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::Size() const -> size_t
    {
        return m_size_;
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::Capacity() const -> size_t
    {
        return 0;
    }

    template< class Ty, Buffer::eType Type >
    void cBuffer< Ty, Type >::Resize( const size_t _new_size )
    {
        m_size_ = _new_size;
        m_buffer_.Resize( getByteSize( _new_size ) );
    }

    template< class Ty, Buffer::eType Type >
    void cBuffer< Ty, Type >::Reserve( const size_t _new_capacity )
    {
        // TODO: Add the reserve function
    }

    template< class Ty, Buffer::eType Type >
    template< class... Args >
    requires std::constructible_from< Ty, Args... >
    size_t cBuffer< Ty, Type >::EmplaceBack( Args&&... _value )
    {
        auto index = Size();
        Resize( index + 1 );
        
        std::construct_at( Data() + index, std::forward< Args >( _value )... );
        ::new( Data() + index ) Ty{ std::forward< Args >( _value )... };
        return index;
    }

    template< class Ty, Buffer::eType Type >
    void cBuffer< Ty, Type >::PopBack( Ty* _removed_value )
    {
        auto index = Size() - 1;
        
        if( _removed_value != nullptr )
            *_removed_value = Data()[ index ];
        
        Resize( index );
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::Front() -> Ty&
    {
        return *Data();
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::Front() const -> const Ty&
    {
        return *Data();
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::Back() -> Ty&
    {
        return Data()[ Size() - 1 ];
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::Back() const -> const Ty&
    {
        return Data()[ Size() - 1 ];
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::begin() -> Ty*
    {
        return Data();
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::begin() const -> const Ty*
    {
        return Data();
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::end() -> Ty*
    {
        return Data() + Size();
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::end() const -> const Ty*
    {
        return Data() + Size();
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::operator->() -> Ty*
    {
        return Data();
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::operator->() const -> const Ty*
    {
        return Data();
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::operator*() -> Ty&
    {
         return *Data();
    }

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::operator*() const -> const Ty&
    {
        return *Data();
    }

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
        return Data()[ _index ];
    } // operator[]

    template< class Ty, Buffer::eType Type >
    auto cBuffer< Ty, Type >::operator[]( size_t _index ) const -> const Ty&
    {
        return Data()[ _index ];
    } // operator[] const
    
    template< class Ty >
    using cConstant_Buffer = cBuffer< Ty, Buffer::eType::kConstant >;
    template< class Ty >
    using cStructured_Buffer = cBuffer< Ty, Buffer::eType::kStructured >;
    template< class Ty >
    using cVertex_Buffer = cBuffer< Ty, Buffer::eType::kVertex >;

} // sk::Graphics::
