#pragma once
#include <cinttypes>
#include <deque>
#include <ez/interpolate/PointCloud.hpp>

struct Select {
	static constexpr std::size_t None = ~std::size_t(0);
	
	Select(const Select&) noexcept = default;
	Select& operator=(const Select&) noexcept = default;
	~Select() = default;

	Select() noexcept
		: index(None)
	{}
	Select(std::size_t i) noexcept
		: index(i)
	{}

	void reset() noexcept {
		index = None;
	}
	void reset(std::size_t i) noexcept {
		index = i;
	}

	std::size_t get() const noexcept {
		return index;
	}

	explicit operator bool() const noexcept {
		return index != None;
	}

	std::size_t index;
};

class Context {
public:
	using Reduction = ez::PointCloud<float, 1, 2>;

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
	Reduction& getReduction();
	const Reduction& getReduction() const;

	void buildCurve();
	std::vector<glm::vec2>& getCurve();
	const std::vector<glm::vec2>& getCurve() const;
private:
	std::ptrdiff_t pointSelect, controlSelect, inputSelect;
	std::deque<Reduction> reduction;

	std::vector<glm::vec2> curve;
};