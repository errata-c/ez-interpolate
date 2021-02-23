#include <ez/imgui/ImguiEngine.hpp>

#include <memory>
#include <vector>

#include <glm/vec2.hpp>
#include <fmt/format.h>
#include <ez/geo/BezierFunctions.hpp>
#include <RenderTools/RenderTools.hpp>
#include "imgui.h"

#include <Test.hpp>
#include <CSplit.hpp>
#include <QSplit.hpp>
#include <QOffset.hpp>
#include <COffset.hpp>
#include <QThrough.hpp>
#include <CThrough.hpp>
#include <QTangent.hpp>
#include <CTangent.hpp>
#include <CFitting.hpp>

class Engine: public ImGuiEngine {
public:
    Engine()
        : index(0)
    {}

    virtual void postInitialize() override {
        glClearColor(1, 1, 1, 1);

        NVGcontext* ctx = getNVGContext();

        tests.emplace_back(new CFitting{ ctx });
        tests.emplace_back(new CSplit{ ctx });
        tests.emplace_back(new QSplit{ ctx });
        tests.emplace_back(new QOffset{ ctx });
        tests.emplace_back(new COffset{ ctx });
        tests.emplace_back(new QThrough{ ctx });
        tests.emplace_back(new CThrough{ ctx });
        tests.emplace_back(new QTangent{ ctx });
        tests.emplace_back(new CTangent{ ctx });
    }
    virtual void handleEvent(const ez::InputEvent& ev) override {

    }
    virtual void buildGui() override {
        ImGui::Begin("Test Select");
        
        if (ImGui::ListBoxHeader("", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {

            for (std::size_t i = 0; i < tests.size(); ++i) {
                if (ImGui::Selectable(tests[i]->getName().data(), i == index)) {
                    if (i != index) {
                        index = i;
                    }
                }
            }

            ImGui::ListBoxFooter();
        }
      

        ImGui::End();
    }
    virtual void render() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec2 window = getWindowSize();
        glm::vec2 frame = getFrameSize();

        NVGcontext* ctx = getNVGContext();
        nvgBeginFrame(ctx, window.x, window.y, window.x / frame.x);

        tests[index]->draw(frame);

        nvgEndFrame(ctx);

        glFlush();
    }

    std::vector<std::unique_ptr<Test>> tests;
    std::size_t index;
};

// Length function I know works, just very slowly.
float trustedLength(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2) {
    float total = 0.f;
    constexpr int N = 512;
    constexpr float delta = 1.f / static_cast<float>(N - 1);
    constexpr float end = 1.f - delta / 2.f;
    glm::vec2 prior = p0;
    for (float i = delta; i < end; i += delta) {
        glm::vec2 post = ez::Bezier::interpolate(p0, p1, p2, i);
        total += glm::length(post - prior);
        prior = post;
    }

    total += glm::length(p2 - prior);
    return total;
}

int main(int argc, char** argv) {
    Engine engine;
    
    if (!engine.initialize()) {
        return -1;
    }

    if (!engine.createWindow(800, 600, "Bezier Multi Test")) {
        return -1;
    }

    return engine.run();
}