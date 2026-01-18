premake.override( path, "getDefaultSeparator", function( base )
	if( os.ishost( "windows" ) ) then
		return "\\"
	else
		return "/"
	end
end )

plugin_kind = "SharedLib" -- When making a game build use StaticLib.

include( "./utils/Utils.lua" )
include( "./utils/Module_Manager.lua" )
include( "./utils/Plugin_Manager.lua" )

-- Return to root dir.
os.chdir( "../" )

-- Make sure game/bin exists. Without it we'll get an error.
os.mkdir( "game/bin" )

function CommonEngineIncludeDirs()
    return { "src/engine", "external/fastgltf/include", "external/stb", "external/fastgltf/deps/" }
end

Load_Modules()
Load_Plugins()

local root_build_dir = "./Build/Project/"

workspace "Skape_Playground"
    configurations { "Debug", "Release", "Final" }
    platforms { Get_Supported_Platforms() }
    startproject "Startup"

    files { "visualizers/*.natvis" }

    rtti "On"

    Setup_Workspace()
    Setup_Plugin_Workspace()

filter { "configurations:Debug" }
    defines {
        "DEBUG"
    }
    symbols "On"
    optimize "Off"

filter { "configurations:Release" }
    defines {
        "NDEBUG", "RELEASE"
    }
    symbols "On"
    optimize "Debug"

filter { "configurations:Final" }
    defines {
        "NDEBUG",
        "FINAL",
        "SK_TRACKER_DISABLED"
    }
    optimize "Full"

group "Game"
project "Framework"
    kind "StaticLib"
    location( root_build_dir .. "framework" )
    language "C++"
    targetdir "bin/Framework"

    defines { "SK_IS_MODULE=0" }

    files { "src/framework/**.hpp", "src/framework/**.cpp", "src/framework/**.h" }

    links { "Engine" }

    Get_Module_Includes()
    Get_Plugin_Includes()
    includedirs { "src/framework", CommonEngineIncludeDirs() }

project "Startup"
    kind "WindowedApp"
    location( root_build_dir .. "startup" )
    language "C++"
    targetdir "game/bin"

    defines { "SK_IS_MODULE=0" }

    links { "Engine", "Framework" }

    files { "src/startup/main.cpp" }

    Get_Module_Includes()
    Get_Plugin_Includes()
    includedirs { "src/framework", CommonEngineIncludeDirs() }

group "Skape"
project "Engine"
    kind "StaticLib"
    location( root_build_dir .. "engine" )
    language "C++"
    targetdir "bin/Engine" 

    Get_Module_Links()
    Get_Plugin_Links()
    links { "fastgltf" }

    files { "src/engine/**.hpp", "src/engine/**.cpp", "src/engine/**.h" }

    Get_Module_Includes()
    Get_Plugin_Includes()
    includedirs { CommonEngineIncludeDirs() }

group "Modules"
    CreateModules()

group "Plugins"
    CreatePlugins()

-- Rename to external?
group "Dependencies"

CMakeBuilder( "External/fastgltf", "" )

project "fastgltf"
    kind "StaticLib"
    location "Build/External/fastgltf"
    language "C++"
    targetdir "bin/external"

    includedirs { "external/fastgltf/include", "external/fastgltf/deps/simdjson/" }

    files { "external/fastgltf/src/**.cpp", "external/fastgltf/src/**.hpp", "external/fastgltf/deps/**.cpp", "external/fastgltf/deps/**.h" }
