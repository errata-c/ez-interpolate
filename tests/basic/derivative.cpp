#include <catch2/catch.hpp>

#include <vector>
#include <algorithm>

#include <ez/bezier/Bezier.hpp>

namespace bezier = ez::bezier;

TEST_CASE("Linear derivatives") {
	glm::vec2 p0{-5,-3}, p1{4,10};

	glm::vec2 result = bezier::derivativeAt(p0, p1, 0.f);
	glm::vec2 compare = p1 - p0;

	REQUIRE(result.x == Approx(compare.x));
	REQUIRE(result.y == Approx(compare.y));
}

TEST_CASE("Quadratic derivatives") {
	glm::vec2 p0{-5,-3}, p1{-9,12}, p2{9,10};

	std::vector<float> ts{{ 0.f, 0.25f, 0.5f, 0.75f, 1.f }};
	std::vector<glm::vec2> results;

	std::transform(ts.begin(), ts.end(), std::back_inserter(results), [p0, p1, p2](float t){
		return bezier::derivativeAt(p0, p1, p2, t);
	});

	std::vector<glm::vec2> compare{{
		glm::vec2{-8,30},
		glm::vec2{3,21.5},
		glm::vec2{14,13},
		glm::vec2{25,4.5},
		glm::vec2{36,-4}
	}};
	for (int i = 0 ; i < compare.size(); ++i) {
		INFO("t == " << ts[i]);
		REQUIRE(compare[i].x == Approx(results[i].x));
		REQUIRE(compare[i].y == Approx(results[i].y));
	}
}

TEST_CASE("Cubic derivatives") {
	glm::vec2 p0{ -5,-3 }, p1{ -9,12 }, p2{ 9,10 }, p3{10,2};

	std::vector<float> ts{ { 0.f, 0.25f, 0.5f, 0.75f, 1.f } };
	std::vector<glm::vec2> results;

	std::transform(ts.begin(), ts.end(), std::back_inserter(results), [p0, p1, p2, p3](float t) {
		return bezier::derivativeAt(p0, p1, p2, p3, t);
		});

	std::vector<glm::vec2> compare{ {
		glm::vec2{-12,45},
		glm::vec2{13.6875,21.5625},
		glm::vec2{24.75,2.25},
		glm::vec2{21.1875,-12.9375},
		glm::vec2{3,-24}
	} };
	for (int i = 0; i < compare.size(); ++i) {
		INFO("t == " << ts[i]);
		REQUIRE(compare[i].x == Approx(results[i].x));
		REQUIRE(compare[i].y == Approx(results[i].y));
	}
}