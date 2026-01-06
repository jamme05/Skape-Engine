/*
 *
 * COPYRIGHT William Ask S. Ness 2025
 *
 */

#include "Renderer_Impl.h"

#include <glbinding/Binding.h>
#include <glbinding/glbinding.h>
#include <glbinding/gl/functions.h>

#include <Assets/Management/Asset_Manager.h>
#include <Platform/Platform_Base.h>

#include "Assets/Shader.h"
#include "Assets/Management/Asset_Job_Manager.h"

using namespace sk;
using namespace sk::Graphics;

namespace 
{
    void loadGLSLMeta( const std::filesystem::path& _path, Assets::cAsset_List& _metas )
    {
        SK_WARN_IF_RET( sk::Severity::kEngine, !std::filesystem::exists( _path ),
            "Warning: Asset doesn't exist" )
        
        _metas.AddAsset( sk::make_shared< cAsset_Meta >( _path.filename().replace_extension().string(), sk::kTypeInfo< Assets::cShader > ) );
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
        
        constexpr auto frag_v = Hashing::fnv1a_64( "frag" );
        constexpr auto vert_v = Hashing::fnv1a_64( "vert" );
        constexpr auto comp_v = Hashing::fnv1a_64( "comp" );
        
        Shader::eType type;
        switch( meta->GetExtension().hash() )
        {
        case frag_v: type = Shader::eType::kFragment; break;
        case vert_v: type = Shader::eType::kVertex;   break;
        case comp_v: type = Shader::eType::kCompute;  break;
        default: SK_BREAK; break;
        }

        meta->setAsset( SK_SINGLE( Assets::cShader, type, buffer, file_size ) );
        
        delete[] buffer;
    }
    
    std::thread::id main_thread_id;
} // ::

cGLRenderer::cGLRenderer()
{
    glbinding::initialize( 0, &Platform::get_proc_address, true );
    for( auto& function : glbinding::Binding::functions() )
    {
        if( std::string_view{ "glGetError" } != function->name() )
            function->addCallbackMask( glbinding::CallbackMask::After );
    }
    /*
    glbinding::setAfterCallback([]( const glbinding::FunctionCall& ){
        if( const auto error = gl::glGetError(); error != gl::GL_NO_ERROR ){
            SK_BREAK;
            SK_WARNING( sk::Severity::kGraphics, "OpenGL Error: {}", static_cast< size_t >( error ) )
        }
    } );
    /**/
    
    cAsset_Manager::get().AddFileLoaderForExtensions(
        { "frag", "vert", "comp" }, &loadGLSL );
    
    main_thread_id = std::this_thread::get_id();
} // cRenderer

cGLRenderer::~cGLRenderer()
{
    cAsset_Manager::get().RemoveFileLoaders( { "frag", "vert", "comp" } );
}

void cGLRenderer::AddGLTask( const std::function< void() >& _function, const bool _wait )
{
    if( std::this_thread::get_id() == main_thread_id )
    {
        _function();
        return;
    }
    
    get().addGLTask( _function, _wait );
}

void cGLRenderer::addGLTask( const std::function< void() >& _function, const bool _wait )
{
    std::atomic_bool completed = false;
    
    m_task_mtx_.lock();
    m_tasks_.emplace_back( sTask{ .stopper = _wait ? &completed : nullptr, .function = _function } );
    m_task_mtx_.unlock();
    
    if( _wait )
        completed.wait( false );
}

void cGLRenderer::Update()
{
    std::vector< sTask > tasks;
    m_task_mtx_.lock();
    tasks.swap( m_tasks_ );
    m_task_mtx_.unlock();

    for( auto& [ stopper, function ] : tasks )
    {
        function();
        if( stopper )
        {
            stopper->store( true );
            stopper->notify_one();
        }
    }
}

void sk::Graphics::InitRenderer()
{
    cGLRenderer::init();
}
