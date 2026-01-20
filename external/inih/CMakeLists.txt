cmake_minimum_required(VERSION 3.22.1)
project(inih)

# Find source files
file(GLOB INIH_SOURCES "*.c")

# Create the Static Library
add_library(inih STATIC ${INIH_SOURCES})

# Handle Includes
target_include_directories(inih PUBLIC .)

# Set Linker Flags
target_link_options(inih PRIVATE "-Wl,--no-undefined")

add_library(inih::inih ALIAS inih)
