#pragma once
#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <ez/input/InputEngine.hpp>

class Engine {
public:
	Engine(const char * title);

	int run();
	
	virtual void handleEvent(const ez::InputEvent& ev) = 0;
	virtual void gui() = 0;
	virtual void render() = 0;

	glm::vec2 getWindowSize() const;
	glm::vec2 getFrameSize() const;

	sf::RenderWindow window;
};