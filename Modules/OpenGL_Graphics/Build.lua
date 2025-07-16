
local binder_path = "glbinding"
local binder_src = path.join( binder_path, "source" )

module = {
    Name = "OpenGL",
    Setup_Workspace = function()
        defines( { "SK_GRAPHICS_OPENGL" } )
    end,

    Init = function( module_dir )
        CMakeBuilder( path.join( module_dir, binder_path ), "", "Release" )
    end,

    IncludeLibs = function( module_dir )
        local dll_path = path.join( "Build/Modules", module_dir, binder_path, "cmake/Release/glbinding.dll" )
        os.copyfile( dll_path, "game/bin/glbinding.dll" )

        return {
            path.join( "Build/Modules", module_dir, binder_path, "cmake/Release/glbinding.lib" )
        }
    end,

    -- TODO: Add cache.
    IncludeDirs = function( module_dir )
        local includes = {}
        
        local tmp = {
            "3rdparty/KHR",
            "glbinding"
        }
        for _, include in pairs( tmp ) do
            table.insert( includes, path.join( "Modules", module_dir, binder_src, include, "include" ) )
        end

        table.insert( includes, path.join( "Build/Modules", module_dir, binder_path, "cmake/source/include" ) )
        table.insert( includes, path.join( "Build/Modules", module_dir, binder_path, "cmake/source/glbinding/include" ) )

        return includes
    end,
}
module.Module_Project = Module_Setup( "OpenGL",
    module.IncludeLibs,
    module.IncludeDirs
)
