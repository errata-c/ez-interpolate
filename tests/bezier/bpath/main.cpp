#include "BezierCommon.hpp"
#include <ez/bezier/BPath.hpp>

class Window: public BasicWindow {
public:
    Window()
        : BasicWindow("BPath")
        , index(-1)
    {
        ImGui::StyleColorsDark();

        path.append(glm::vec2{ 0.2, 0.8 });
        path.append(glm::vec2{ 0.2, 0.2 });
        path.append(glm::vec2{ 0.4, 0.1 });
        path.append(glm::vec2{ 0.8, 0.2 });
        path.append(glm::vec2{ 0.8, 0.8 });

        fmt::print("The path has {} segments.\n", path.numSegments());
    }
    ~Window() {}

    void handleEvent(const ez::InputEvent& ev) override {
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

    void drawGUI() override {

    }
    void drawVG() override {
        glm::vec2 frame = getViewportSize();
        drawPath(frame);
    }

    void drawPath(glm::vec2 size) {
        strokeWidth(3.f);

        fillColor(0,0,1);
        for (auto& point : path) {
            drawPoint(point * size);
        }

        strokeColor(0,0,0);

        beginPath();

        std::size_t count = path.numSegments();
        for (std::size_t i = 0; i < count; ++i) {
            auto seg = path.segmentAt(i);
            for (auto& point : seg) {
                point = point * size;
            }

            moveTo(seg[0]);
            bezierTo(seg[1], seg[2], seg[3]);
        }
        nvgStroke(ctx);
    }
private:
    ez::BPath<glm::vec2> path;
    std::ptrdiff_t index;
};

int main(int argc, char ** argv) {
    ez::window::BasicEngine engine;
    engine.add(new Window{});

    return engine.run(argc, argv);
}
