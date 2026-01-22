project(STB_Image)

add_library(stb_image INTERFACE)

target_sources(stb_image
  INTERFACE
    stb_image.h
)
