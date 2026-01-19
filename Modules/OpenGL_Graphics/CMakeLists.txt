project(OpenGL_Graphics)

set(SKAPE_OPENGL_PLUGIN_NAME "OpenGL")

SkapeAddPlugin(${SKAPE_OPENGL_PLUGIN_NAME} "" "" "")
SkapeAddGraphicsSupport("OpenGL" ${SKAPE_OPENGL_PLUGIN_NAME})
