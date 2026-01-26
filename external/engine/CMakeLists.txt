cmake_minimum_required(VERSION 3.22.1)
project(engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect Source Files
# CONFIGURE_DEPENDS tells CMake to try and check the directory at build time
file(GLOB ENGINE_SOURCES CONFIGURE_DEPENDS "*.cpp")

# Exclude Windows and Linux folders
list(FILTER ENGINE_SOURCES EXCLUDE REGEX "/(linux|windows)/")

# Create the Shared Library
add_library(engine SHARED ${ENGINE_SOURCES})

# Include Paths & Exporting
target_include_directories(engine 
	PUBLIC 
		# For users of the library (after install)
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        # For building the library itself (local path)
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE
		# Internal headers
		${CMAKE_CURRENT_SOURCE_DIR}/include/engine
)

# Link Dependencies
target_link_libraries(engine
    PUBLIC        
        SDL3::SDL3
		inih::inih
        SDL3_image::SDL3_image
		SDL3_mixer::SDL3_mixer
	PRIVATE
        # System Libraries (the -l prefix is handled by find_library or direct naming)
		GLESv3
        OpenSLES
)

add_library(engine::engine ALIAS engine)
