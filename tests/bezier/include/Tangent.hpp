#pragma once
#include "BezierCommon.hpp"

class Tangent : public BezierTest {
public:
    glm::vec2 interpolate(float t) {
        return ez::bezier::interpolate(curve[0], curve[1], curve[2], curve[3], t);
    }

    Tangent(BasicWindow& _window)
        : BezierTest("Bezier Length", _window)
        , index(-1)
    {
        curve[0] = glm::vec2(100, 500);
        curve[1] = glm::vec2(100, 100);
        curve[2] = glm::vec2(700, 100);
        curve[3] = glm::vec2(700, 500);
    }

    void handleEvent(const ez::InputEvent& ev) override {
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

    void drawGUI() override {
        
    }

    void drawVG() override {
        strokeColor(0, 0, 0);
        fillColor(0.2, 0.2, 0.2);
        strokeWidth(3);

        beginPath();
        moveTo(curve[0]);
        bezierTo(curve[1], curve[2], curve[3]);
        stroke();

        for (int i = 0; i < 4; ++i) {
            if (i == index) {
                fillColor(0, 0, 1);
            }
            else {
                fillColor(1, 0, 0);
            }
            drawPoint(curve[i]);
        }


    }

    std::ptrdiff_t index;
    std::array<glm::vec2, 4> curve;
};