/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Graphics/Buffer/Unsafe_Buffer.h>

#include <Reflection/Types.h>
#include <Math/Types.h>

// TYPE UNSAFE BUFFER

namespace sk::Graphics
{
    class cDynamic_Buffer
    {
    public:
        cDynamic_Buffer( const std::string& _name, const Buffer::eType _type )
        : m_size_( 0 )
        , m_type_size_( 0 )
        , m_type_( nullptr )
        , m_buffer_( _name, 0, _type, false ) // Dynamic buffer will be non-static by default.
        {} // cDynamic_Buffer
        cDynamic_Buffer( const std::string& _name, const Buffer::eType& _type, size_t& _size );

        cDynamic_Buffer( const cDynamic_Buffer& _other );
        cDynamic_Buffer( cDynamic_Buffer&& _other ) noexcept;

        template< reflected Ty >
        void AlignAs();

        template< class Ty >
        void AlignAs();

        void AlignAs( size_t _align );

        ~cDynamic_Buffer();
    private:
        // The outwards size of this buffer. Get the size from the unsafe buffer for the capacity.
        size_t         m_size_;
        // Uses both.
        size_t         m_type_size_;
        type_info_t    m_type_;

        cUnsafe_Buffer m_buffer_;
    };

    template< reflected Ty >
    void cDynamic_Buffer::SetAlign()
    {
        static constexpr bool is_safe_align = Memory::get_size< Ty, Memory::eAlignment::kShaderAlign >();
        SK_WARN_IFN( sk::Severity::kConstGraphics | 100,
            is_safe_align, TEXT( "WARNING: The type isn't aligned by 16 bits" ) )

        m_type_ = get_type_info< Ty >::kInfo;
    }

#define BUFFER_VARIANT_CONSTRUCTOR_0( VariantEnum, VariantClass ) \
template< class Ty > class VariantClass : public cBuffer< Ty >{ public: \
explicit VariantClass ( const std::string& _name, const bool _is_static = false ) \
: cBuffer< Ty >( _name, VariantEnum, _is_static ){} };
    
#define BUFFER_VARIANT_0( VariantEnum, VariantClass ) \
BUFFER_VARIANT_CONSTRUCTOR_0( VariantEnum, VariantClass )

#define BUFFER_VARIANT( Variant ) \
BUFFER_VARIANT_0( CONCAT( Buffer::eType::k, Variant ), M_CLASS( CONCAT( Variant, _Buffer ) ) )

    static auto cConstant_Dynamic_Buffer( const std::string& _name )
    {
        return cDynamic_Buffer( _name, Buffer::eType::kConstant );
    };

    // BUFFER_VARIANT( Constant )
    // BUFFER_VARIANT( Structed )
    // BUFFER_VARIANT( Vertex )


    // template< class Ty >
    // using cConstant_Dynamic_Buffer = cDynamic_Buffer< Buffer::eType::kConstant >;
    // template< class Ty >
    // using cStructed_Dynamic_Buffer = cDynamic_Buffer< Buffer::eType::kStructed >;
    // template< class Ty >
    // using cVertex_Dynamic_Buffer = cDynamic_Buffer< Buffer::eType::kVertex >;

} // sk::Graphics

#undef BUFFER_VARIANT_CONSTRUCTOR_0
#undef BUFFER_VARIANT_0
#undef BUFFER_VARIANT