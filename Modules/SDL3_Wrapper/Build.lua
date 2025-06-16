
local SDL_Path = "SDL3-3.2.16"

module = {
    Name = "SDL3",
    Setup_Workspace = function()
        defines( { "SK_WINDOW_SDL3", "SK_WINDOW_SDL=3" } )
    end,

    Module_Project = Module_Setup( "SDL3",
        -- Lib Dirs
        function( module_dir )
            return { SDL_Path .. "/lib" }
        end,
        -- Include Dirs
        function( module_dir )
            return { SDL_Path .. "/include" }
        end )
}