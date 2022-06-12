#include <catch2/catch_all.hpp>

#include <vector>
#include <algorithm>

#include <ez/bezier/Bezier.hpp>

namespace bezier = ez::bezier;
using Approx = Catch::Approx;

TEST_CASE("bezier linear interpolation") {
	SECTION("vec1") {
		glm::vec1 a{ 0.f }, b{ 1.f };

		glm::vec1 result = bezier::interpolate(a, b, 0.5f);

		REQUIRE(result.x == Approx(0.5));
	}

	SECTION("vec2") {
		glm::vec2 a{ 0.f }, b{ 1.f };

		glm::vec2 result = bezier::interpolate(a, b, 0.5f);

		REQUIRE(result.x == Approx(0.5));
		REQUIRE(result.y == Approx(0.5));
	}

	SECTION("vec3") {
		glm::vec3 a{ 0.f }, b{ 1.f };

		glm::vec3 result = bezier::interpolate(a, b, 0.5f);

		REQUIRE(result.x == Approx(0.5));
		REQUIRE(result.y == Approx(0.5));
		REQUIRE(result.z == Approx(0.5));
	}

	SECTION("vec4") {
		glm::vec4 a{ 0.f }, b{ 1.f };

		glm::vec4 result = bezier::interpolate(a, b, 0.5f);

		REQUIRE(result.x == Approx(0.5));
		REQUIRE(result.y == Approx(0.5));
		REQUIRE(result.z == Approx(0.5));
		REQUIRE(result.w == Approx(0.5));
	}
}

TEST_CASE("Bezier quadratic interpolation") {
	glm::vec2 p0{5,0}, p1{-3,7}, p2{-4,-1};

	std::vector<float> ts{{0.f, 0.25f, 0.5f, 0.75f, 1.f}};
	std::vector<glm::vec2> results;
	std::transform(ts.begin(), ts.end(), std::back_inserter(results), [p0, p1, p2](float t){ 
		return bezier::interpolate(p0, p1, p2, t); 
	});

	std::vector<glm::vec2> compare{{
		glm::vec2{5,0},
		glm::vec2{1.4375, 2.5625},
		glm::vec2{-1.25, 3.25},
		glm::vec2{-3.0625, 2.0625},
		glm::vec2{-4, -1}
	}};
	for (int i = 0; i < compare.size(); ++i) {
		INFO("t == " << ts[i]);
		REQUIRE(compare[i].x == Approx(results[i].x));
		REQUIRE(compare[i].y == Approx(results[i].y));
	}
}

TEST_CASE("Bezier quadratic static interpolation") {
	glm::vec2 points[] = {
		glm::vec2{ 5,0 },
		glm::vec2{ -3,7 }, 
		glm::vec2{ -4,-1 }
	};

	std::vector<float> ts{ {0.f, 0.25f, 0.5f, 0.75f, 1.f} };
	std::vector<glm::vec2> results;
	std::transform(ts.begin(), ts.end(), std::back_inserter(results), [&points](float t) {
		return bezier::interpolateStatic<3>(points, t);
	});

	std::vector<glm::vec2> compare{ {
		glm::vec2{5,0},
		glm::vec2{1.4375, 2.5625},
		glm::vec2{-1.25, 3.25},
		glm::vec2{-3.0625, 2.0625},
		glm::vec2{-4, -1}
	} };
	for (int i = 0; i < compare.size(); ++i) {
		INFO("t == " << ts[i]);
		REQUIRE(compare[i].x == Approx(results[i].x));
		REQUIRE(compare[i].y == Approx(results[i].y));
	}
}

TEST_CASE("Cubic bezier interpolation") {
	glm::vec2 p0{ 4,5 }, p1{ -3,7 }, p2{ -4,-1 }, p3{4,-1};

	std::vector<float> ts{ {0.f, 0.25f, 0.5f, 0.75f, 1.f} };
	std::vector<glm::vec2> results;
	std::transform(ts.begin(), ts.end(), std::back_inserter(results), [p0, p1, p2, p3](float t) {
		return bezier::interpolate(p0, p1, p2, p3, t);
	});

	std::vector<glm::vec2> compare{ {
		glm::vec2{4,5},
		glm::vec2{-0.078125, 4.90625},
		glm::vec2{-1.625, 2.75},
		glm::vec2{-0.359375, 0.21875},
		glm::vec2{4, -1}
	} };
	for (int i = 0; i < compare.size(); ++i) {
		INFO("t == " << ts[i]);
		REQUIRE(compare[i].x == Approx(results[i].x));
		REQUIRE(compare[i].y == Approx(results[i].y));
	}
}

TEST_CASE("Cubic bezier static interpolation") {
	glm::vec2 points[] = {
		glm::vec2{ 4,5 }, 
		glm::vec2{ -3,7 }, 
		glm::vec2{ -4,-1 }, 
		glm::vec2{ 4,-1 }
	};

	std::vector<float> ts{ {0.f, 0.25f, 0.5f, 0.75f, 1.f} };
	std::vector<glm::vec2> results;
	std::transform(ts.begin(), ts.end(), std::back_inserter(results), [&points](float t) {
		return bezier::interpolateStatic<4>(points, t);
	});

	std::vector<glm::vec2> compare{ {
		glm::vec2{4,5},
		glm::vec2{-0.078125, 4.90625},
		glm::vec2{-1.625, 2.75},
		glm::vec2{-0.359375, 0.21875},
		glm::vec2{4, -1}
	} };
	for (int i = 0; i < compare.size(); ++i) {
		INFO("t == " << ts[i]);
		REQUIRE(compare[i].x == Approx(results[i].x));
		REQUIRE(compare[i].y == Approx(results[i].y));
	}
}