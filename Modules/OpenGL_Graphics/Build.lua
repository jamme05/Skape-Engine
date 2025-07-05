
local glad_Path = "glad"

module = {
    Name = "OpenGL",
    Setup_Workspace = function()
        defines( { "SK_GRAPHICS_OPENGL" } )
    end,

    IncludeLibs = function( module_dir )
        -- TODO: Make it non dependent on editor.
        return {  }
    end,

    IncludeDirs = function( module_dir )
        return { module_dir .. "/" .. glad_Path .. "/include" }
    end,
}
module.Module_Project = Module_Setup( "OpenGL",
    module.IncludeLibs,
    module.IncludeDirs
)   
