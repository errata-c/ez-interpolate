#pragma once
#include <cinttypes>
#include <deque>
#include <ez/interpolate/PointCloud.hpp>

class Context {
public:
	Context();

	bool hasPointSelect() const;
	void setPointSelect(std::ptrdiff_t i);
	std::ptrdiff_t getPointSelect() const;
	std::ptrdiff_t numPoints() const;

	bool hasControlSelect() const;
	void setControlSelect(std::ptrdiff_t i);
	std::ptrdiff_t getControlSelect() const;
	std::ptrdiff_t numControls() const;

	bool hasInputSelect() const;
	void setInputSelect(std::ptrdiff_t i);
	std::ptrdiff_t getInputSelect() const;
	std::ptrdiff_t numInputs() const;

	bool hasReduction() const;
	ez::PointCloud<glm::vec2>& getReduction();
	const ez::PointCloud<glm::vec2>& getReduction() const;

	void buildCurve();
	std::vector<glm::vec2>& getCurve();
	const std::vector<glm::vec2>& getCurve() const;

	std::vector<float>& getInputs();
	const std::vector<float>& getInputs() const;
private:
	std::ptrdiff_t pointSelect, controlSelect, inputSelect;
	std::deque<ez::PointCloud<glm::vec2>> reduction;

	std::vector<float> inputs;
	std::vector<glm::vec2> curve;
};