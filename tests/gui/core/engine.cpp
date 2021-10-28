#include "engine.hpp"
//#include <RenderTools/RenderTools.hpp>
#include <ez/math/constants.hpp>
#include <ez/input/compat/SFML.hpp>
#include <GL/glew.h>
#include <nvg.hpp>

#include "nanovg.h"

Engine::Engine(const char * title) 
	: window(sf::VideoMode(640, 480), title)
{
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

	glewInit();

	vg = nvgCreate();
}
Engine::~Engine() {
	nvgDelete(vg);
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
		glm::vec2 dims = getWindowSize();
		glm::vec2 frame = getFrameSize();
		glClearColor(217 / 255.f, 245 / 255.f, 255 / 255.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		nvgBeginFrame(vg, dims.x, dims.y, frame.x / dims.x);
		render();

		nvgEndFrame(vg);

		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}

void Engine::beginPath() {
	nvgBeginPath(vg);
}
void Engine::moveTo(const glm::vec2& p0) {
	nvgMoveTo(vg, p0.x, p0.y);
}
void Engine::pathTo(const glm::vec2& p0) {
	nvgLineTo(vg, p0.x, p0.y);
}
void Engine::pathTo(const glm::vec2& p0, const glm::vec2& p1) {
	nvgQuadTo(vg, p0.x, p0.y, p1.x, p1.y);
}
void Engine::pathTo(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2) {
	nvgBezierTo(vg, p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
}

void Engine::circle(const glm::vec2& center, float radius) {
	nvgCircle(vg, center.x, center.y, radius);
}
void Engine::rect(const glm::vec2& pos, const glm::vec2& size) {
	nvgRect(vg, pos.x, pos.y, size.x, size.y);
}

void Engine::strokeWidth(float width) {
	nvgStrokeWidth(vg, width);
}

void Engine::stroke() {
	nvgStroke(vg);
}
void Engine::fill() {
	nvgFill(vg);
}

void Engine::strokeColor(float r, float g, float b) {
	nvgStrokeColor(vg, nvgRGBf(r,g,b));
}
void Engine::fillColor(float r, float g, float b) {
	nvgFillColor(vg, nvgRGBf(r,g,b));
}

void Engine::translate(const glm::vec2& offset) {
	nvgTranslate(vg, offset.x, offset.y);
}
void Engine::scale(float amount) {
	nvgScale(vg, amount, amount);
}
void Engine::scale(const glm::vec2& amount) {
	nvgScale(vg, amount.x, amount.y);
}

void Engine::save() {
	nvgSave(vg);
}
void Engine::restore() {
	nvgRestore(vg);
}