
local vulkan_dir     = "%VULKAN_SDK%"
local vulkan_hpp_dir = "Vulkan-Hpp"
local vulkan_hpp_config = "cmake -DVULKAN_HPP_SAMPLES_BUILD=ON -DVULKAN_HPP_SAMPLES_BUILD_WITH_LOCAL_VULKAN_HPP=ON -DVULKAN_HPP_TESTS_BUILD=ON -DVULKAN_HPP_TESTS_BUILD_WITH_LOCAL_VULKAN_HPP=ON"

module = {
    Name = "Vulkan",
    Setup_Workspace = function( mod )
        filter( mod.Filter )
        defines( { "SK_GRAPHICS_VULKAN" } )
    end,

    Init = function( module_dir )
        CmakePreBuild( path.join( module_dir, vulkan_hpp_dir ), "", vulkan_hpp_config )
        CMakeBuilder( path.join( module_dir, vulkan_hpp_dir ), "", "Release" )
        -- Find a way to have the rebuild be optional? Or with a certain interval?
        -- os.execute( ".\\Build\\Modules\\Vulkan_Graphics\\Vulkan-Hpp\\cmake\\Release\\VulkanHppGenerator" )
    end,

    IncludeLibs = function( module_dir )
        return {}
    end,

    -- TODO: Add cache.
    IncludeDirs = function( module_dir )
        local includes = {}

        table.insert( includes, path.join( vulkan_dir, "Include" ) )
        table.insert( includes, path.join( module_dir, vulkan_hpp_dir ) )

        return includes
    end,

    Module_Project = Default_Module_Setup( "Vulkan" )
}