
find_package(ez-input CONFIG REQUIRED)
find_package(ez-window CONFIG REQUIRED)
find_package(ez-imgui CONFIG REQUIRED)
find_package(nanovg CONFIG REQUIRED)

find_package(glew CONFIG REQUIRED)
find_package(rt-loader CONFIG REQUIRED COMPONENTS glew)

add_library(bezier_test_common STATIC "common/BezierCommon.cpp")
target_include_directories(bezier_test_common PUBLIC "common")
target_link_libraries(bezier_test_common PUBLIC 
	fmt::fmt 
	ez::window
	ez::imgui
	ez::imgui-opengl3 
	ez::input
	nanovg::nanovg

	rt::loader-glew

	ez::interpolate
)

#add_subdirectory("fitting")
add_subdirectory("offset")
add_subdirectory("bpath")
add_subdirectory("knots")