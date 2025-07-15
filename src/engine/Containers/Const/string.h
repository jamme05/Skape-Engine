/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Containers/Const/Array.h>
#include <Math/Math.h>

// TODO: Make a template specialization for array< char, Size >.

namespace sk
{
    typedef char char_t;
    template< size_t Size >
    struct string : array< char_t, Size >
    {
        string()
        : array< char_t, Size >{}
        {
        } // string

        // TODO: Use size parameter in string struct.
        constexpr string( const char_t* _ptr )
        : array< char_t, Size >( _ptr )
        {} // string

        // Makes this string lowercase.
        constexpr void to_lower();
        // Returns a copy which is lowercase.
        constexpr string as_lower() const
        {
            string result = *this;
            result.to_lower();
            return result;
        }

        // Makes this string uppercase.
        constexpr void to_upper();
        // Returns a copy which is uppercase.
        constexpr string as_upper() const
        {
            string result = *this;
            result.to_upper();
            return result;
        }
    };

    template< size_t Size >
    string( const char_t ( & )[ Size ] ) -> string< Size >;
    template< size_t Size >
    string( const string< Size >& ) -> string< Size >;

    template< class Ty >
    struct is_string
    {
        static constexpr bool kValue = false;
    };
    template< size_t S >
    struct is_string< string< S > >
    {
        static constexpr bool kValue = true;
    };

    namespace str
    {
        constexpr static int64_t get_single_size( const size_t _size, const int64_t _value )
        {
            if( _value == -1 )
                return static_cast< int64_t >( _size ) - 1;
            if( _value < 0 )
                return static_cast< int64_t >( _size ) + _value - 1;

            return _value;
        } // get_single_size

        constexpr static std::pair< size_t, size_t > get_range( const size_t _length, const int64_t _start, const int64_t _end )
        {
            const auto start = get_single_size( _length, _start );
            const auto end   = get_single_size( _length, _end );

            if( start > end || start < 0 || end < 0 )
                return { 0, 0 };

            return { start, static_cast< size_t >( end - start ) };
        } // get_index

        template< class Ty, class Ty2 = Ty >
        concept is_valid_string = std::is_same_v< Ty, char > && std::is_same_v< Ty, Ty2 >;

        template< int64_t Start, int64_t End, array Str >
        requires is_valid_string< arr::type< Str > >
        struct substr_internal
        {
            // TODO: Only use range
            constexpr static size_t offset = get_range( Str.size(), Start, End ).first;
            constexpr static size_t size   = get_range( Str.size(), Start, End ).second;
            constexpr substr_internal( void )
            {
                std::copy_n( Str.value + offset, size, value );
            } // substr_calc
            char value[ size + 1 ]{};
        };

        template< int64_t Start, array Str, int64_t End = -1 >
        requires is_valid_string< arr::type< Str > >
        struct substr
        {
            constexpr static substr_internal< Start, End, Str > compute{};
            constexpr static const char*   kValue = compute.value;
            constexpr static array kForward{ compute.value };
        };

        template< size_t StrSize, size_t SearchSize >
        constexpr static int64_t find_index_of( const char ( &_str )[ StrSize ], const char ( &_search )[ SearchSize ], const bool _backwards = false )
        {
            // TODO: Write better.
            static_assert( StrSize >= SearchSize, "Error: Target string has to be larger than the one to find." );
            // Remove null ending from check.
            const auto str_size    = StrSize - ( _backwards ? 2 : 1 );
            constexpr auto search_size = SearchSize - 2;

            int64_t found   = -1;
            size_t  counter = 0;

            if( _backwards )
            {
                size_t index = search_size;
                for( int64_t i = static_cast< int64_t >( str_size ); i >= 0; --i )
                {
                    if( _str[ i ] == _search[ index ] )
                    {
                        --index;
                        found = i;
                        if( counter++ == search_size )
                            return found; // Found
                    }
                    else
                    {
                        if( found != -1 )
                        {
                            i = found;
                            found   = -1;
                        }
                        counter = 0;
                        index   = search_size;
                    }
                }
            }
            else
            {
                for( size_t i = 0; i < str_size; ++i )
                {
                    if( _str[ i ] == _search[ counter ] )
                    {
                        if( found == -1 )
                            found = static_cast< int64_t >( i );
                        if( counter++ == search_size )
                            return found; // Found
                    }
                    else
                    {
                        if( found != -1 )
                        {
                            i = static_cast< size_t >( found );
                            found = -1;
                        }
                        counter = 0;
                    }
                }
            }

            return -1;
        } // find_index_of

        template< array Str, array Find, bool Backwards = false >
        requires is_valid_string< arr::type< Str >, arr::type< Find > >
        struct find
        {
            constexpr static int64_t kIndex = find_index_of( Str.value, Find.value, Backwards );
            constexpr static bool    kFound = kIndex != -1;
        };

        template< array Str, array Other >
        requires is_valid_string< arr::type< Str >, arr::type< Other > >
        struct equals
        {
            constexpr static bool kValue = Str.Length == Other.Length && find_index_of( Str.value, Other.value ) == 0;
        };

        template< array Str, array Find >
        requires is_valid_string< arr::type< Str >, arr::type< Find > >
        struct starts_with
        {
            constexpr static bool kValue = find_index_of( Str.value, Find.value ) == 0;
        };

        template< array Str, array Find >
        requires is_valid_string< arr::type< Str >, arr::type< Find > >
        struct ends_with
        {
            constexpr static bool kValue = find_index_of( Str.value, Find.value, true ) == Str.size() - Find.size();
        };

        // Follows the c++ standard. https://en.cppreference.com/w/cpp/string/byte/tolower
        // TODO: Fix some locale variant?
        constexpr int to_lower( int _char )
        {
            using pair_t = std::pair< int, int >;
            static constexpr auto upper = pair_t{ 'A', 'Z' };
            static constexpr auto lower = pair_t{ 'a', 'z' };

            // Check that char is within the upper range.
            if( _char >= upper.first && _char <= upper.second )
                _char = lower.first + _char - upper.first;

            return _char;
        }

        // Follows the c++ standard. https://en.cppreference.com/w/cpp/string/byte/toupper.html
        constexpr int to_upper( int _char )
        {
            using pair_t = std::pair< int, int >;
            static constexpr auto upper = pair_t{ 'A', 'Z' };
            static constexpr auto lower = pair_t{ 'a', 'z' };

            // Check that char is within the upper range.
            if( _char >= lower.first && _char <= lower.second )
                _char = upper.first + _char - lower.first;

            return _char;
        } // to_lower

        template< std::integral Ty >
        constexpr Ty to_lower( Ty _char )
        {
            return static_cast< Ty >( to_lower( static_cast< int >( _char ) ) );
        }

        template< std::integral Ty >
        constexpr Ty to_upper( Ty _char )
        {
            return static_cast< Ty >( to_upper( static_cast< int >( _char ) ) );
        }

        constexpr auto test  = to_lower( 'T' );
        constexpr auto test2 = to_upper( 't' );
    } // sk::str::

    // Define to_lower and to_upper here.
    template< size_t Size >
    constexpr void string< Size >::to_lower()
    {
        for( size_t i = 0; i < Size; ++i )
            ( *this )[ i ] = str::to_lower( ( *this )[ i ] );
    }

    template< size_t Size >
    constexpr void string< Size >::to_upper()
    {
        for( size_t i = 0; i < Size; ++i )
            ( *this )[ i ] = str::to_upper( ( *this )[ i ] );
    }

    namespace formatting
    {
        template< class Ty >
        struct string_max
        {};
        template<>
        struct string_max< float >
        {
            static constexpr auto kIntegral = 64;
            static constexpr auto kDecimal  = FLT_DIG;
        };
        template<>
        struct string_max< double >
        {
            static constexpr auto kIntegral = 350;
            static constexpr auto kDecimal  = DBL_DIG;
        } ;
    } // sk::str::formatting::

    template< class Ty, class Ta >
    requires ( std::is_integral_v< Ty > && std::is_unsigned_v< Ty > )
    static constexpr auto parse_uint_to_string( Ty _v, Ta* _out )
    {
        do {
            *--_out = static_cast< Ta >( _v % 10 + '0' );
            _v /= 10;
        } while( _v != 0 );
        return _out;
    } // parse_uint_to_string

    template< class Ty >
    requires std::is_integral_v< Ty >
    static constexpr auto to_string( Ty _v )
    {
        constexpr size_t size = std::is_signed_v< Ty > ? 22 : 21;
        string< size > buff;
        auto curr    = buff.end() - 1;

        if constexpr( std::is_signed_v< Ty > )
        {
            const auto uv = static_cast< std::make_unsigned_t< Ty > >( _v );
            if( _v < 0 )
            {
                curr = parse_uint_to_string( 0 - uv, curr );
                *--curr = static_cast< char_t >( '-' );
            }
            else
                curr = parse_uint_to_string( uv, curr );
        }
        else
        {
            curr = parse_uint_to_string( _v, curr );
        }

        buff.offset = curr - buff.begin();

        return buff;
    } // to_string

    template< class Ty, class Ta >
    requires std::is_floating_point_v< Ty >
    static constexpr auto parse_pos_floating_point_to_string( Ty _v, int_fast8_t _dec, Ta* _out )
    {
        _dec = Math::min< int_fast8_t >( _dec, formatting::string_max< Ty >::kDecimal );
        Ta num_buff[ formatting::string_max< Ty >::kIntegral ];
        Ta dec_buff[ 32 ];

        // Handle integer values
        Ta* n_curr = num_buff;
        Ty bv = _v;
        Ty exp = 0;
        do
        {
            const auto idv = static_cast< uint_fast32_t >( bv ) % 10;
            *n_curr++ = static_cast< char >( '0' + idv );
            bv /= 10;
            ++exp;
        } while( bv > 1 );

        // Handle decimals
        Ta* d_curr = dec_buff;
        _v -= static_cast< Ty >( static_cast< uint64_t >( _v ) );
        if( _dec > 0 )
        {
            *d_curr++ = static_cast< Ta >( '.' );
            do {
                _v *= 10;
                const auto idv = static_cast< uint_fast32_t >( _v );
                *d_curr++ = static_cast< Ta >( idv % 10 + '0' );
                //( _v -= static_cast< Ty >( idv ) ) *= 10;
                --_dec;
            } while( _v > 0 && _dec != 0 );
        }

        // Feed buffers into output
        while( d_curr != dec_buff )
            *--_out = *--d_curr;

        auto end = n_curr;
        n_curr   = num_buff;
        while( n_curr != end )
            *--_out = *n_curr++;

        return _out;
    }

    static constexpr auto to_string( const float _v, int_fast8_t _decimals = -1 )
    {
        string< 57 > buff;
        auto curr  = buff.end() - 1;

        _decimals = ( _decimals < 0 ) ? std::numeric_limits< int_fast8_t >::max() : _decimals;

        if( _v < 0 )
        {
            curr = parse_pos_floating_point_to_string( -_v, _decimals, curr );
            *--curr = static_cast< char_t >( '-' );
        }
        else
        {
            curr = parse_pos_floating_point_to_string( _v, _decimals, curr );
        }
        buff.offset = curr - buff.begin();

        return buff;
    } // to_string

    /**
     *
     * WARNING: Can be heavy to compute, so recommended to use floats instead.
     * @param _v Value to make a string of.
     * @param _decimals How many decimals to have in the value.
     * @return 
     */
    static constexpr auto to_string( const double _v, int_fast8_t _decimals = -1 )
    {
        string< 400 > buff;
        auto curr   = buff.end() - 1;

        _decimals = ( _decimals < 0 ) ? std::numeric_limits< int_fast8_t >::max() : _decimals;

        if( _v < 0 )
        {
            curr = parse_pos_floating_point_to_string( -_v, _decimals, curr );
            *--curr = static_cast< char_t >( '-' );
        }
        else
        {
            curr = parse_pos_floating_point_to_string( _v, _decimals, curr );
        }
        buff.offset = curr - buff.begin();

        return buff;
    } // to_string

    static constexpr auto to_string( const bool _v )
    {
        string< 6 > buff;
        if( _v )
            buff = "true";
        else
            buff = "false";
        return buff;
    } // to_string

    template< string Str >
    constexpr auto operator ""_lower()
    {
        return Str.as_lower();
    }

    template< string Str >
    constexpr auto operator ""_upper()
    {
        return Str.as_upper();
    }
} // sk::