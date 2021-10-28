
# Components of the gui system, primarily based on imgui for the gui widgets, and nanovg for the vector graphics
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(nanovg CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

find_package(ez-input CONFIG REQUIRED)
find_package(ez-math CONFIG REQUIRED)

add_library(gui_test_interface STATIC
	"core/engine.cpp"
	"core/nvg.cpp"
)
target_link_libraries(gui_test_interface PUBLIC
	ImGui-SFML::ImGui-SFML
	fmt::fmt
	
	GLEW::GLEW
	nanovg::nanovg

	ez::interpolate
	ez::input
	ez::math
)
target_include_directories(gui_test_interface PUBLIC
	"core"
)


add_executable(approx_test "approx/main.cpp")
target_link_libraries(approx_test PRIVATE gui_test_interface)

add_executable(bpath_test "bpath.cpp")
target_link_libraries(bpath_test PRIVATE gui_test_interface)

