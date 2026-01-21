cmake_minimum_required(VERSION 3.22.1)
project(App)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the OpenGL ES v3 library (or v1/v3 as needed)
find_library(
    GLESv3-lib
    GLESv3
)

# Collect Source Files
# CONFIGURE_DEPENDS tells CMake to try and check the directory at build time
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "src/*.cpp" "gameData/*.cpp")

# Create the Shared Library
add_library(App SHARED ${APP_SOURCES})

# Include Paths
target_include_directories(App PRIVATE src)

# Link Dependencies
target_link_libraries(App
    PRIVATE
        engine::engine
        ${GLESv3-lib}
)

# Linker Flags
target_link_options(App PRIVATE "-Wl,--no-undefined")
