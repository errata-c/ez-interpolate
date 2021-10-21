#include "engine.hpp"
//#include <RenderTools/RenderTools.hpp>
#include <ez/math/constants.hpp>
#include <ez/input/compat/SFML.hpp>
#include <GL/glew.h>

Engine::Engine(const char * title) 
	: window(sf::VideoMode(640, 480), title)
{
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

	glewInit();
}

glm::vec2 Engine::getWindowSize() const {
	auto size = window.getSize();
	return {size.x, size.y};
}
glm::vec2 Engine::getFrameSize() const {
	const auto& view = window.getView();
	return {view.getSize().x, view.getSize().y};
}

int Engine::run() {
	sf::Clock deltaClock;
	while (window.isOpen()) {
		sf::Event sfev;
		ez::InputEvent ev;
		while (window.pollEvent(sfev)) {
			ImGui::SFML::ProcessEvent(sfev);

			ev = ez::input::remapEvent(sfev);
			handleEvent(ev);

			if (ev.type == ez::InputEventType::Closed) {
				window.close();
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());
		gui();

		window.clear();
		render();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}
