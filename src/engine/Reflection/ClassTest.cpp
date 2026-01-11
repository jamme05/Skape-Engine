/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include <Misc/Smart_Ptrs.h>
#include <Reflection/RuntimeClass.h>

// TODO: Move test to it's own file.
SK_CLASS( Test )
{
    SK_CLASS_BODY( Test )
	
    uint64_t member_0 = 0;
    SK_VARIABLE( member_0 )
    uint32_t member_1 = 0;
    SK_VARIABLE( member_1 )
    uint32_t member_2 = 0;
    SK_VARIABLE( member_2 )

sk_public:
    static constexpr uint32_t member_3 = 3;
    SK_VARIABLE( member_3 )

    uint32_t member_5 = 0;
    SK_VARIABLE( member_5 )

    cTest() = default;

    cTest( uint64_t _mem0, uint32_t _mem1 )
    : member_0( _mem0 ), member_1( _mem1 )
    {
        // Look at std::apply and std::make_index_sequence and std::index_sequence to succeed with construction reflection.
    }
    SK_CONSTRUCTOR( cTest )
    SK_CONSTRUCTOR( cTest, uint64_t, uint64_t )

    void test_func( const uint32_t _val ){ member_1 = _val; }
	
    static bool test_func1(){ return false; }
    static int test_func1( bool _value ){ return _value; }
    uint32_t test_func2( void )  { return member_2; }
    uint16_t test_func2( void ) const { return member_2; }

    SK_FUNCTION( test_func )

    SK_FUNCTION( test_func1, STATIC )
    SK_FUNCTION( test_func1, STATIC, ARG( bool ) )

    SK_FUNCTION_P( test_func2 )

    SK_FUNCTION_O( test_func2 )
    SK_FUNCTION_C( test_func2 )
    SK_FUNCTION( test_func2, CONST )

    // TODO: Figure out how to handle virtual functions? Some custom virtual table or member functions?

    // TODO: Figure out a way to register virtual functions. Overridable tag?

    // Future example: SK_FUNCTION( test_func, Visibility=Private )
};

DECLARE_CLASS( Test )
