#include <memory>
#include <vector>
#include <string>
#include <glm/vec2.hpp>
#include <fmt/format.h>
#include <ez/bezier/BPath.hpp>
#include <ez/bezier/CPath.hpp>
#include <ez/bezier/Bezier.hpp>

#include <ez/gl.hpp>

#include <ez/window/Window.hpp>
#include <ez/window/Engine.hpp>
#include <ez/window/BasicEngine.hpp>

#include <ez/imgui/Context.hpp>
#include <imgui.h>

#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

class Window: public ez::window::Window {
public:
    Window(std::string_view _title, glm::ivec2 size, ez_window::Style _style, const ez_window::RenderSettings& rs)
        : ez::window::Window(_title, size, _style, rs)
        , context(*this)
        , index(-1)
    {
        ez::gl::load();

        ctx = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

        ImGui::StyleColorsDark();

        glClearColor(1, 1, 1, 1);

        path.append(glm::vec2{ 0.2, 0.8 });
        path.append(glm::vec2{ 0.2, 0.2 });
        path.append(glm::vec2{ 0.4, 0.1 });
        path.append(glm::vec2{ 0.8, 0.2 });
        path.append(glm::vec2{ 0.8, 0.8 });

        fmt::print("The path has {} segments.\n", path.numSegments());
    }
    ~Window() {

    }
    void handleInput(const ez::InputEvent& ev) {
        if (index == -1) {
            if (ev.type == ez::InputEventType::MousePress && ev.mouse.button == ez::Mouse::Left) {
                glm::vec2 mpos = ev.mouse.position;
                glm::vec2 frame = getViewportSize();

                float dist2 = 8;
                std::ptrdiff_t select = -1;

                std::ptrdiff_t count = static_cast<std::ptrdiff_t>(path.size());
                for (std::ptrdiff_t i = 0; i < count; ++i) {
                    glm::vec2 cpos = path[i];
                    float dist1 = glm::length(cpos * frame - mpos);
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
                glm::vec2 frame = getViewportSize();
                mpos /= frame;

                path[index] = mpos;
            }
        }
    }

    void handleInput() override {
        // make the ImGui context active
        context.makeActive();

        ez::InputEvent ev;
        while (pollInput(ev)) {
            context.processEvent(ev);
            
            handleInput(ev);

            if (ev.type == ez::InEv::Closed) {
                close();
            }
        }
    }

    void draw() override {
        glm::vec2 window = getSize();
        glm::vec2 frame = getViewportSize();

        context.newFrame(*this);

        // make the ImGui context active
        context.makeActive();

        // Make the opengl context active
        setActive(true);
        
        nvgBeginFrame(ctx, window.x, window.y, window.x / frame.x);

        drawPath(frame);

        ImGuiIO& io = ImGui::GetIO();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        nvgEndFrame(ctx);
        context.render();

        glFlush();
    }

    void drawPath(glm::vec2 size) {
        nvgStrokeWidth(ctx, 3.f);

        nvgFillColor(ctx, nvgRGBf(0, 0, 1));
        for (auto& point : path) {
            glm::vec2 tmp = point * size;
            drawCircle(tmp, 5.f);
        }

        nvgStrokeColor(ctx, nvgRGBf(0, 0, 0));

        nvgBeginPath(ctx);

        std::size_t count = path.numSegments();
        for (std::size_t i = 0; i < count; ++i) {
            auto seg = path.segmentAt(i);
            for (auto& point : seg) {
                point = point * size;
            }

            nvgMoveTo(ctx, seg[0].x, seg[0].y);
            nvgBezierTo(ctx, seg[1].x, seg[1].y, seg[2].x, seg[2].y, seg[3].x, seg[3].y);
        }
        nvgStroke(ctx);
    }
    void drawCubic(const std::array<glm::vec2, 4>& p) {
        nvgBeginPath(ctx);
        nvgMoveTo(ctx, p[0].x, p[0].y);
        nvgBezierTo(ctx, p[1].x, p[1].y, p[2].x, p[2].y, p[3].x, p[3].y);
        nvgStroke(ctx);
    }
    void drawCircle(glm::vec2 pos, float radius) {
        nvgBeginPath(ctx);
        nvgArc(ctx, pos.x, pos.y, radius, 0.f, ez::tau<float>(), NVG_CCW);
        nvgFill(ctx);
    }
private:
    NVGcontext* ctx;
    ez::imgui::Context context;

    ez::BPath<glm::vec2> path;
    std::ptrdiff_t index;
};

int main(int argc, char ** argv) {
    ez::window::BasicEngine engine;

    ez_window::GLSettings gset;
    gset.majorVersion() = 4;
    gset.minorVersion() = 5;
    gset.depthBits() = 24;
    gset.stencilBits() = 8; // We need a stencil buffer for nanovg

    Window* window = new Window("bezier fitting", { 800, 600 }, ez_window::StylePreset::Default, gset);
    engine.add(window);

    return engine.run(argc, argv);
}
