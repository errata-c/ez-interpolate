
find_package(ez-input CONFIG REQUIRED)
find_package(ez-window CONFIG REQUIRED)
find_package(ez-imgui CONFIG REQUIRED)
find_package(nanovg CONFIG REQUIRED)

find_package(glew CONFIG REQUIRED)
find_package(rt-loader CONFIG REQUIRED COMPONENTS glew)

add_executable(bezier_tests 
	"src/BezierCommon.cpp"
	"src/main.cpp"
)
target_include_directories(bezier_tests PUBLIC "include")
target_link_libraries(bezier_tests PUBLIC 
	fmt::fmt 
	ez::window
	ez::imgui
	ez::imgui-opengl3 
	ez::input
	nanovg::nanovg

	rt::loader-glew

	ez::interpolate
)