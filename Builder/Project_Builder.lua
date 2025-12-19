premake.override( path, "getDefaultSeparator", function( base )
	if( os.ishost( "windows" ) ) then
		return "\\"
	else
		return "/"
	end
end )

plugin_kind = "SharedLib" -- When making a game build use StaticLib.

include( "./utils/Module_Manager.lua" )

-- Return to root dir.
os.chdir( "../" )

-- Make sure game/bin exists. Without it we'll get an error.
os.mkdir( "game/bin" )

-- TODO: Move to external file.
function CmakePreBuild( dependency, cmake_path, settings )
    local dependency_path = "./Build/" .. dependency .. "/cmake"
    local return_path   = os.realpath( "./" )

    if( cmake_path == nil ) then
        cmake_path = ""
    end

    os.mkdir( dependency_path )
    os.chdir( dependency_path )
    print( "Building " .. dependency .. " files..." )
    os.execute( "cmake " .. path.join( return_path, dependency, cmake_path ) )
    if( build_config ~= nil ) then
        os.execute( "cmake " .. settings .. "-B build" )
    end
    print( "Build done.\n" )
    os.chdir( return_path )

end
function CMakeBuilder( dependency, cmake_path, build_config )
    local dependency_path = "./Build/" .. dependency .. "/cmake"
    local return_path   = os.realpath( "./" )

    if( cmake_path == nil ) then
        cmake_path = ""
    end

    os.mkdir( dependency_path )
    os.chdir( dependency_path )
    print( "Building " .. dependency .. " files..." )
    os.execute( "cmake " .. path.join( return_path, dependency, cmake_path ) )
    if( build_config ~= nil ) then
        os.execute( "cmake --build . --config " .. build_config )
    end
    print( "Build done.\n" )
    os.chdir( return_path )
end

function CommonEngineIncludeDirs()
    return { "src/engine", "external/fastgltf/include", "external/stb", "external/fastgltf/deps/" }
end

Load_Modules()

local root_build_dir = "./Build/Project/"

workspace "Skape_Playground"
    configurations { "Debug", "Release", "Final" }
    platforms { Get_Supported_Platforms() }
    startproject "Startup"

    files { "visualizers/*.natvis" }

    Setup_Workspace()

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
    includedirs { "src/framework", CommonEngineIncludeDirs() }

group "Skape"
project "Engine"
    kind "StaticLib"
    location( root_build_dir .. "engine" )
    language "C++"
    targetdir "bin/Engine" 

    Get_Module_Links()
    links { "fastgltf" }

    files { "src/engine/**.hpp", "src/engine/**.cpp", "src/engine/**.h" }

    Get_Module_Includes()
    includedirs { CommonEngineIncludeDirs() }

group "Modules"
    CreateModules()

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
