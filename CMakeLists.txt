cmake_minimum_required(VERSION 3.15)

project(EZ_INTERPOLATE
VERSION 0.2.1
LANGUAGES C CXX)

include(GNUInstallDirs)
include(FetchContent)

FetchContent_Declare(
	ez-cmake
	GIT_REPOSITORY "https://github.com/errata-c/ez-cmake.git"
	GIT_TAG "a59477ebb9d8ce1f686b99118586310f47cb7a80"
)
FetchContent_MakeAvailable(ez-cmake)

option(BUILD_TESTS "Build the test executables" ON)
option(SEPARATE_DIRECTORY "Install the package into its own directory" ON)
set(CONFIG_DIR "${CMAKE_INSTALL_DATAROOTDIR}/ez-interpolate" CACHE STRING "The relative directory to install package config files.")


find_package(ez-math CONFIG REQUIRED)

add_library(interpolate INTERFACE)

target_include_directories(interpolate INTERFACE 
	"$<INSTALL_INTERFACE:include>"
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
)

target_link_libraries(interpolate INTERFACE 
	ez::math
)

target_compile_features(interpolate INTERFACE cxx_std_17)

add_library(ez::interpolate ALIAS interpolate)

if(BUILD_TESTS)
	add_subdirectory("tests")
endif()

if(SEPARATE_DIRECTORY)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/ez-interpolate)
endif()


install(
	DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
	TYPE INCLUDE
	FILES_MATCHING
	PATTERN "*.h" PATTERN "*.hpp"
)

install(TARGETS interpolate
	EXPORT ez-interpolate-targets
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}/$<CONFIG>"
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/$<CONFIG>"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/$<CONFIG>"
)

install_package(
	NAME "ez-interpolate"
	NAMESPACE "ez::"
	EXPORT "ez-interpolate-targets"
	VERSION "${EZ_INTERPOLATE_VERSION}"
	COMPATIBILITY "SameMajorVersion"
	DESTINATION "${CONFIG_DIR}"
	PRECONFIG "cmake/preconfig.cmake"

	ARCH_INDEPENDENT
)