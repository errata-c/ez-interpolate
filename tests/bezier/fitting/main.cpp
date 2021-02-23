#include <memory>
#include <vector>
#include <string>

#include <ez/window/Window.hpp>
#include <ez/window/Engine.hpp>
#include <ez/window/BasicEngine.hpp>

#include <ez/imgui/Context.hpp>
#include <imgui.h>
#include <ez/gl.hpp>

#include <glm/vec2.hpp>
#include <fmt/format.h>
#include <ez/bezier/Bezier.hpp>
#include <ez/bezier/BPath.hpp>

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

class BasicWindow : public ez::window::Window {
public:
	BasicWindow(std::string_view _title, glm::ivec2 size, ez_window::Style _style, const ez_window::RenderSettings& rs)
		: Window(_title, size, _style, rs)
		, context(*this)
	{
		ez::gl::load();

		ctx = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

		ImGui::StyleColorsDark();
	}
	~BasicWindow() {

	}

	void handleInput() override {
		// make the ImGui context active
		context.makeActive();

		ez::InputEvent ev;
		while (pollInput(ev)) {
			context.processEvent(ev);
			if (ev.type == ez::InEv::Closed) {
				close();
				// Window will not be destroyed until full frame cycle is complete.
			}
		}
	}

	void draw() override {
		glm::vec2 size = getSize();

		// make the ImGui context active
		context.makeActive();

		// Make the opengl context active
		setActive(true);

		context.newFrame(*this);
		nvgBeginFrame(ctx, size.x, size.y, 1.f);

		ImGui::Begin("Controls");
		{
			static const double low = 0.0, high = 1.0;
			ImGui::SliderScalar("alph", ImGuiDataType_Double, (void*)&ez::Bezier::alph, (const void*)&low, (const void*)&high, "%.10f", 1.f);
			ImGui::SliderScalar("gamma", ImGuiDataType_Double, (void*)&ez::Bezier::gamma, (const void*)&low, (const void*)&high, "%.10f", 1.f);
		}
		ImGui::End();

		std::array<double, 7> tdata{
			1.0 / 8.0,
			2.0 / 8.0,
			3.0 / 8.0,

			4.0 / 8.0,
			5.0 / 8.0,
			6.0 / 8.0,
			7.0 / 8.0
		};
		std::array<double, 7> vdata{
			0.1,
			0.2,
			0.25,

			0.26,
			0.27,
			0.3,
			0.5,
		};

		std::array<double, 4> curve;

		ez::Bezier::fitCubic(vdata.begin(), vdata.begin() + 7, tdata.begin(), tdata.begin() + 7, curve.begin());

		nvgFillColor(ctx, nvgRGB(0,0,0));
		for (int i = 0; i < 4; ++i) {
			nvgBeginPath(ctx);
			nvgCircle(ctx, tdata[i], vdata[i], 4.f);
			nvgFill(ctx);
		}

		ImGuiIO& io = ImGui::GetIO();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		nvgEndFrame(ctx);

		context.render();
	}

	void drawCubic(const std::array<glm::dvec2, 4>& p) {
		nvgBeginPath(ctx);
		nvgMoveTo(ctx, p[0].x, p[0].y);
		nvgBezierTo(ctx, p[1].x, p[1].y, p[2].x, p[2].y, p[3].x, p[3].y);
		nvgStroke(ctx);
	}
private:
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	float f = 0.0f;
	int counter = 0;

	std::string text;

	NVGcontext* ctx;
	ez::imgui::Context context;
};

int main(int argc, char* argv[]) {
	ez::window::BasicEngine engine;
	
	ez_window::GLSettings gset;
	gset.majorVersion() = 4;
	gset.minorVersion() = 5;
	gset.depthBits() = 24;
	gset.stencilBits() = 8; // We need a stencil buffer for nanovg

	BasicWindow* window = new BasicWindow("bezier fitting", { 800, 600 }, ez_window::Style::Default, gset);
	engine.add(window);
	
	return engine.run(argc, argv);
}
