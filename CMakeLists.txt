project(HelloWorldSensor)
cmake_minimum_required(VERSION 3.12)

add_subdirectory(lib/open-simulation-interface)
get_directory_property(OSI_VERSION_MAJOR DIRECTORY lib/open-simulation-interface DEFINITION VERSION_MAJOR)
get_directory_property(OSI_VERSION_MINOR DIRECTORY lib/open-simulation-interface DEFINITION VERSION_MINOR)
get_directory_property(OSI_VERSION_PATCH DIRECTORY lib/open-simulation-interface DEFINITION VERSION_PATCH)
set(OSIVERSION "${OSI_VERSION_MAJOR}.${OSI_VERSION_MINOR}.${OSI_VERSION_PATCH}")

include_directories(lib/fmi2/headers)

set(FMU_INSTALL_DIR "${CMAKE_BINARY_DIR}" CACHE PATH "Target directory for generated FMU")

add_subdirectory(src/)