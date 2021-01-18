#include "gui.hpp"
#include "imgui.h"
#include "context.hpp"
#include <fmt/format.h>

GUI::GUI(Context& ctx)
	: context(ctx)
{}

void GUI::draw() {
	ImGui::Text("There are %d curve points.", context.numPoints());
	if (context.hasPointSelect()) {
		ImGui::Text("Point %d is selected.", context.getPointSelect());
	}
	else {
		ImGui::TextUnformatted("No point selected.");
	}

	if (context.hasControlSelect()) {
		ImGui::Text("Control %d is selected.", context.getControlSelect());
	}
	else {
		ImGui::TextUnformatted("No control selected.");
	}

	if (context.hasInputSelect()) {
		ImGui::Text("Input %d is selected.", context.getInputSelect());
	}
	else {
		ImGui::TextUnformatted("No input selected.");
	}

	static constexpr int64_t step = 1;
	std::ptrdiff_t pointSelect = context.getPointSelect();

	if (ImGui::InputScalar("##sca", ImGuiDataType_S64, &pointSelect, &step, &step)) {
		context.setPointSelect(pointSelect);
		pointSelect = context.getPointSelect();
	}

	std::vector<float>& inputs = context.getInputs();
	if (ImGui::ListBoxHeader("##Inputs", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
		ImGui::Columns(2, "99", true);
		ImGui::Separator();
		ImGui::Text("Index"); ImGui::NextColumn();
		ImGui::Text("Value"); ImGui::NextColumn();
		ImGui::Separator();

		std::ptrdiff_t select = context.getInputSelect();

		static constexpr char form[] = "{}";
		char text[16];
		for (std::ptrdiff_t i = 0; i < inputs.size(); ++i) {
			fmt::format_to_n(text, sizeof(text), fmt::string_view(form, sizeof(form)), i);
			if (ImGui::Selectable(text, i == select)) {
				if (i == select) {
					context.setInputSelect(-1);
				}
				else {
					context.setInputSelect(i);
				}
			}
			ImGui::NextColumn();

			ImGui::PushID(i);
			ImGui::DragFloat("", &inputs[i], 0.001f);
			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::ListBoxFooter();
	}
}