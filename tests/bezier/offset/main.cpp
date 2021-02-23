#include <ez/window/Window.hpp>
#include <ez/window/BasicEngine.hpp>

#include <memory>
#include <vector>

#include <glm/vec2.hpp>
#include <fmt/format.h>
#include <ez/bezier/Bezier.hpp>
#include <ez/math/poly.hpp>

#include <ez/gl.hpp>

#include "imgui.h"

struct NVGcontext;
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

#include <ez/imgui/Context.hpp>

class Window : public ez::Window {
public:
    Window(std::string_view name, glm::ivec2 size, ez::window::Style _style, const ez::window::RenderSettings& rs)
        : ez::window::Window(name, size, _style, rs)
        , index(-1)
        , mode(1)
        , ctx(nullptr)
        , imguiContext(*this)
    {
        setActive(true);
        ez::gl::load();

        ctx = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);  

        curve[0] = glm::vec2(100, 500);
        curve[1] = glm::vec2(100, 100);
        curve[2] = glm::vec2(700, 100);
        curve[3] = glm::vec2(700, 500);
    }
    void handleInput() override {
        imguiContext.makeActive();

        ez::InputEvent ev;
        while (pollInput(ev)) {
            imguiContext.processEvent(ev);
            handleEvent(ev);
            
            if (ev.type == ez::InEv::Closed) {
                close();
            }
        }
    }
    void handleEvent(const ez::InputEvent & ev) {
        if (index == -1) {
            if (ev.type == ez::InputEventType::MousePress && ev.mouse.button == ez::Mouse::Left) {
                glm::vec2 mpos = ev.mouse.position;

                float dist2 = 8;
                std::ptrdiff_t select = -1;

                std::ptrdiff_t count = static_cast<std::ptrdiff_t>(4);
                for (std::ptrdiff_t i = 0; i < count; ++i) {
                    glm::vec2 cpos = curve[i];
                    float dist1 = glm::length(cpos - mpos);
                    if (dist1 < dist2) {
                        select = i;
                        dist2 = dist1;
                    }
                }

                index = select;
            }
        }
        else {
            if (ev.type == ez::InputEventType::MouseRelease && ev.mouse.button == ez::Mouse::Left) {
                index = -1;
            }
            else if (ev.type == ez::InputEventType::MouseMove) {
                glm::vec2 mpos = ev.mouse.position;

                curve[index] = mpos;
            }
        }
    }
    void buildGui() {
        ImGui::Begin("Edit");

        ImGui::RadioButton("Taper", &mode, 1);
        ImGui::RadioButton("Normal", &mode, 2);

        ImGui::End();
    }
    void draw() override {
        imguiContext.makeActive();
        imguiContext.newFrame(*this);
        buildGui();

        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec2 window = getSize();
        glm::vec2 frame = getViewportSize();
        
        nvgBeginFrame(ctx, window.x, window.y, window.x / frame.x);

        nvgStrokeColor(ctx, nvgRGBf(0, 0, 0));
        nvgFillColor(ctx, nvgRGBf(0.2, 0.2, 0.8));
        nvgStrokeWidth(ctx, 3.f);

        nvgBeginPath(ctx);
        moveTo(ctx, curve[0]);
        bezierTo(ctx, curve[1], curve[2], curve[3]);
        nvgStroke(ctx);
        
        offset.clear();
        if (mode == 1) {
            ez::bezier::taperedPixelOffset(curve[0], curve[1], curve[2], curve[3], glm::vec4{ 0, 32, 32, 0 }, std::back_inserter(offset));
        }
        else {
           ez::bezier::pixelOffset(curve[0], curve[1], curve[2], curve[3], 32.f, std::back_inserter(offset));
        }

        if (offset.size() > 0) {
            nvgBeginPath(ctx);
            for (std::ptrdiff_t i = 0; i < std::ptrdiff_t(offset.size()); i += 4) {
                moveTo(ctx, offset[i]);
                bezierTo(ctx, offset[i + 1], offset[i + 2], offset[i + 3]);
            }
            nvgStrokeColor(ctx, nvgRGBf(0, 0, 0));
            nvgStroke(ctx);
        }

        for (int i = 0; i < 4; ++i) {
            glm::vec2 pos = curve[i];

            drawPoint(ctx, pos);
        }

        for (std::size_t i = 0; i < offset.size(); i += 4) {
            drawPoint(ctx, offset[i]);
            drawPoint(ctx, offset[i + 3]);
        }

        nvgEndFrame(ctx);

        imguiContext.render();

        glDisable(GL_BLEND);
    }

    void moveTo(NVGcontext* ctx, glm::vec2 pos) {
        nvgMoveTo(ctx, pos.x, pos.y);
    }
    void bezierTo(NVGcontext* ctx, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2) {
        nvgBezierTo(ctx, p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
    }
    void bezierTo(NVGcontext* ctx, glm::vec2 p0, glm::vec2 p1) {
        nvgQuadTo(ctx, p0.x, p0.y, p1.x, p1.y);
    }
    void lineTo(NVGcontext* ctx, glm::vec2 p) {
        nvgLineTo(ctx, p.x, p.y);
    }
    void drawPoint(NVGcontext* ctx, glm::vec2 pos) {
        nvgBeginPath(ctx);
        nvgArc(ctx, pos.x, pos.y, 5.f, 0.f, ez::tau<float>(), NVG_CCW);
        nvgFill(ctx);
    }

    struct NVGcontext* ctx;
    ez::imgui::Context imguiContext;

    std::ptrdiff_t index;
    std::array<glm::vec2, 4> curve;
    std::vector<glm::vec2> offset;

    int mode;
};

int main(int argc, char** argv) {
    ez::window::BasicEngine engine;

    ez::window::GLSettings rset;
    rset.majorVersion() = 4;
    rset.minorVersion() = 5;
    rset.stencilBits() = 8;
    rset.depthBits() = 24;

    engine.add(new Window{ "bezier_offset test", {800, 600}, ez::window::StylePreset::Default,  rset});

    return engine.run(argc, argv);
}
