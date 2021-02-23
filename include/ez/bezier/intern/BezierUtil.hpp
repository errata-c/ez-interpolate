#pragma once
#include <ez/meta.hpp>
#include <ez/math/poly.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

namespace glm {
	template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	GLM_FUNC_DECL T normalize(T const& x) {
		return x / glm::length(x);
	}
};

namespace ez {
	namespace bezier {
		// Returns the middle control, assuming p0 and p2 are the start and end controls.
		template<typename vec_t, typename Iter>
		void curveThrough(const vec_t& p0, const vec_t& p1, const vec_t& p2, Iter output) {
			static_assert(is_vec_v<vec_t>, "ez::bezier::curveThrough requires vector types!");
			using T = vec_value_t<vec_t>;

			static_assert(is_output_iterator_v<Iter>, "ez::bezier::curveThrough requires an output iterator!");
			static_assert(is_iterator_writable_v<Iter, vec_t>, "ez::bezier::curveThrough cannot convert vector type to iterator value_type!");

			*output++ = (p1 - p0 * static_cast<T>(0.25) - p2 * static_cast<T>(0.25))* static_cast<T>(2.0);
		}

		// Returns the two middle controls, assuming p0 and p3 are the start and end controls.
		template<typename vec_t, typename Iter>
		void curveThrough(const vec_t& a, const vec_t& k0, const vec_t& k1, const vec_t& d, Iter output) {
			static_assert(is_vec_v<vec_t>, "ez::bezier::curveThrough requires vector types!");
			using T = vec_value_t<vec_t>;

			static_assert(is_output_iterator_v<Iter>, "ez::bezier::curveThrough requires an output iterator!");
			static_assert(is_iterator_writable_v<Iter, vec_t>, "ez::bezier::curveThrough cannot convert vector type to iterator value_type!");

			vec_t b =
				static_cast<T>(54.0 / 18.0)* k0 +
				static_cast<T>(-27.0 / 18.0)* k1 +
				static_cast<T>(-15.0 / 18.0)* a +
				static_cast<T>(6.0 / 18.0)* d;

			vec_t c =
				static_cast<T>(27.0 / 6.0)* k0 +
				static_cast<T>(-8.0 / 6.0)* a +
				static_cast<T>(-12.0 / 6.0)* b +
				static_cast<T>(-1.0 / 6.0)* d;

			(*output++) = b;
			(*output++) = c;
		}

		// Function for finding the cusp on a cubic curve, if it exists.
		template<typename T>
		bool findCusp(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, const glm::vec<2, T>& p3, T& out) {
			static_assert(std::is_floating_point_v<T>, "ez::bezier::findCusp requires floating point types!" );
			
			glm::vec<2, T>
				a = static_cast<T>(3)* (-p0 + static_cast<T>(3)* p1 - static_cast<T>(3)* p2 + p3),
				b = static_cast<T>(6)* (p0 - static_cast<T>(2)* p1 + p2),
				c = static_cast<T>(3)* (p1 - p0);

			std::array<T, 2> xroots, yroots;
			int xcount = ez::poly::solveQuadratic(a.x, b.x, c.x, xroots.begin());
			int ycount = ez::poly::solveQuadratic(a.y, b.y, c.y, yroots.begin());

			static constexpr T eps = ez::epsilon<T>() * static_cast<T>(3);

			for (int x = 0; x < xcount; ++x) {
				for (int y = 0; y < ycount; ++y) {
					if (std::abs(xroots[x] - yroots[y]) < eps) {
						// Found a place where derivative equals zero in both x and y.
						out = xroots[x];
						return true;
					}
				}
			}
			return false;
		};
	};
};