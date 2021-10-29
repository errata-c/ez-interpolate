
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


add_executable(gui_approx_test "approx.cpp")
target_link_libraries(gui_approx_test PRIVATE gui_test_interface)

add_executable(gui_split_test "split.cpp")
target_link_libraries(gui_split_test PRIVATE gui_test_interface)

add_executable(gui_through_test "through.cpp")
target_link_libraries(gui_through_test PRIVATE gui_test_interface)

add_executable(gui_bpath_test "bpath.cpp")
target_link_libraries(gui_bpath_test PRIVATE gui_test_interface)

add_executable(gui_bpath2_test "bpath2.cpp")
target_link_libraries(gui_bpath2_test PRIVATE gui_test_interface)

add_executable(gui_length_test "length.cpp")
target_link_libraries(gui_length_test PRIVATE gui_test_interface)

add_executable(gui_offset_test "offset.cpp")
target_link_libraries(gui_offset_test PRIVATE gui_test_interface)