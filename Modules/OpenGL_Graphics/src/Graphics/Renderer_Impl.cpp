/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Renderer_Impl.h"

#include <glbinding/glbinding.h>
#include <glbinding/gl/functions.h>

#include <Assets/Management/Asset_Manager.h>
#include <Platform/Platform_Base.h>

#include "Assets/Shader.h"

using namespace sk;
using namespace sk::Graphics;

namespace 
{
    void loadGLSLMeta( const std::filesystem::path& _path, Assets::cAsset_List& _metas )
    {
        SK_WARN_IF_RET( sk::Severity::kEngine, !std::filesystem::exists( _path ),
            "Warning: Asset doesn't exist" )
        
        _metas.AddAsset( sk::make_shared< cAsset_Meta >( _path.filename().replace_extension().c_str(), sk::kTypeInfo< Assets::cShader > ) );
    }

    void loadGLSL( const std::filesystem::path& _path, Assets::cAsset_List& _metas, const Assets::eAssetTask _task )
    {
        if( _task == Assets::eAssetTask::kLoadMeta )
            return loadGLSLMeta( _path, _metas );
        
        // It'll always only have one meta.
        auto& meta = *_metas.begin();
        
        auto file_size = std::filesystem::file_size( _path );
        auto buffer = new char[ file_size ];
        
        auto file = std::ifstream( _path, std::ios::binary );
        file.read( buffer, static_cast< std::streamsize >( file_size ) );
        
        Shader::eType type;
        switch( meta->GetExtension().hash() )
        {
        case Hashing::fnv1a_64( "frag" ):    type = Shader::eType::kFragment; break;
        case Hashing::fnv1a_64( "vert" ):    type = Shader::eType::kVertex;   break;
        case Hashing::fnv1a_64( "compute" ): type = Shader::eType::kCompute;  break;
        default: SK_BREAK
        }

        meta->setAsset( SK_SINGLE( Assets::cShader, type, buffer, file_size ) );
        delete[] buffer;
    }
} // ::

cGLRenderer::cGLRenderer()
{
    glbinding::initialize( &Platform::get_proc_address );

    cAsset_Manager::get().AddFileLoaderForExtensions(
        { "frag", "vert", "compute" }, &loadGLSL );
} // cRenderer

cGLRenderer::~cGLRenderer()
{
    cAsset_Manager::get().RemoveFileLoaders( { "frag", "vert", "compute" } );
}

void sk::Graphics::InitRenderer()
{
    cGLRenderer::init();
}
