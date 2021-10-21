#include <catch2/catch.hpp>

#include <ez/bezier/Bezier.hpp>

TEST_CASE("bezier linear interpolation") {
	SECTION("vec1") {
		glm::vec1 a{ 0.f }, b{ 1.f };

		glm::vec1 result = ez::bezier::interpolate(a, b, 0.5f);

		REQUIRE(result.x == Approx(0.5));
	}

	SECTION("vec2") {
		glm::vec2 a{ 0.f }, b{ 1.f };

		glm::vec2 result = ez::bezier::interpolate(a, b, 0.5f);

		REQUIRE(result.x == Approx(0.5));
		REQUIRE(result.y == Approx(0.5));
	}

	SECTION("vec3") {
		glm::vec3 a{ 0.f }, b{ 1.f };

		glm::vec3 result = ez::bezier::interpolate(a, b, 0.5f);

		REQUIRE(result.x == Approx(0.5));
		REQUIRE(result.y == Approx(0.5));
		REQUIRE(result.z == Approx(0.5));
	}

	SECTION("vec4") {
		glm::vec4 a{ 0.f }, b{ 1.f };

		glm::vec4 result = ez::bezier::interpolate(a, b, 0.5f);

		REQUIRE(result.x == Approx(0.5));
		REQUIRE(result.y == Approx(0.5));
		REQUIRE(result.z == Approx(0.5));
		REQUIRE(result.w == Approx(0.5));
	}
}