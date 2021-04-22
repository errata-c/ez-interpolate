#pragma once
#include <memory>
#include "BezierCommon.hpp"
#include "BPath.hpp"
#include "Offset.hpp"
#include "Length.hpp"

class Window : public BasicWindow {
public:
    Window()
        : BasicWindow("Bezier tests")
        , currentTest(nullptr)
    {
        ImGui::StyleColorsDark();

        tests[0].reset(new BPath{ *this });
        tests[1].reset(new Offset{ *this });
        tests[2].reset(new Length{ *this });
    }
    ~Window() {}

    void handleEvent(const ez::InputEvent& ev) override {
        if (currentTest) {
            currentTest->handleEvent(ev);
        }
    }
    void drawGUI() override {
        if (currentTest) {
            currentTest->drawGUI();
        }
        else {
            ImGui::Begin("Select");

            for (auto & test : tests) {
                if (ImGui::Button(test->name.c_str())) {
                    currentTest = test.get();
                }
            }

            ImGui::End();
        }
    }
    void drawVG() override {
        if (currentTest) {
            currentTest->drawVG();
        }
    }

private:
    BezierTest* currentTest;

    std::array<std::unique_ptr<BezierTest>, 3> tests;
};

int main(int argc, char** argv) {
    ez::window::BasicEngine engine;
    engine.add(new Window{});

    return engine.run(argc, argv);
}
