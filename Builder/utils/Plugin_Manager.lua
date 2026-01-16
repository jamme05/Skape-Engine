
local plugins_config = "./Settings/plugins.json"
local plugins_dir    = "./Plugins/"
local dyn_plugins    = {}
local stat_plugins   = {}

function DefaultPluginFiles( basepath, types )
    local files = {}
    for _, t in pairs( types ) do
        table.insert( files, basepath .. t )
    end
    return files
end

function ForeachPlugin( run, is_static )
    local plugins;
    if is_static then
        plugins = stat_plugins;
    else
        plugins = dyn_plugins;
    end

    if plugins == nil then
        return
    end

    for _, plugin in pairs( plugins ) do
        run( plugin )
    end
end

function Get_Plugin_Links()
    local _links = {}
    ForeachPlugin( function( plugin )
        filter( plugin.Filter )

        if( plugin.IncludeLibs ~= nil ) then
            table.insert( _links, plugin.IncludeLibs( plugin.Dir ) )
        end
        table.insert( _links, plugin.Name )
    end, true )
    ForeachPlugin( function( plugin )
        local _links = {}
        filter( plugin.Filter )

        if( plugin.IncludeLibs ~= nil ) then
            table.insert( _links, plugin.IncludeLibs( plugin.Dir ) )
        end
        table.insert( _links, plugin.Name )
    end, false )
    links( _links )
    filter()
end

function Get_Plugin_Includes()
    ForeachPlugin( function( plugin )
        filter( plugin.Filter )
        includes = {}
        if( plugin.IncludeDirs ~= nil ) then
            table.insert( includes, plugin.IncludeDirs( plugin.Dir ) )
        end
        table.insert( includes, "Plugins/" .. plugin.Dir .. "/src" )

        includedirs( includes )
    end, true )
    filter()
end

function Default_Plugin_Setup( plugin_name )
    return function( plugin, plugin_dir )
    project( plugin_name )
        location( "Build/Plugins/" .. plugin_name )
        filter( plugin.Filter )
        kind "StaticLib"
        language "C++"
        targetdir( "bin/Plugins/" .. plugin_name )

        defines { "SK_IS_PLUGIN=1" }
    
        files { DefaultPluginFiles( "Plugins/" .. plugin_dir .. "/src/**", { ".hpp", ".h", ".cpp", ".c" } ) }
        
        includedirs { CommonEngineIncludeDirs(), Get_Plugin_Includes() }
    end
end

function Plugin_Setup( plugin_name, library_dirs, includes )
    return function( plugin, plugin_dir ) -- Make the plugin dir be the absolute path.
    project( plugin_name )
        location( "Build/Plugins/" .. plugin_name )
        filter( plugin.Filter )
        kind "StaticLib"
        language "C++"
        targetdir( "bin/Plugins/" .. plugin_name )

        defines { "SK_IS_PLUGIN=1" }
    
        -- TODO: Add plugin.extensions to decide which files.
        files { DefaultPluginFiles( "Plugins/" .. plugin_dir .. "/src/**", { ".hpp", ".h", ".cpp", ".c" } ) }
        
        includedirs { CommonEngineIncludeDirs(), Get_Plugin_Includes() }
        libdirs { library_dirs( plugin_dir ) }
    end
end


function Load_Plugin( partial_plugin )
    local plugin_dir       = plugins_dir .. partial_plugin.Dir
    local plugin_file      = io.readfile( plugin_dir .. "/plugin.json" )
    local plugin_json, err = json.decode( plugin_file )
    if( err ~= nil ) then
        print( "Warning: Plugin Path mismatch... Unable to find plugin with Id: " .. partial_plugin.Id .. " at directory " .. plugin_dir .. " Correcting TODO" )
        return
    end

    local build_file = plugin_json[ "BuildFile" ] or "Build.lua"

    local plugin = {}
    include( plugin_dir .. "/" .. build_file )
    plugin.Raw    = plugin_json
    plugin.Dir    = partial_plugin.Dir
    plugin.Filter = ""
    for i, platform in pairs( plugin_json[ "Supports" ][ "Platforms" ] ) do
        if( i == 1 ) then
            plugin.Filter = "platforms:" .. platform
        else
            plugin.Filter = plugin.Filter .. " or " .. platform
        end
    end
    if( plugin.Init ~= nil ) then
        plugin.Init( plugin_dir )
    end
    table.insert( plugins, plugin )
end

function ValidatePlugins()
end

function Load_Plugins()
    local plugins_file      = io.readfile( plugins_config )
    local plugins_json, err = json.decode( plugins_file )
    if( err ~= nil ) then
        print( err )
        return
    end
    
    for _, plugin in pairs( plugins_json ) do
        Load_Plugin( plugin )
    end


    -- Will throw error inside itself if invalid.
    ValidatePlugins()
end

function Get_Supported_Platforms()
    local platforms_parser = {}
    ForeachPlugin( function( plugin )
        -- Only platform in case it's coming from a platform plugin.
        if plugin.Raw.Type ~= "Platform" then
            return
        end
        for _, p in pairs( plugin.Raw.Supports.Platforms ) do
            platforms_parser[ p ] = 1
        end
    end, true )
    local platforms = {}
    for p, _ in pairs( platforms_parser ) do
        table.insert( platforms, p )
    end
    return platforms
end

function Setup_Workspace()
    ForeachPlugin( function( plugin ) plugin.Setup_Workspace( plugin ) end, true )
    ForeachPlugin( function( plugin ) plugin.Setup_Workspace( plugin ) end, false )
    filter()
end

function CreatePlugins()
    includedirs( Get_Plugin_Includes() )
    ForeachPlugin( function( plugin ) plugin.Plugin_Project( plugin, plugin.Dir ) end, true )
    ForeachPlugin( function( plugin ) plugin.Plugin_Project( plugin, plugin.Dir ) end, false )
    filter()
end