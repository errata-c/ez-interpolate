#include "context.hpp"
#include <cassert>
#include <algorithm>

Context::Context()
	: pointSelect(-1)
	, controlSelect(-1)
	, inputSelect(-1)
{
	inputs.resize(3, 0.f);
	reduction.resize(5);

	glm::vec2 point{50, 300};
	for (int i = 0; i < 5; ++i) {
		ez::PointCloud<glm::vec2>& reduce = reduction[i];
		reduce.resizeInputs(3);
		reduce.push_back(point);

		for (int u = 0; u < inputs.size(); ++u) {
			auto& knot = reduce.getKnot(0, u);
			//knot.domain = 1024.f;
			knot.position = inputs[u];
		}

		point.x += 100;
	}
}

bool Context::hasPointSelect() const {
	return pointSelect >= 0;
}
void Context::setPointSelect(std::ptrdiff_t i) {
	if (i >= 0) {
		pointSelect = std::max(std::ptrdiff_t(0), std::min(i, numPoints()-1));
		controlSelect = -1;
	}
	else {
		pointSelect = -1;
	}
}
std::ptrdiff_t Context::getPointSelect() const {
	return pointSelect;
}
std::ptrdiff_t Context::numPoints() const {
	return reduction.size();
}

bool Context::hasControlSelect() const {
	return controlSelect >= 0;
}
void Context::setControlSelect(std::ptrdiff_t i) {
	if (i >= 0) {
		controlSelect = std::max(std::ptrdiff_t(0), std::min(i, numControls()));
	}
	else {
		controlSelect = -1;
	}
}
std::ptrdiff_t Context::getControlSelect() const {
	return controlSelect;
}
std::ptrdiff_t Context::numControls() const {
	if (hasPointSelect()) {
		return reduction[pointSelect].numPoints();
	}
	else {
		return 0;
	}
}

bool Context::hasInputSelect() const {
	return inputSelect >= 0;
}
void Context::setInputSelect(std::ptrdiff_t i) {
	if (i >= 0) {
		inputSelect = std::max(std::ptrdiff_t(0), std::min(i, numInputs()));
	}
	else {
		inputSelect = -1;
	}
}
std::ptrdiff_t Context::getInputSelect() const {
	return inputSelect;
}
std::ptrdiff_t Context::numInputs() const {
	return static_cast<std::ptrdiff_t>(inputs.size());
}

bool Context::hasReduction() const {
	return hasPointSelect();
}
ez::PointCloud<glm::vec2>& Context::getReduction() {
	assert(hasReduction());
	return reduction[pointSelect];
}
const ez::PointCloud<glm::vec2>& Context::getReduction() const {
	assert(hasReduction());
	return reduction[pointSelect];
}

void Context::buildCurve() {
	curve.clear();
	for (ez::PointCloud<glm::vec2> & reduce : reduction) {
		glm::vec2 result = reduce.eval(inputs.begin(), inputs.end());
		curve.push_back(result);
	}
}
std::vector<glm::vec2>& Context::getCurve() {
	return curve;
}
const std::vector<glm::vec2>& Context::getCurve() const {
	return curve;
}

std::vector<float>& Context::getInputs() {
	return inputs;
}
const std::vector<float>& Context::getInputs() const {
	return inputs;
}