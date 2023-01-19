cmake_minimum_required(VERSION 3.24)

include(FetchContent)
FetchContent_Declare(
	fmt
	GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
	GIT_TAG "9.1.0"
	FIND_PACKAGE_ARGS CONFIG
)
FetchContent_Declare(
	Catch2
	GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
	GIT_TAG "v3.2.1"
	FIND_PACKAGE_ARGS CONFIG
)

FetchContent_MakeAvailable(fmt Catch2)


add_executable(basic_test 
	"interpolate.cpp"
	"derivative.cpp"
	"length.cpp"
)
target_link_libraries(basic_test PRIVATE 
	fmt::fmt 
	Catch2::Catch2WithMain
	ez::interpolate
)