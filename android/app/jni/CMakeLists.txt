cmake_minimum_required(VERSION 3.22.1)

project(MainModule)

set(EXTERNAL_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/../../../external")

# SDL_mixer options
set(SDLMIXER_GME OFF CACHE BOOL "Disable GME support" FORCE)
set(SDLMIXER_FLAC_LIBFLAC OFF CACHE BOOL "Disable FLAC via libFLAC support" FORCE)
set(SDLMIXER_FLAC_DRFLAC OFF CACHE BOOL "Disable FLAC via drFLAC support" FORCE)
set(SDLMIXER_MOD_XMP OFF CACHE BOOL "Disable MOD support" FORCE)
set(SDLMIXER_OPUS OFF CACHE BOOL "Disable OPUS support" FORCE)
set(SDLMIXER_MIDI_FLUIDSYNTH  OFF CACHE BOOL "Disable Fluidsynth support" FORCE)
set(SDLMIXER_MIDI_TIMIDITY OFF CACHE BOOL "Disable Timidity support" FORCE)
set(SDLMIXER_MP3_DRMP3  OFF CACHE BOOL "Disable MP3 via dr_mp3 support" FORCE)
set(SDLMIXER_MP3_MPG123  OFF CACHE BOOL "Disable MP3 via libmpg123 support" FORCE)

#TODO SDL_image options

# Add Dependencies
add_subdirectory("${EXTERNAL_LIBS}/SDL" SDL)
add_subdirectory("${EXTERNAL_LIBS}/SDL_image" SDL_image)
add_subdirectory("${EXTERNAL_LIBS}/SDL_mixer" SDL_mixer)
add_subdirectory("${EXTERNAL_LIBS}/engine" engine)
add_subdirectory("${EXTERNAL_LIBS}/inih" inih)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../../SweetCrush" App)

# Collect Source Files
# Equivalent to $(wildcard $(LOCAL_PATH)/src/*.c)
file(GLOB_RECURSE SOURCE_FILES
        "src/*.c"
        "src/*.cpp"
)

# Create the 'main' Shared Library
add_library(main SHARED ${SOURCE_FILES})

# Link Libraries
target_link_libraries(main
        App
)
