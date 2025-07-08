/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

// TODO: Use forward declare instead?
#include <Containers/Const/Array.h>
#include <Misc/Hashing.h>

#include <utility>

namespace sk
{
    template< class Ty >
    struct is_valid_hash
    {
    private:
        template< class Ty2 >
        static uint8_t  test( decltype( &Ty2::getHash ) );
        template< class >
        static uint16_t test( ... );
    public:
        static constexpr bool is_valid = sizeof( decltype( test< Ty >( 0 ) ) ) == 1;
    };
    template< class Ty >
    constexpr bool is_valid_hash_v = is_valid_hash< Ty >::is_valid;

    template< class KeyTy, class ValueTy >
    class map_base
    {
    public:
        typedef KeyTy key_type;
        typedef ValueTy value_type;
        typedef std::pair< KeyTy, ValueTy > value_pair_type;
        
        map_base( void ) = default;
        virtual ~map_base( void ) = default;
        // TODO: Watch and make sure it doesn't cause any issues.
        virtual const ValueTy* find( const KeyTy& _key ) const = 0;

        virtual const value_pair_type* begin( void ) const = 0;
        virtual const value_pair_type* begin( void )       = 0;
        virtual const value_pair_type* end  ( void ) const = 0;
        virtual const value_pair_type* end  ( void )       = 0;
    };

    namespace map_helper
    {
        static constexpr auto _get_middle( size_t _begin, size_t _end )
        {
            size_t middle_index = ( _end - _begin ) / 2 + ( _begin );
            // Ensure just in case? Should be safe.
            // middle_index = Math::min( middle_index, m_array.size() - 1 );
            return middle_index;
        } // _get_middle
    } // map_helper::

    struct pair_less
    {
        template< class Ty1, class Ty2 >
        constexpr bool operator()( std::pair< Ty1, Ty2 >&& _left, Ty2&& _right ) const
        {
            return static_cast< Ty1&& >( _left.first ) < static_cast< Ty2&& >( _right.first );
        }
    };

    // TODO: Decide weather or not the const map functioning as a multimap is undefined behaviour. Will be treated as fine for now due to convenience.
    template< class KeyTy, class ValueTy, size_t Size, class Pred = std::less<> >
    class const_map
    {
    public:
        typedef KeyTy key_type;
        typedef ValueTy value_type;
        typedef std::pair< KeyTy, ValueTy > value_pair_type;
        static constexpr auto kSize = Size;

        constexpr const_map() = default;

        constexpr const_map( const const_map& ) = default;
        constexpr const_map( const_map&& ) = default;

        consteval const_map( const array< std::pair< KeyTy, ValueTy >, Size >& _array );
        consteval const_map( std::initializer_list< std::pair< KeyTy, ValueTy > > _list );

        template< class Itr >
        requires std::is_same_v< typename Itr::value_type, value_pair_type >
        consteval const_map( Itr _left, Itr _right );

        constexpr static bool _compare( const value_pair_type& _left, const value_pair_type& _right )
        {
            static constexpr auto pred = Pred();
            return pred( _left.first, _right.first );
        } // _compare_values

        constexpr const_map& operator=( const const_map& _map ) = default;
        constexpr const_map& operator=( const_map&& _map ) = default;

        // TODO: Make iterator so find can become constexpr?
        [[ nodiscard ]]
        constexpr const value_pair_type* find( const KeyTy& _key ) const
        {
            // TODO: See if there's a better way? Default construction may fail.
            return std::lower_bound( begin(), end(), std::pair{ _key, ValueTy{} }, _compare );
        }

        [[ nodiscard ]]
        constexpr auto range( const KeyTy& _key ) const
        {
            return std::equal_range( begin(), end(), std::pair{ _key, ValueTy{} }, _compare );
        }
        constexpr const value_pair_type& get( const size_t _index ) const { return m_array[ _index ]; }

        constexpr const value_pair_type* begin( void ) const { return m_array.begin(); }
        constexpr const value_pair_type* begin( void )       { return m_array.begin(); }
        constexpr const value_pair_type* end  ( void ) const { return m_array.end(); }
        constexpr const value_pair_type* end  ( void )       { return m_array.end(); }

        constexpr size_t size( void ) const { return m_array.size(); }

        array< value_pair_type, Size > m_array;
    };

    template< class KeyTy, class ValueTy, size_t Size, class Pred >
    consteval const_map< KeyTy, ValueTy, Size, Pred >::const_map( const array< std::pair< KeyTy, ValueTy >, Size >& _array )
    : m_array( _array )
    {
        std::sort( m_array.begin(), m_array.end(), _compare );
    } // const_hashmap

    template< class KeyTy, class ValueTy, size_t Size, class Pred >
    consteval const_map< KeyTy, ValueTy, Size, Pred >::const_map( std::initializer_list< std::pair< KeyTy, ValueTy > > _list )
    : const_map( _list.begin(), _list.end() )
    {} // const_map

    template< class KeyTy, class ValueTy, size_t Size, class Pred >
    template< class Itr > requires std::is_same_v< typename Itr::value_type, std::pair< KeyTy, ValueTy > >
    consteval const_map<KeyTy, ValueTy, Size, Pred>::const_map( Itr _left, Itr _right )
    : m_array{}
    {
        if( m_array.size() == 0 )
            return;
            
        std::copy( std::move( _left ), std::move( _right ), m_array.get() );
        std::sort( m_array.begin(), m_array.end(), _compare );
    } // const_map

    template< class KeyTy, class ValueTy, class Pred = std::less<> >
    class map_ref
    {
    public:
        constexpr map_ref( void )
        : m_size( 0 )
        , m_data( nullptr )
        {}

        /**
         *
         * @attention Requires _map to be static.
         * 
         * @tparam Size The size of the map.
         * @param _map The map to reference.
         */
        template< size_t Size >
        constexpr map_ref( const const_map< KeyTy, ValueTy, Size, Pred >& _map );

        typedef std::pair< KeyTy, ValueTy > pair_t;
        typedef const pair_t* pair_ptr_t;
        typedef const pair_t& pair_ref_t;

        [[ nodiscard ]]
        constexpr pair_ptr_t find( const KeyTy& _key ) const
        {
            return std::lower_bound( begin(), end(), std::pair{ _key, ValueTy{} }, Pred{} );
        }
        [[ nodiscard ]]
        constexpr int64_t find_index( const KeyTy& _key ) const;
        [[ nodiscard ]]
        constexpr std::pair< pair_ptr_t, pair_ptr_t > range( const KeyTy& _key ) const
        {
            return std::equal_range( begin(), end(), std::pair{ _key, ValueTy{} }, Pred{} );
        }

        [[ nodiscard ]]
        constexpr pair_ptr_t begin( void ) const { return get(); }
        [[ nodiscard ]]
        constexpr pair_ptr_t end  ( void ) const { return get() + size(); }

        [[ nodiscard ]]
        constexpr pair_ptr_t get  ( void ) const { return m_data; }

        [[ nodiscard ]]
        constexpr pair_ref_t operator[]( const size_t _index ) const { return get()[ _index ]; }

        [[ nodiscard ]]
        constexpr size_t size( void ) const { return m_size; }

    private:
        size_t     m_size;
        pair_ptr_t m_data;
    };

    template< class KeyTy, class ValueTy, class Pred >
    constexpr int64_t map_ref< KeyTy, ValueTy, Pred >::find_index( const KeyTy& _key ) const
    {
        constexpr auto pred = Pred();
        size_t begin_edge = 0;
        size_t end_edge   = size();
#define GET_MID c = map_helper::_get_middle( begin_edge, end_edge )
        for( size_t GET_MID; c != end_edge; GET_MID )
#undef GET_MID
        {
            // TODO: Figure out why this doesn't work.
            auto& current = m_data[ c ];
            if( current.first == _key )
                return static_cast< int64_t >( c );
            if( pred( current.first, _key ) )
                begin_edge = c + 1;
            else
                end_edge = c;
        }

        return -1;
    } // find_index

    template< class KeyTy, class ValueTy, class Pred >
    template< size_t Size >
    constexpr map_ref< KeyTy, ValueTy, Pred >::map_ref( const const_map< KeyTy, ValueTy, Size, Pred >& _map )
    : m_size( Size )
    , m_data( _map.begin() )
    {
    } // map_ref
} // sk::
