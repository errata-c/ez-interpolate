#include "engine.hpp"


int main() {
	Engine engine;
	if (!engine.initialize()) {
		return -1;
	}

	if (!engine.createWindow(800, 600, "ez-interpolate interactive")) {
		return -1;
	}

	return engine.run();
}