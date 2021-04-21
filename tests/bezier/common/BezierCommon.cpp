#include "BezierCommon.hpp"

#include <rt/loader.hpp>

#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

BasicWindow::BasicWindow(std::string_view name)
    : ez::Window(name, glm::ivec2{ 800, 600 }, ez::window::StylePreset::Default, getRS())
    , ctx(nullptr)
{
    setActive(true);

    ctx = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
}

void BasicWindow::handleInput() {
    ez::InputEvent ev;
    while (pollInput(ev)) {
        imguiContext.processEvent(ev);
        handleEvent(ev);

        if (ev.type == ez::InEv::Closed) {
            close();
        }
    }
}

void BasicWindow::draw() {
    imguiContext.newFrame(*this);

    drawGUI();

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec2 window = getSize();
    glm::vec2 frame = getViewportSize();

    nvgBeginFrame(ctx, window.x, window.y, window.x / frame.x);

    drawVG();

    nvgEndFrame(ctx);

    imguiContext.render();

    glDisable(GL_BLEND);
}

void BasicWindow::beginPath() {
    nvgBeginPath(ctx);
}
void BasicWindow::strokeWidth(float width) {
    nvgStrokeWidth(ctx, width);
}
void BasicWindow::strokeColor(float r, float g, float b) {
    nvgStrokeColor(ctx, nvgRGBf(r, g, b));
}
void BasicWindow::fillColor(float r, float g, float b) {
    nvgFillColor(ctx, nvgRGBf(r, g, b));
}
void BasicWindow::stroke() {
    nvgStroke(ctx);
}
void BasicWindow::fill() {
    nvgFill(ctx);
}

void BasicWindow::moveTo(glm::vec2 pos) {
    nvgMoveTo(ctx, pos.x, pos.y);
}
void BasicWindow::bezierTo(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2) {
    nvgBezierTo(ctx, p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
}
void BasicWindow::bezierTo(glm::vec2 p0, glm::vec2 p1) {
    nvgQuadTo(ctx, p0.x, p0.y, p1.x, p1.y);
}
void BasicWindow::lineTo(glm::vec2 p) {
    nvgLineTo(ctx, p.x, p.y);
}
void BasicWindow::drawPoint(glm::vec2 pos, float radius) {
    nvgBeginPath(ctx);
    nvgArc(ctx, pos.x, pos.y, radius, 0.f, ez::tau<float>(), NVG_CCW);
    nvgFill(ctx);
}