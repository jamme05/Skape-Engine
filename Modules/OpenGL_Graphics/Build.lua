
local OpenGL_Path = "glfw-3.4.bin"

module = {
    Name = "OpenGL",
    Setup_Workspace = function()
        defines( { "SK_GRAPHICS_OPENGL" } )
    end,

    Module_Project = Module_Setup( "OpenGL",
        -- Lib Dirs
        function( module_dir )
            return { OpenGL_Path .. "/lib-vc2022" } -- TODO: Make it non dependent on editor.
        end,
        -- Include Dirs
        function( module_dir )
            return { OpenGL_Path .. "/include" }
        end )
}