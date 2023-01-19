cmake_minimum_required(VERSION 3.24)

project(EZ_INTERPOLATE
	VERSION 0.3.2
	LANGUAGES C CXX
)

include(FetchContent)
FetchContent_Declare(
	ez-cmake
	GIT_REPOSITORY "https://github.com/errata-c/ez-cmake.git"
	GIT_TAG "main"
)
FetchContent_Declare(
	ez-math
	GIT_REPOSITORY "https://github.com/errata-c/ez-math.git"
	GIT_TAG "main"
	FIND_PACKAGE_ARGS CONFIG
)
FetchContent_Declare(
	ez-geo
	GIT_REPOSITORY "https://github.com/errata-c/ez-geo.git"
	GIT_TAG "main"
	FIND_PACKAGE_ARGS CONFIG
)

FetchContent_MakeAvailable(ez-cmake ez-math ez-geo)

include(CMakeDependentOption)

set(EZ_INTERPOLATE_CONFIG_DIR "share/ez-interpolate" CACHE STRING "The relative directory to install package config files.")


add_library(interpolate INTERFACE)
target_include_directories(interpolate INTERFACE 
	"$<INSTALL_INTERFACE:include>"
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
)
target_link_libraries(interpolate INTERFACE 
	ez::math
	ez::geo
)
target_compile_features(interpolate INTERFACE cxx_std_17)

add_library(ez::interpolate ALIAS interpolate)


if(PROJECT_IS_TOP_LEVEL)
	include(CTest)
	cmake_dependent_option(EZ_INTERPOLATE_BUILD_GUI_TESTS "Build the gui based test executables" OFF BUILD_TESTING OFF)
	if(BUILD_TESTING)
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
endif()