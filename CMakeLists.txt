cmake_minimum_required(VERSION 3.16)

project(EZ_INTERPOLATE
	VERSION 0.3.0
	LANGUAGES C CXX
)

include(FetchContent)

FetchContent_Declare(
	ez-cmake
	GIT_REPOSITORY "https://github.com/errata-c/ez-cmake.git"
	GIT_TAG "main"
)
FetchContent_MakeAvailable(ez-cmake)


option(EZ_INTERPOLATE_BUILD_TESTS "Build the test executables" OFF)
set(EZ_INTERPOLATE_CONFIG_DIR "share/ez-interpolate" CACHE STRING "The relative directory to install package config files.")


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

if(EZ_INTERPOLATE_BUILD_TESTS)
	add_subdirectory("tests")
endif()

install(
	DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
	TYPE INCLUDE
	FILES_MATCHING
	PATTERN "*.h" PATTERN "*.hpp"
)

install(TARGETS interpolate
	EXPORT ez-interpolate-targets
	RUNTIME DESTINATION "bin/$<CONFIG>"
	ARCHIVE DESTINATION "lib/$<CONFIG>"
	LIBRARY DESTINATION "lib/$<CONFIG>"
)

install_package(
	NAME "ez-interpolate"
	NAMESPACE "ez::"
	EXPORT "ez-interpolate-targets"
	VERSION "${EZ_INTERPOLATE_VERSION}"
	COMPATIBILITY "SameMajorVersion"
	DESTINATION "${EZ_INTERPOLATE_CONFIG_DIR}"
	PRECONFIG "cmake/preconfig.cmake"

	ARCH_INDEPENDENT
)