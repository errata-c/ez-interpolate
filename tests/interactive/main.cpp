#include <ez/window/BasicEngine.hpp>
#include "window.hpp"

int main(int argc, char* argv[]) {
	ez::window::BasicEngine engine;
	
	ez::window::GLSettings rset;
	rset.stencilBits() = 8;
	rset.depthBits() = 24;
	rset.majorVersion() = 4;
	rset.minorVersion() = 5;
	engine.add(new Window("ez-interpolate interactive", { 800, 600 }, ez::window::StylePreset::Default, rset));

	return engine.run(argc, argv);
}