#pragma once
#include <ez/meta.hpp>
#include <ez/math/poly.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>
#include <algorithm>

namespace glm {
	template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
	GLM_FUNC_DECL T normalize(T const& x) {
		return x / glm::length(x);
	}
};

namespace ez {
	namespace bezier {
		
		// Writes the new middle control using the output iterator.
		// The final curve will have the same start and end points as the inputs.
		template<typename vec_t, typename Iter>
		void curveThrough(const vec_t& p0, const vec_t& p1, const vec_t& p2, Iter output) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::curveThrough requires vector types!");
			using T = vec_value_t<vec_t>;

			static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::curveThrough requires an output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::curveThrough cannot convert vector type to iterator value_type!");

			*output++ = (p1 - p0 * T(0.25) - p2 * T(0.25))* T(2.0);
		}

		// Returns the two middle controls, assuming p0 and p3 are the start and end controls.
		// The final curve will have the same start and end points as the inputs.
		template<typename vec_t, typename Iter>
		void curveThrough(const vec_t& a, const vec_t& k0, const vec_t& k1, const vec_t& d, Iter output) {
			static_assert(ez::is_vec_v<vec_t>, "ez::bezier::curveThrough requires vector types!");
			using T = vec_value_t<vec_t>;

			static_assert(ez::is_output_iterator_v<Iter>, "ez::bezier::curveThrough requires an output iterator!");
			static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "ez::bezier::curveThrough cannot convert vector type to iterator value_type!");

			vec_t b =
				T(54.0 / 18.0)* k0 +
				T(-27.0 / 18.0)* k1 +
				T(-15.0 / 18.0)* a +
				T(6.0 / 18.0)* d;

			vec_t c =
				T(27.0 / 6.0)* k0 +
				T(-8.0 / 6.0)* a +
				T(-12.0 / 6.0)* b +
				T(-1.0 / 6.0)* d;

			(*output++) = b;
			(*output++) = c;
		}

		// Function for finding the cusp on a cubic curve, if it exists.
		template<typename T, glm::length_t N>
		bool findCusp(const glm::vec<N, T>& p0, const glm::vec<N, T>& p1, const glm::vec<N, T>& p2, const glm::vec<N, T>& p3, T& out) {
			static_assert(std::is_floating_point_v<T>, "ez::bezier::findCusp requires floating point types!" );
			static_assert(N != 1, "ez::bezier::findCusp does not work for N == 1");
			
			glm::vec<N, T>
				a = T(3)* (-p0 + T(3)* p1 - T(3)* p2 + p3),
				b = T(6)* (p0 - T(2)* p1 + p2),
				c = T(3)* (p1 - p0);

			std::array<int, 2 * N> dims;
			std::array<T, 2 * N> roots;
			int rcount = 0;
			for (int i = 0; i < N; ++i) {
				int count = ez::poly::solveQuadratic(a[i], b[i], c[i], &roots[rcount]);
				for (int u = 0; u < count; ++u) {
					dims[rcount + u] = i;
				}
				rcount += count;
			}
			if (rcount == 0) {
				return false;
			}

			// Sort the results list, look for N consecutive elements in the list (one from each dim)
			// Simple selection sort, should be very fast for arrays this small.
			for (int i = 0; i < rcount; ++i) {
				int mi = i;
				for (int u = i+1; u < rcount; ++u) {
					if (roots[u] < roots[mi]) {
						mi = u;
					}
				}
				std::swap(roots[i], roots[mi]);
				std::swap(dims[i], dims[mi]);
			}

			static constexpr T eps = ez::epsilon<T>() * T(6);

			// Number of consecutive found
			int count = 1;
			// index of first in consecutives
			int first = 0;
			// Dims passed
			std::array<int, N> passed;
			passed[0] = dims[0];

			for (int i = 1; i < rcount; ++i) {
				// The root is close enough to be valid
				if (std::abs(roots[first] - roots[i]) < eps) {
					// If we have NOT passed this dim already
					if (std::find(passed.begin(), passed.begin() + count, dims[i]) == passed.end()) {
						passed[count] = dims[i];
						++count;

						if (count == N) {
							out = roots[first];
							return true;
						}
					}
				}
				else {
					count = 1;
					first = i;
					passed[0] = dims[i];
				}
			}

			return false;
		};
	};
};