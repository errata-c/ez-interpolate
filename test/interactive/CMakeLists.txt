
find_package(ez-imgui CONFIG REQUIRED)
find_package(ez-input CONFIG REQUIRED)
find_package(ez-math CONFIG REQUIRED)
find_package(glew CONFIG REQUIRED)

add_executable(interactive_test "main.cpp" "engine.cpp" "context.cpp" "gui.cpp")
target_link_libraries(interactive_test PRIVATE 
	fmt::fmt 
	ez-interpolate 
	ez::imgui 
	ez::imgui-opengl3 
	ez::input 
	ez::math 
	GLEW::GLEW
)