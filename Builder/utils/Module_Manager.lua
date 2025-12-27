
local modules_config = "./Settings/modules.json"
local modules_dir    = "./Modules/"
local modules        = {}

function DefaultModuleFiles( basepath, types )
    local files = {}
    for _, t in pairs( types ) do
        table.insert( files, basepath .. t )
    end
    return files
end

function ForeachModule( run )
    if modules == nil then
        return
    end

    for _, mod in pairs( modules ) do
        run( mod )
    end
end

function Get_Module_Links()
    ForeachModule( function( mod )
        local _links = {}
        filter( mod.Filter )

        if( mod.IncludeLibs ~= nil ) then
            table.insert( _links, mod.IncludeLibs( mod.Dir ) )
        end
        table.insert( _links, mod.Name )
        links( _links )
    end )
    filter()
end

function Get_Module_Includes()
    ForeachModule( function( mod )
        filter( mod.Filter )
        includes = {}
        if( mod.IncludeDirs ~= nil ) then
            table.insert( includes, mod.IncludeDirs( mod.Dir ) )
        end
        table.insert( includes, "Modules/" .. mod.Dir .. "/src" )

        includedirs( includes )
    end )
    filter()
end

function Default_Module_Setup( module_name )
    return function( module, module_dir )
    project( module_name )
        location( "Build/Modules/" .. module_name )
        filter( module.Filter )
        kind "StaticLib"
        language "C++"
        targetdir( "bin/Modules/" .. module_name )

        defines { "SK_IS_MODULE=1" }
    
        files { DefaultModuleFiles( "Modules/" .. module_dir .. "/src/**", { ".hpp", ".h", ".cpp", ".c" } ) }
        
        includedirs { CommonEngineIncludeDirs(), Get_Module_Includes() }
    end
end

function Module_Setup( module_name, library_dirs, includes )
    return function( module, module_dir ) -- Make the module dir be the absolute path.
    project( module_name )
        location( "Build/Modules/" .. module_name )
        filter( module.Filter )
        kind "StaticLib"
        language "C++"
        targetdir( "bin/Modules/" .. module_name )

        defines { "SK_IS_MODULE=1" }
    
        -- TODO: Add module.extensions to decide which files.
        files { DefaultModuleFiles( "Modules/" .. module_dir .. "/src/**", { ".hpp", ".h", ".cpp", ".c" } ) }
        
        includedirs { CommonEngineIncludeDirs(), Get_Module_Includes() }
        libdirs { library_dirs( module_dir ) }
    end
end


function Load_Module( partial_module )
    local module_dir       = modules_dir .. partial_module.Dir
    local module_file      = io.readfile( module_dir .. "/module.json" )
    local module_json, err = json.decode( module_file )
    if( err ~= nil ) then
        print( "Warning: Module Path mismatch... Unable to find module with Id: " .. partial_module.Id .. " at directory " .. module_dir .. " Correcting TODO" )
        return
    end

    local build_file = module_json[ "BuildFile" ] or "Build.lua"


    include( module_dir .. "/" .. build_file )
    module.Raw    = module_json
    module.Dir    = partial_module.Dir
    module.Filter = ""
    for i, platform in pairs( module_json[ "Supports" ][ "Platforms" ] ) do
        if( i == 1 ) then
            module.Filter = "platforms:" .. platform
        else
            module.Filter = module.Filter .. " or " .. platform
        end
    end
    if( module.Init ~= nil ) then
        module.Init( module_dir )
    end
    table.insert( modules, module )
end

function ValidateModules()
end

function Load_Modules()
    local modules_file      = io.readfile( modules_config )
    local modules_json, err = json.decode( modules_file )
    if( err ~= nil ) then
        print( err )
        return
    end
    
    for _, mod in pairs( modules_json ) do
        Load_Module( mod )
    end


    -- Will throw error inside itself if invalid.
    ValidateModules()
end

function Get_Supported_Platforms()
    local platforms_parser = {}
    ForeachModule( function( mod )
        -- Only platform in case it's coming from a platform module.
        if mod.Raw.Type ~= "Platform" then
            return
        end
        for _, p in pairs( mod.Raw.Supports.Platforms ) do
            platforms_parser[ p ] = 1
        end
    end )
    local platforms = {}
    for p, _ in pairs( platforms_parser ) do
        table.insert( platforms, p )
    end
    return platforms
end

function Setup_Workspace()
    ForeachModule( function( mod ) mod.Setup_Workspace( mod ) end )
    filter()
end

function CreateModules()
    includedirs( Get_Module_Includes() )
    ForeachModule( function( mod ) mod.Module_Project( mod, mod.Dir ) end )
    filter()
end