cmake_minimum_required(VERSION 3.16)

find_package(Catch2 CONFIG REQUIRED)

add_executable(basic_test 
	"main.cpp"
	"bezier.cpp"
)
target_link_libraries(basic_test PRIVATE 
	fmt::fmt 
	Catch2::Catch2
	ez::interpolate
)