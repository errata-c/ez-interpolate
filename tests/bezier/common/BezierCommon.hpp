#pragma once
#include <ez/window/Window.hpp>
#include <ez/window/BasicEngine.hpp>

#include <memory>
#include <vector>

#include <glm/vec2.hpp>
#include <fmt/format.h>
#include <ez/bezier/Bezier.hpp>
#include <ez/math/poly.hpp>

#include "imgui.h"

struct NVGcontext;
#include "nanovg.h"

#include <ez/imgui/Context.hpp>

class BasicWindow : public ez::Window {
public:
    static ez::window::GLSettings getRS() {
        ez::window::GLSettings rset;
        rset.majorVersion() = 4;
        rset.minorVersion() = 5;
        rset.stencilBits() = 8;
        rset.depthBits() = 24;
        return rset;
    }

    BasicWindow(std::string_view name);
    
    void handleInput() override;
    void draw() override;

    virtual void handleEvent(const ez::InputEvent& ev) = 0;
    virtual void drawGUI() = 0;
    virtual void drawVG() = 0;

    void beginPath();
    void strokeWidth(float width);
    void strokeColor(float r, float g, float b);
    void fillColor(float r, float g, float b);
    void stroke();
    void fill();

    void moveTo(glm::vec2 pos);
    void bezierTo(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2);
    void bezierTo(glm::vec2 p0, glm::vec2 p1);
    void lineTo(glm::vec2 p);
    void drawPoint(glm::vec2 pos, float radius = 5.f);

    struct NVGcontext* ctx;
    ez::imgui::Context imguiContext;
};