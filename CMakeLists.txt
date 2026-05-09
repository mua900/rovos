cmake_minimum_required(VERSION 4.0)

project(game)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# For multi-config generators (Visual Studio, Xcode)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib)
endforeach()

set(CMAKE_CXX_STANDARD 23)

add_executable(game
	src/application.hpp
	src/application.cpp
	src/game.hpp
	src/game.cpp
	src/audio.hpp
	src/audio.cpp
	src/input.hpp
	src/input.cpp
	src/ui.hpp
	src/ui.cpp
	src/text.hpp
	src/asset.hpp
	src/asset.cpp
	src/draw.hpp
	src/draw.cpp
	src/log.hpp
	src/log.cpp
	src/common.hpp
	src/common.cpp
	src/math_util.hpp
	src/math_util.cpp
	src/template.hpp
	src/main.cpp
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT game)

add_subdirectory(vendor/SDL-3.4.4 EXCLUDE_FROM_ALL)

# turn unused image formats off
set(SDLIMAGE_ANI  OFF)
set(SDLIMAGE_AVIF OFF)
set(SDLIMAGE_BMP  OFF)
set(SDLIMAGE_GIF  OFF)
set(SDLIMAGE_JPG  OFF)
set(SDLIMAGE_JXL  OFF)
set(SDLIMAGE_LBM  OFF)
set(SDLIMAGE_PCX  OFF)
set(SDLIMAGE_PNM  OFF)
set(SDLIMAGE_QOI  OFF)
set(SDLIMAGE_TGA  OFF)
set(SDLIMAGE_TIF  OFF)
set(SDLIMAGE_WEBP OFF)
set(SDLIMAGE_XCF  OFF)
set(SDLIMAGE_XPM  OFF)
set(SDLIMAGE_XV   OFF)
# png
set(SDLIMAGE_PNG ON)
set(SDLIMAGE_SVG  ON)
add_subdirectory(vendor/SDL_image-3.4.2 EXCLUDE_FROM_ALL)

add_subdirectory(vendor/SDL_mixer-3.2.0 EXCLUDE_FROM_ALL)
add_subdirectory(vendor/SDL_ttf-3.2.2 EXCLUDE_FROM_ALL)

target_include_directories(game PRIVATE vendor/SDL-3.4.4/include/)
target_include_directories(game PRIVATE vendor/SDL_image-3.4.2/include/)
target_include_directories(game PRIVATE vendor/SDL_mixer-3.2.0/include)
target_include_directories(game PRIVATE vendor/SDL_ttf-3.2.2/include/)

target_link_libraries(
	game
	PRIVATE
	SDL3::SDL3
	SDL3_image::SDL3_image
	SDL3_mixer::SDL3_mixer
	SDL3_ttf::SDL3_ttf
)
