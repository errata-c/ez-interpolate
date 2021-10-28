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
	virtual ~Engine();

	int run();
	
	virtual void handleEvent(const ez::InputEvent& ev) = 0;
	virtual void gui() = 0;
	virtual void render() = 0;

	glm::vec2 getWindowSize() const;
	glm::vec2 getFrameSize() const;

	// VG functions
	void beginPath();
	void moveTo(const glm::vec2& p0);
	void pathTo(const glm::vec2& p0);
	void pathTo(const glm::vec2& p0, const glm::vec2& p1);
	void pathTo(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2);

	void circle(const glm::vec2 & center, float radius);
	void rect(const glm::vec2 & pos, const glm::vec2 & size);

	void strokeWidth(float width);

	void stroke();
	void fill();

	void strokeColor(float r, float g, float b);
	void fillColor(float r, float g, float b);

	void translate(const glm::vec2 & offset);
	void scale(float amount);
	void scale(const glm::vec2 & amount);

	void save();
	void restore();

	// Data
	struct NVGcontext* vg;
	sf::RenderWindow window;
};