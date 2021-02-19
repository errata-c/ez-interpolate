cmake_minimum_required(VERSION 3.15)

project(EZ_INTERPOLATE
VERSION 0.1.0
LANGUAGES C CXX)

option(BUILD_TESTS "Build the tests for ez-interpolate" OFF)
set(CONFIG_DIR "share/ez-interpolate")

include(GNUInstallDirs)
find_package(ez-cmake CONFIG REQUIRED)

find_package(glm CONFIG REQUIRED)

add_library(ez-interpolate INTERFACE)
target_include_directories(ez-interpolate INTERFACE "$<INSTALL_INTERFACE:include>$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>")
target_link_libraries(ez-interpolate INTERFACE glm)
target_compile_features(ez-interpolate INTERFACE cxx_std_17)

set_target_properties(ez-interpolate PROPERTIES EXPORT_NAME "interpolate")

if(${BUILD_TESTS})
	#add_subdirectory("test")
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/ez-interpolate)

install(
	DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
	TYPE INCLUDE
	FILES_MATCHING
	PATTERN "*.h" PATTERN "*.hpp"
)

install(TARGETS ez-interpolate
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
	PRECONFIG "${CMAKE_CURRENT_SOURCE_DIR}/preconfig.cmake"
)