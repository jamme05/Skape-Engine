
module = {
    Name = "Vulkan",
    Setup_Workspace = function( mod )
        filter( mod.Filter )
        defines( { "SK_GRAPHICS_VULKAN" } )
    end,

    Module_Project = Default_Module_Setup( "Vulkan" )
}