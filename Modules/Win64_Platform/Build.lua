
module = {
    Name = "Windows",
    Setup_Workspace = function()
        filter{ "platforms:Win64" }
            toolset( "clang" )
            cppdialect( "c++23" ) -- Add something like Maximum Cpp version/dialect?
            defines( { "SK_TARGET_WIN64", "SK_CPP20", "SK_GAME_DIR=\"./\"" } )

        -- May remove QW_CPP20 later.
    end,

    Module_Project = Default_Module_Setup( "Windows" )
}