cmake_minimum_required(VERSION 3.22.1)
project(MainModule)

# Define this at the top of your main CMakeLists.txt
set(ROOT_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/../../../../..")
set(EXTERNAL_LIBS "${ROOT_FOLDER}/external")

# Add Dependencies
add_subdirectory("${EXTERNAL_LIBS}/SDL" SDL)
add_subdirectory("${EXTERNAL_LIBS}/SDL_image" SDL_image)
add_subdirectory("${EXTERNAL_LIBS}/SDL_mixer" SDL_mixer)
add_subdirectory("${EXTERNAL_LIBS}/engine" engine)
add_subdirectory("${EXTERNAL_LIBS}/inih" inih)
add_subdirectory("${ROOT_FOLDER}/SweetCrush" SweetCrush)

# FORCE the output name to be lowercase 'sdl' so the IDE is happy
# Even if the target is named 'SDL', the file on disk will be 'libsdl3.so'
if(TARGET SDL3)
    set_target_properties(SDL3 PROPERTIES OUTPUT_NAME "sdl3")
elseif(TARGET SDL3-shared)
    set_target_properties(SDL3-shared PROPERTIES OUTPUT_NAME "sdl3")
endif()

add_definitions(-DVK_USE_PLATFORM_OHOS=1)
# Create the 'entry' Shared Library
add_library(entry SHARED dummy.cpp)
add_dependencies(entry SDL3::SDL3-shared)

# Include Paths
# If SDL_PATH was active, you'd uncomment the line below:
# target_include_directories(entry PRIVATE ../SDL/include)

# Link Libraries
target_link_libraries(entry
    PUBLIC
    SDL3::SDL3         # This links the SDL3 library + the N-API bridge code
    App
    libace_napi.z.so   # Required for the N-API bridge inside SDL3
    libhilog_ndk.z.so  # For logging
)
