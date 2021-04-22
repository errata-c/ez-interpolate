#pragma once
#include <ez/meta.hpp>
#include <glm/gtc/vec1.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>
#include "BezierUtil.hpp"

namespace ez {
	namespace bezier {
		namespace intern {
			template<typename T>
			T circleArcApprox(T D1, T D2) {
				static constexpr T coeff = T(1.0 / 3.0);
				return D2 + (D2 - D1) * coeff;
			}

			// iterations has to be odd number

			template<typename T>
			constexpr int quadIterations() {
				return 25;
			};
			template<>
			constexpr int quadIterations<double>() {
				return 41;
			};

			template<typename T>
			constexpr int cubicIterations() {
				return 35;
			};
			template<>
			constexpr int cubicIterations<double>() {
				return 61;
			};
		};

		template<typename vec_t>
		vec_value_t<vec_t> length(const vec_t& p0, const vec_t& p1) {
			static_assert(is_vec_v<vec_t>, "ez::bezier::length requires vector types!");
			using T = vec_value_t<vec_t>;
			static_assert(std::is_floating_point_v<T>, "ez::bezier::length requires floating point types!");

			return glm::length(p1 - p0);
		};

		template<typename vec_t>
		vec_value_t<vec_t> length(const vec_t& p0, const vec_t& p1, const vec_t& p2) {
			static_assert(is_vec_v<vec_t>, "ez::bezier::length requires vector types!");
			using T = vec_value_t<vec_t>;
			static_assert(std::is_floating_point_v<T>, "ez::bezier::length requires floating point types!");
			
			constexpr int N = intern::quadIterations<T>();
			constexpr int N1 = N - 1;
			constexpr T delta = T(1) / T(N1);
			// compute using a sliding window
			// account for end pieces

			T totalLen = T(0);
			T plen, slen, elen;
			T t = delta;

			vec_t
				interp0 = p0,
				interp1 = interpolate(p0, p1, p2, t),
				interp2;
			t += delta;

			plen = glm::length(interp1 - interp0);

			for (int i = 1; i < N1; ++i) {
				interp2 = interpolate(p0, p1, p2, t);
				t += delta;

				slen = glm::length(interp2 - interp1);
				elen = glm::length(interp2 - interp0);

				totalLen += intern::circleArcApprox(elen, plen + slen);

				plen = slen;
				interp0 = interp1;
				interp1 = interp2;
			}


			// Account for first segment
			{
				interp0 = p0;
				interp1 = interpolate(p0, p1, p2, delta * T(0.5));
				interp2 = interpolate(p0, p1, p2, delta);

				plen = glm::length(interp1 - interp0);
				slen = glm::length(interp2 - interp1);
				elen = glm::length(interp2 - interp0);

				totalLen += intern::circleArcApprox(elen, plen + slen);
			}
			// Account for final segment
			{
				interp0 = interpolate(p0, p1, p2, T(1) - delta);
				interp1 = interpolate(p0, p1, p2, T(1) - delta * T(0.5));
				interp2 = p3;

				plen = glm::length(interp1 - interp0);
				slen = glm::length(interp2 - interp1);
				elen = glm::length(interp2 - interp0);

				totalLen += intern::circleArcApprox(elen, plen + slen);
			}
			/**/

			return totalLen * T(0.5);
		};

		template<typename vec_t>
		vec_value_t<vec_t> length(const vec_t& p0, const vec_t& p1, const vec_t& p2, const vec_t& p3) {
			static_assert(is_vec_v<vec_t>, "ez::bezier::length requires vector types!");
			using T = vec_value_t<vec_t>;
			static_assert(std::is_floating_point_v<T>, "ez::bezier::length requires floating point types!");

			constexpr int N = intern::cubicIterations<T>();
			constexpr int N1 = N - 1;
			constexpr T delta = T(1) / T(N1);

			// compute using a sliding window
			// account for end pieces

			T totalLen = T(0);
			T plen, slen, elen;
			T t = delta;

			vec_t 
				interp0 = p0, 
				interp1 = interpolate(p0, p1, p2, p3, t), 
				interp2;
			t += delta;
			
			plen = glm::length(interp1 - interp0);

			for (int i = 1; i < N1; ++i) {
				interp2 = interpolate(p0, p1, p2, p3, t);
				t += delta;

				slen = glm::length(interp2 - interp1);
				elen = glm::length(interp2 - interp0);

				totalLen += intern::circleArcApprox(elen, plen + slen);

				plen = slen;
				interp0 = interp1;
				interp1 = interp2;
			}

			
			// Account for first segment
			{
				interp0 = p0;
				interp1 = interpolate(p0, p1, p2, p3, delta * T(0.5));
				interp2 = interpolate(p0, p1, p2, p3, delta);

				plen = glm::length(interp1 - interp0);
				slen = glm::length(interp2 - interp1);
				elen = glm::length(interp2 - interp0);

				totalLen += intern::circleArcApprox(elen, plen + slen);
			}
			// Account for final segment
			{
				interp0 = interpolate(p0, p1, p2, p3, T(1) - delta);
				interp1 = interpolate(p0, p1, p2, p3, T(1) - delta * T(0.5));
				interp2 = p3;

				plen = glm::length(interp1 - interp0);
				slen = glm::length(interp2 - interp1);
				elen = glm::length(interp2 - interp0);

				totalLen += intern::circleArcApprox(elen, plen + slen);
			}
			/**/

			return totalLen * T(0.5);
		}

		template<typename Iter>
		iterator_value_t<Iter> lengthRange(Iter begin, Iter end) {
			using vec_t = iterator_value_t<Iter>;
			static_assert(is_random_iterator_v<Iter>, "ez::bezier::lengthRange requires a random access iterator!");
			static_assert(is_vec_v<vec_t>, "ez::bezier::lengthRange requires vector types!");
			using T = vec_value_t<vec_t>;
			static_assert(std::is_floating_point_v<T>, "ez::bezier::lengthRange requires floating point types!");


			std::size_t N1 = ((end - begin) - 1) * 10;

			T delta = T(1) / T(N1);
			T last = delta * (T(N1) - T(0.5));

			T len = T(0);

			vec_t prior = p0;
			for (T t = delta; t < end; t += delta) {
				vec_t mid = interpolate(p0, p1, p2, p3, t);
				t += delta;
				vec_t post = interpolate(p0, p1, p2, p3, t);

				len += intern::circleArcApprox(glm::length(post - prior), glm::length(mid - prior) + glm::length(post - mid));
				prior = post;
			}

			return len;
		}
	};
};