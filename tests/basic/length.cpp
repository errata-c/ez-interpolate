#include <catch2/catch.hpp>

#include <vector>
#include <algorithm>

#include <glm/geometric.hpp>

#include <ez/bezier/Bezier.hpp>

namespace bezier = ez::bezier;

/*
What kind of guarantees can I provide for the accuracy of these length calculations?
*/

TEST_CASE("Line lengths") {
	std::vector<glm::vec2>
		p0{{ glm::vec2{-1, 0}, glm::vec2{-5, -3}, glm::vec2{2,  7} }},
		p1{{ glm::vec2{10, 4}, glm::vec2{6,   6}, glm::vec2{7, -9} }};

	std::vector<float> results;
	std::vector<float> compare;
	for (int i = 0; i < p0.size(); ++i) {
		results.push_back(bezier::length(p0[i], p1[i]));
		compare.push_back(glm::length(p1[i] - p0[i]));
	}

	for (int i = 0; i < results.size(); ++i) {
		INFO("i == " << i);
		REQUIRE(compare[i] == Approx(results[i]));
	}
}

// Arclengths calculated via integral on desmos
TEST_CASE("Quadratic lengths") {
	std::vector<glm::vec2>
		p0{ { glm::vec2{-1, 0}, glm::vec2{-5, -3}, glm::vec2{2,   7} } },
		p1{ { glm::vec2{10, 4}, glm::vec2{6,   6}, glm::vec2{7,  -9} } },
		p2{ { glm::vec2{4,  4}, glm::vec2{12, -5}, glm::vec2{19, 20} } };

	std::vector<float> results;
	
	for (int i = 0; i < p0.size(); ++i) {
		results.push_back(bezier::length(p0[i], p1[i], p2[i]));
	}

	std::vector<float> compare{{
		10.3360032529,
		20.6084839302,
		30.8623806644
	}};

	for (int i = 0; i < results.size(); ++i) {
		INFO("i == " << i);
		REQUIRE(compare[i] == Approx(results[i]));
	}
}

TEST_CASE("Cubic lengths") {
	
}