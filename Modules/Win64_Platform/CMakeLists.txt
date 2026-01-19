project(Win64_Platform)

set(SKAPE_WIN64_PLUGIN_NAME "Win64")

SkapeAddPlugin(${SKAPE_WIN64_PLUGIN_NAME} "" "" "")
SkapeAddPlatformSupport("Win64" ${SKAPE_WIN64_PLUGIN_NAME})