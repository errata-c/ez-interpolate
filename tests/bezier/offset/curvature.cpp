#include <memory>
#include <vector>

#include "Core.hpp"

#include <glm/vec2.hpp>
#include <fmt/format.h>
#include <ez/geo/Bezier.hpp>
#include <ez/geo/BPath.hpp>
#include <ez/math/Polynomial.hpp>

// How to make this work...
void handleEvent(const ez::InputEvent& ev) {
    if (index == -1) {
        if (ev.type == ez::InputEventType::MousePress && ev.mouse.isPressed(ez::Mouse::Left)) {
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
        if (ev.type == ez::InputEventType::MouseRelease && ev.mouse.isPressed(ez::Mouse::Left)) {
            index = -1;
        }
        else if (ev.type == ez::InputEventType::MouseMove) {
            glm::vec2 mpos = ev.mouse.position;

            curve[index] = mpos;
        }
    }
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

int main(int argc, char** argv) {
    Core core("Curvature test", { 800, 600 });

    std::ptrdiff_t index = -1;
    ez::CBezier<float, 2> curve{
        glm::vec2(100, 500),
        glm::vec2(100, 100),
        glm::vec2(700, 100),
        glm::vec2(700, 500)
    };
    std::vector<glm::vec2> offset;

    NVGcontext* ctx = core.vg;
    
    while (core.beginFrame()) {

        nvgStrokeColor(ctx, nvgRGBf(0, 0, 0));
        nvgFillColor(ctx, nvgRGBf(0.2, 0.2, 0.8));
        nvgStrokeWidth(ctx, 3.f);

        glm::vec2 prev = curve[0];
        glm::vec2 prevTan = ez::Bezier::tangent(curve[0], curve[1], curve[2], curve[3], 0.f);

        for (int i = 0; i < 200; ++i) {
            float t = float(i) / 199.f;

            glm::vec2 tmp = curve.evalAt(t);
            glm::vec2 tan = ez::Bezier::tangent(curve[0], curve[1], curve[2], curve[3], t);

            float d = glm::dot(tan, prevTan);

            d = std::sqrt(2.f - 2.f * d) * 30.f;

            nvgStrokeColor(ctx, nvgRGBf(d, 0, 1.f - d));

            nvgBeginPath(ctx);
            moveTo(ctx, prev);
            nvgLineTo(ctx, tmp.x, tmp.y);
            nvgStroke(ctx);

            prevTan = tan;
            prev = tmp;
        }


        for (int i = 0; i < 4; ++i) {
            glm::vec2 pos = curve[i];

            drawPoint(ctx, pos);
        }

        core.endFrame();
    }

    return 0;
}
