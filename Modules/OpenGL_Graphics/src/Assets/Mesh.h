/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#pragma once

#include <Assets/Asset.h>
#include <Containers/Map.h>

namespace sk::Graphics
{
    class cDynamic_Buffer;
} // sk::Graphics::

// TODO: Decide if I should move the Mesh from the opengl module to the main engine.
namespace sk::Assets
{
    SK_ASSET_CLASS( Mesh )
    {
        SK_CLASS_BODY( Mesh )

        using buffer_t     = Graphics::cDynamic_Buffer*;
        using buffer_map_t = unordered_map< str_hash, buffer_t >;

    sk_public:

        cMesh();
        ~cMesh() override;

        [[ nodiscard ]] auto GetIndexBuffer()       { return m_indices_; }
        [[ nodiscard ]] auto GetIndexBuffer() const { return m_indices_; }
        SK_FUNCTION_P( GetIndexBuffer )

        [[ nodiscard ]] auto& GetVertexBuffers()       { return m_vertex_buffers_; }
        [[ nodiscard ]] auto& GetVertexBuffers() const { return m_vertex_buffers_; }
        SK_FUNCTION_P( GetVertexBuffers )

    sk_private:
        buffer_t     m_indices_;
        buffer_map_t m_vertex_buffers_;
    };
} // sk::Assets

// DECLARE_CLASS( sk::Assets::Mesh )
namespace sk::Assets::Mesh
{
    namespace Internal
    {
        static constexpr auto _xxx_sk_func_counter_64133024 = class_type::func_counter_t::next();
        using func_t = std::pair< sk::str_hash, sk::Reflection::cMemberFunction >;
        using func_map_t = sk::const_map< func_t::first_type, const func_t::second_type*,
                                          _xxx_sk_func_counter_64133024 >;
        static constexpr auto kParentFunctions = class_type::parent_type::staticGetFunctions();
        static constexpr auto kParentFuncCount = kParentFunctions.size();
        static constexpr auto _xxx_sk_var_counter_64133024 = class_type::var_counter_t::next();
        using var_t = sk::Reflection::member_var_pair_t;
        using var_map_t = sk::const_map< var_t::first_type, const var_t::second_type*, _xxx_sk_var_counter_64133024 >;
        static constexpr auto kParentVariables = class_type::parent_type::staticGetVariables();
        static constexpr auto kParentVarCount = kParentVariables.size();
        using func_array_t = sk::array< func_t, func_map_t::kSize >;
        using var_array_t = sk::array< var_t, var_map_t::kSize >;
        static constexpr auto kRawFunctions = sk::Reflection::process_member_functions<
            class_type, Internal::_xxx_sk_func_counter_64133024 >();
        static constexpr auto kRawVariables = sk::Reflection::process_member_variables<
            class_type, Internal::_xxx_sk_var_counter_64133024 >();
    }

    static constexpr auto kFunctions = Internal::func_map_t{
        sk::Reflection::finalize_member_functions< Internal::kParentFuncCount >(
            Internal::kRawFunctions, Internal::kParentFunctions )
    };
    static constexpr auto kVariables = Internal::var_map_t{
        sk::Reflection::finalize_member_variables< Internal::kParentVarCount >(
            Internal::kRawVariables, Internal::kParentVariables )
    };
}

constexpr auto sk::Assets::Mesh::class_type::staticGetVariables()->sk::Reflection::member_var_map_ref_t
{
    return sk::Assets::Mesh::kVariables;
}

inline auto sk::Assets::Mesh::class_type::getVariables() const->sk::Reflection::member_var_map_ref_t
{
    return sk::Assets::Mesh::kVariables;
}

constexpr auto sk::Assets::Mesh::class_type::staticGetVariable(
    const sk::str_hash& _hash )->sk::Reflection::member_var_ptr_t
{
    const auto itr = sk::Assets::Mesh::kVariables.find( _hash );
    return itr == nullptr ? nullptr : itr->second;
}

inline auto sk::Assets::Mesh::class_type::getVariable(
    const sk::str_hash& _hash ) const->sk::Reflection::member_var_ptr_t { return staticGetVariable( _hash ); }

inline auto sk::Assets::Mesh::class_type::getBoundVariable(
    const sk::str_hash& _hash )->std::optional< sk::Reflection::cMemberVariableInstance< class_type > >
{
    if( const auto member = getVariable( _hash ); member == nullptr ) return {};
    else return sk::Reflection::cMemberVariableInstance{ *member, this };
}

constexpr auto sk::Assets::Mesh::class_type::staticGetFunctions()->sk::Reflection::member_func_map_ref_t
{
    return sk::Assets::Mesh::kFunctions;
}

inline auto sk::Assets::Mesh::class_type::getFunctions() const->sk::Reflection::member_func_map_ref_t
{
    return sk::Assets::Mesh::kFunctions;
}

constexpr auto sk::Assets::Mesh::class_type::staticGetFunction(
    const sk::str_hash& _hash )->sk::Reflection::member_func_ptr_t
{
    const auto itr = sk::Assets::Mesh::kFunctions.find( _hash );
    return itr == nullptr ? nullptr : itr->second;
}

inline auto sk::Assets::Mesh::class_type::getFunction(
    const sk::str_hash& _hash ) const->sk::Reflection::member_func_ptr_t { return staticGetFunction( _hash ); }

constexpr auto sk::Assets::Mesh::class_type::staticGetFunctionOverloads(
    const sk::str_hash& _hash )->sk::Reflection::member_func_range_t
{
    return sk::Assets::Mesh::kFunctions.range( _hash );
}

inline auto sk::Assets::Mesh::class_type::getFunctionOverloads(
    const sk::str_hash& _hash ) const->sk::Reflection::member_func_range_t
{
    return staticGetFunctionOverloads( _hash );
}

constexpr auto sk::Assets::Mesh::class_type::staticGetFunction( const sk::str_hash& _hash,
                                                                const sk::type_hash& _args )->
    sk::Reflection::member_func_ptr_t
{
    for( auto [ fst, lst ] = sk::Assets::Mesh::kFunctions.range( _hash ); fst != lst; ++fst )
    {
        if( fst->second->hasArgs( _args ) ) return fst->second;
    }
    return nullptr;
}

inline auto sk::Assets::Mesh::class_type::getFunction( const sk::str_hash& _hash,
                                                       const sk::type_hash& _args ) const->
    sk::Reflection::member_func_ptr_t { return staticGetFunction( _hash, _args ); }

template< class Ty, class... Args >
constexpr auto
sk::Assets::Mesh::class_type::staticGetFunction( const sk::str_hash& _hash )->sk::Reflection::member_func_ptr_t
{
    return staticGetFunction( _hash, sk::args_hash< Ty, Args... >::kHash );
}

template< class Ty, class... Args >
auto sk::Assets::Mesh::class_type::getFunction( const sk::str_hash& _hash )->sk::Reflection::member_func_ptr_t
{
    return getFunction( _hash, sk::args_hash< Ty, Args... >::kHash );
}

inline auto sk::Assets::Mesh::class_type::getBoundFunction(
    const sk::str_hash& _hash )->std::optional< sk::Reflection::cMemberFunctionInstance< class_type > >
{
    if( const auto member = getFunction( _hash ); member == nullptr ) return {};
    else return sk::Reflection::cMemberFunctionInstance{ *member, this };
}

template<>
struct sk::get_type_info< sk::Assets::Mesh::class_type > : sk::template_type_info
{
    constexpr static auto& kClass = sk::Assets::Mesh::class_type::kClass;
    constexpr static sClass_Type_Info kInfo = {
        {
            .type = sType_Info::eType::kClass, .hash = kClass.getType(), .size = sizeof( sk::Assets::Mesh::class_type ),
            .name = kClass.getRawName(), .raw_name = kClass.getRawName()
        },
        &sk::Assets::Mesh::class_type::getStaticClass()
    };
    constexpr static bool kValid = true;
};

inline auto type_registry_64133025 = sk::Reflection::cType_Manager::RegisterType< sk::Assets::Mesh::class_type >();
