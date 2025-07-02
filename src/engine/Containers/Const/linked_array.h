/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Containers/Const/Array.h>

namespace sk
{
    template< typename Ty >
    class cLinked_Iterator
    {
    public:
        typedef size_t difference_type;
        typedef Ty     value_type;
        typedef Ty*    pointer;
        typedef Ty&    reference;
        typedef std::bidirectional_iterator_tag iterator_category;

        constexpr cLinked_Iterator( void )
        : m_next( nullptr )
        , m_element( nullptr )
        { }
        constexpr cLinked_Iterator( Ty& _element, const cLinked_Iterator* _next )
        : m_next( _next )
        , m_element( &_element )
        { }
        constexpr cLinked_Iterator( const cLinked_Iterator& _other )
        : m_next( _other.m_next )
        , m_element( _other.m_element )
        { }
        constexpr cLinked_Iterator( cLinked_Iterator&& _other ) noexcept
        : m_next( _other.m_next )
        , m_element( _other.m_element )
        { }

        constexpr ~cLinked_Iterator( void ) = default;

        constexpr auto get_Next( void ) const { return *m_next; }

        constexpr auto& operator++( void )
        {
            if( m_next )
                *this = *m_next;
            else
                m_element = nullptr;
            return *this;
        } // ++self

        constexpr auto operator++( int )
        {
            auto tmp = *this;
            if( m_next )
                *this = *m_next;
            else
                m_element = nullptr;
            return tmp;
        } // self++

        constexpr auto& operator*( void ) noexcept
        {
            return *m_element;
        } // *self

        constexpr const Ty& operator*( void ) const noexcept
        {
            return *m_element;
        } // *self

        constexpr auto operator->( void ) const
        {
            return m_element;
        } // self->

        constexpr auto& operator=( const cLinked_Iterator& _right )
        {
            if( this == &_right )
                return *this;
            m_next    = _right.m_next;
            m_element = _right.m_element;
            return *this;
        } // self = (copy)

        constexpr auto& operator=( cLinked_Iterator&& _right ) noexcept
        {
            m_next    = _right.m_next;
            m_element = _right.m_element;
            return *this;
        } // self = (move)

        constexpr auto operator==( const cLinked_Iterator& _right ) const
        {
            return m_element == _right.m_element && m_next == _right.m_next;
        } // self == right

        constexpr auto operator!=( const cLinked_Iterator& _right ) const
        {
            return !( *this == _right );
        } // self != right

    private:
        const cLinked_Iterator* m_next;
        Ty*                     m_element;
    };

    template< typename Ty >
    class cLinked_Array
    {
    public:
        constexpr cLinked_Array( void );
        constexpr cLinked_Array( Ty _element );
        constexpr cLinked_Array( Ty _element, const cLinked_Array& _next );
        constexpr cLinked_Array( const cLinked_Array& _other );
        constexpr cLinked_Array( cLinked_Array&& _other ) noexcept;

        constexpr auto begin( void )       { return m_iterator; }
        constexpr auto begin( void ) const { return m_iterator; }
        constexpr auto end  ( void )       { return cLinked_Iterator< Ty >{}; }
        constexpr auto end  ( void ) const { return cLinked_Iterator< Ty >{}; }

        constexpr auto size ( void ) const { return std::distance( this->begin(), this->end() ); }

    private:
        Ty                     m_element;
        cLinked_Iterator< Ty > m_iterator;
    };

    template< typename Ty >
    constexpr cLinked_Array< Ty >::cLinked_Array( void )
    : m_element{}
    , m_iterator{}
    {
    } // cLinked_Array

    template< typename Ty >
    constexpr cLinked_Array< Ty >::cLinked_Array( Ty _element )
    : m_element( std::move( _element ) )
    , m_iterator( m_element, nullptr )
    {
    } // sLinked_Array

    template< typename Ty >
    constexpr cLinked_Array< Ty >::cLinked_Array( Ty _element, const cLinked_Array& _next )
    : m_element( std::move( _element ) )
    , m_iterator( m_element, &_next.m_iterator )
    {
    } // sLinked_Array

    template< typename Ty >
    constexpr cLinked_Array< Ty >::cLinked_Array( const cLinked_Array& _other )
    : m_element( _other.m_element )
    , m_iterator( m_element, _other.m_iterator )
    {
    } // cLinked_Array

    template< typename Ty >
    constexpr cLinked_Array< Ty >::cLinked_Array( cLinked_Array&& _other ) noexcept
    : m_element( std::move( _other.m_element ) )
    , m_iterator( std::move( _other.m_iterator ) )
    {
    } // sLinked_Array

    template< typename Ty >
    cLinked_Array( Ty ) -> cLinked_Array< Ty >;
    template< typename Ty >
    cLinked_Array( Ty, cLinked_Array< Ty > ) -> cLinked_Array< Ty >;
} // sk::