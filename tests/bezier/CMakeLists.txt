
find_package(ez-input CONFIG REQUIRED)
find_package(ez-window CONFIG REQUIRED)
find_package(ez-imgui CONFIG REQUIRED)
find_package(nanovg CONFIG REQUIRED)
find_package(ez-glcompat CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)

add_library(bezier_test_common INTERFACE)
target_link_libraries(bezier_test_common INTERFACE 
	fmt::fmt 
	ez::window
	ez::imgui
	ez::imgui-opengl3 
	ez::input
	nanovg::nanovg
	GLEW::GLEW
	ez::glcompat
	ez-interpolate
)

#add_subdirectory("fitting")
add_subdirectory("offset")
add_subdirectory("qpath")
add_subdirectory("knots")