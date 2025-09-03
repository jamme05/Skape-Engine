
local SDL_Path = "SDL3-3.2.16"

-- For the future: https://www.youtube.com/watch?v=Mi47TQ4Tsr8 (Add SDL3 as a git submodule)

module = {
    Name = "SDL3",
    Setup_Workspace = function()
        defines( { "SK_WINDOW_SDL3", "SK_WINDOW_SDL=3" } )
    end,

    Init = function( module_name )
    end,

    IncludeLibs = function( module_dir )
        local dll_path = path.join( "Modules", module_dir, SDL_Path, "lib/x64/SDL3.dll" )

        local ok, err = os.copyfile( dll_path, "game/bin/SDL3.dll" )
        if( not ok ) then
            print( err )
        end

        return {
            path.join( "Modules", module_dir, SDL_Path, "lib/x64/SDL3.lib" ),
        }
    end,

    -- TODO: Add cache.
    IncludeDirs = function( module_dir )
        return { path.join( "Modules", module_dir, SDL_Path, "include" ), "src/framework" }
    end,
}
module.Module_Project = Module_Setup( "SDL3",
    module.IncludeLibs,
    function( module_dir )
        local includes = module.IncludeDirs( module_dir )
        table.insert( includes, "src/framework" )
        return includes
    end
)