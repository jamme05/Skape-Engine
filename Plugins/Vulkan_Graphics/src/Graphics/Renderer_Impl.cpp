
#include "Renderer_Impl.h"

#include "Containers/Const/Array.h"

#ifdef DEBUG
constexpr auto kValidationLayers = sk::array{
    "VK_LAYER_KHRONOS_validation",
};
#else // DEBUG
constexpr auto kValidationLayers = sk::array< const char*, 0 >{};
#endif // !DEBUG

sk::Graphics::cVKRenderer::cVKRenderer()
{
    createInstance();
}

namespace sk::Platform
{
    extern std::pair< uint32_t, const char* const* > get_extensions();
} // sk::Platform

static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback( vk::DebugUtilsMessageSeverityFlagBitsEXT _severity, vk::DebugUtilsMessageTypeFlagsEXT _type,
    const vk::DebugUtilsMessengerCallbackDataEXT* _callback_data, void* )
{
    SK_WARNING( sk::Severity::kGraphics, "validation layer: type {} msg: {}", vk::to_string( _type ), _callback_data->pMessage )
    
    return vk::False;
}

void sk::Graphics::cVKRenderer::createInstance()
{
    constexpr vk::ApplicationInfo app_info = {
        "Skape Playground",
        VK_MAKE_VERSION( 0, 0, 1 ),
        "Skape Engine",
        VK_MAKE_VERSION( 0, 0, 1 ),
        vk::ApiVersion14
    };
    
    auto layer_properties = m_context_.enumerateInstanceLayerProperties();
    const bool res = std::ranges::any_of( kValidationLayers, [ &layer_properties ]( const auto& _layer )
    {
        return std::ranges::none_of( layer_properties, [ _layer ]( const auto& _property )
        {
            return strcmp( _property.layerName, _layer ) == 0;
        } );
    } );
    
    SK_ERR_IF( res, "One or more required layers are not supported!" )
    
    auto [ extension_count, extensions_unmodified ] = sk::Platform::get_extensions();
    
    std::vector< const char* > extensions{ extensions_unmodified, extensions_unmodified + extension_count };
    if( !kValidationLayers.empty() )
    {
        extension_count++;
        extensions.emplace_back( vk::EXTDebugUtilsExtensionName );
    }
    
    auto extension_properties = m_context_.enumerateInstanceExtensionProperties();
    
    std::vector< const char* > required_extensions;
    for( uint32_t i = 0; i < extension_count; ++i )
    {
        SK_ERR_IF( std::ranges::none_of( extension_properties, [ extension = extensions[ i ] ]( const auto& _property )
        {
            return strcmp( _property.extensionName, extension ) == 0;
        } ), TEXT( "Required SDL extensions not supported: {}", extensions[ i ] ) )
    }

    const vk::InstanceCreateInfo create_info = {
        {},
        &app_info,
        kValidationLayers,
        extensions,
    };
    
    m_instance_ = vk::raii::Instance{ m_context_, create_info };
}
