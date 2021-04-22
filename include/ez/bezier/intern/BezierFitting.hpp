#pragma once
#include <ez/meta.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>
#include "BezierInterpolation.hpp"

namespace ez {
	namespace bezier {
		// This whole section is iffy...

		static constexpr double alph = 0.05;
		static constexpr double gamma = 0.9999;
		static constexpr std::size_t itermin = 45;

		template<typename VecIter, typename ValueIter, typename OutIter>
		void fitCubic(VecIter vecStart, VecIter vecEnd, ValueIter valueStart, ValueIter valueEnd, OutIter output) {
			static_assert(is_random_iterator<VecIter>::value, "The vector valued iterator passed in must be random access!");
			using vec_t = typename is_random_iterator<VecIter>::vec_t;

			static_assert(is_output_iterator<OutIter, vec_t>::value, "The iterator passed in is not a proper output iterator!");
			using T = typename is_vec<vec_t>::value_type;

			static_assert(std::is_floating_point<std::remove_reference_t<decltype(*valueStart)>>::value, "The value iterator passed in must have floating point values!");
			
			assert((vecEnd - vecStart) == (valueEnd - valueStart));
			std::size_t count = vecEnd - vecStart;
			if (count == 0) {
				return;
			}

			std::array<vec_t, 4> param{
				vec_t{T(0)},
				vec_t{T(0)},
				vec_t{T(0)},
				vec_t{T(0)},
			};
			std::array<vec_t, 4> pgrad2 = param;

			std::array<T, 4> coeff;

			//static constexpr T gama = T(0.9999), igama = T(1) - gama;
			T igamma = T(1) - T(gamma);

			static constexpr T eps = T(1E-8);
			//static constexpr T alph = T(0.03);

			std::size_t iterations;
			if (count > itermin) {
				iterations = 1;
			}
			else {
				iterations = (itermin / count) + 1;
			}

			for (int iterNum = 0; iterNum < iterations; ++iterNum) {
				VecIter vecIt = vecStart;
				ValueIter valIt = valueStart;

				while (vecIt != vecEnd) {

					vec_t coord = *vecIt;
					T t = *valIt;

					{
						T t1 = T(1) - t;
						T tt = t * t;
						T t1t1 = t1 * t1;
						coeff[0] = (t1t1 * t1);
						coeff[1] = (t1t1 * t) * T(3);
						coeff[2] = (tt * t1) * T(3);
						coeff[3] = (tt * t);
					}

					T error = -coord + (param[0] * coeff[0] + param[1] * coeff[1] + param[2] * coeff[2] + param[3] * coeff[3]);

					for (int i = 0; i < 4; ++i) {
						vec_t grad = error * coeff[i];
						//pgrad2[i] = (pgrad2[i] + std::abs(grad)) * gamma + igamma * pgrad2[i];
						pgrad2[i] = pgrad2[i] * T(gamma) + grad * grad * igamma;

						vec_t delparam = -(T(alph) * grad) / std::sqrt(pgrad2[i] + eps);

						param[i] += delparam;
					}

					++valIt;
					++vecIt;
				}
			}

			for (vec_t & p : param) {
				(*output) = p;
				++output;
			}
		};

		template<typename VecIter, typename ValueIter, typename OutIter>
		void fitCubicADAGRAD(VecIter vecStart, VecIter vecEnd, ValueIter valueStart, ValueIter valueEnd, OutIter output) {
			static_assert(is_random_iterator<VecIter>::value, "The vector valued iterator passed in must be random access!");
			using vec_t = typename is_random_iterator<VecIter>::vec_t;

			static_assert(is_output_iterator<OutIter, vec_t>::value, "The iterator passed in is not a proper output iterator!");
			using T = typename is_vec<vec_t>::value_type;

			static_assert(std::is_floating_point<decltype(*valueStart)>::value, "The value iterator passed in must have floating point values!");

			assert((vecEnd - vecStart) == (valueEnd - valueStart));
			std::size_t count = vecEnd - vecStart;
			if (count == 0) {
				return;
			}

			std::array<vec_t, 4> param{
				vec_t{T(0)},
				vec_t{T(0)},
				vec_t{T(0)},
				vec_t{T(0)},
			};
			std::array<vec_t, 4> pgrad2 = param;

			std::array<T, 4> coeff;

			//static constexpr T gama = T(0.9999), igama = T(1) - gama;
			T igama = T(1) - T(gamma);

			static constexpr T eps = T(1E-8);
			//static constexpr T alph = T(0.03);



			std::size_t iterations;
			if (count > itermin) {
				iterations = 1;
			}
			else {
				iterations = (itermin / count) + 1;
			}

			for (int i = 0; i < iterations; ++i) {
				VecIter vecIt = vecStart;
				ValueIter valIt = valueStart;

				while (vecIt != vecEnd) {

					vec_t coord = *vecIt;
					T t = *valIt;

					{
						T t1 = T(1) - t;
						T tt = t * t;
						T t1t1 = t1 * t1;
						coeff[0] = (t1t1 * t1);
						coeff[1] = (t1t1 * t) * T(3);
						coeff[2] = (tt * t1) * T(3);
						coeff[3] = (tt * t);
					}

					T error = -coord + (param[0] * coeff[0] + param[1] * coeff[1] + param[2] * coeff[2] + param[3] * coeff[3]);

					for (int i = 0; i < 4; ++i) {
						vec_t grad = error * coeff[i];
						pgrad2[i] = pgrad2[i] * T(gamma) + grad * grad * igama;

						vec_t delparam = -(T(alph) * grad) / (std::sqrt(pgrad2[i]) + eps);

						param[i] += delparam;
					}

					++valIt;
					++vecIt;
				}
			}

			for (vec_t& p : param) {
				(*output) = p;
				++output;
			}
		};

		template<typename VecIter, typename ValueIter, typename OutIter>
		void fitCubicSurface(VecIter vecStart, VecIter vecEnd, ValueIter valueStart, ValueIter valueEnd, OutIter outputRow0, OutIter outputRow1, OutIter outputRow2, OutIter outputRow3) {
			static_assert(is_random_iterator<VecIter>::value, "The vector valued iterator passed in must be random access!");
			using vec_t = typename is_random_iterator<VecIter>::vec_t;

			static_assert(is_output_iterator<OutIter, vec_t>::value, "The iterator passed in is not a proper output iterator!");
			using T = typename is_vec<vec_t>::value_type;

			static_assert(std::is_floating_point<decltype(*valueStart)>::value, "The value iterator passed in must have floating point values!");

			assert((vecEnd - vecStart) == (valueEnd - valueStart));
			std::size_t count = vecEnd - vecStart;
			if (count == 0) {
				return;
			}

			std::array<std::array<vec_t, 4>, 4> param{
				std::array<vec_t, 4 >> {
					vec_t{T(0)},
					vec_t{T(0)},
					vec_t{T(0)},
					vec_t{T(0)},
				},
				std::array<vec_t, 4 >> {
					vec_t{T(0)},
					vec_t{T(0)},
					vec_t{T(0)},
					vec_t{T(0)},
				},
				std::array<vec_t, 4 >> {
					vec_t{T(0)},
					vec_t{T(0)},
					vec_t{T(0)},
					vec_t{T(0)},
				},
				std::array<vec_t, 4 >> {
					vec_t{T(0)},
					vec_t{T(0)},
					vec_t{T(0)},
					vec_t{T(0)},
				}
			};
			std::array<std::array<vec_t, 4>, 4> pgrad2 = param;

			std::array<T, 4> coeff;

			static constexpr T gama = T(0.9999), igama = T(1) - gama;

			static constexpr T eps = T(1E-8);
			static constexpr T alph = T(0.03);

			static constexpr std::size_t itermin = 45;

			std::size_t iterations;
			if (count > itermin) {
				iterations = 1;
			}
			else {
				iterations = (itermin / count) + 1;
			}

			/*
				Figure out some kind of randomization for this, to reduce the bias.
			*/
			for (int i = 0; i < iterations; ++i) {
				VecIter vecIt = vecStart;
				ValueIter valIt = valueStart;

				while (vecIt != vecEnd) {

					vec_t coord = *vecIt;
					T t = *valIt;

					{
						T t1 = T(1) - t;
						T tt = t * t;
						T t1t1 = t1 * t1;
						coeff[0] = (t1t1 * t1);
						coeff[1] = (t1t1 * t) * T(3);
						coeff[2] = (tt * t1) * T(3);
						coeff[3] = (tt * t);
					}

					T error = -coord + (param[0] * coeff[0] + param[1] * coeff[1] + param[2] * coeff[2] + param[3] * coeff[3]);

					for (int i = 0; i < 4; ++i) {
						vec_t grad = error * coeff[i];
						pgrad2[i] = pgrad2[i] * gama + grad * grad * igama;

						vec_t delparam = -(alph / glm::sqrt(pgrad2[i] + eps)) * grad;

						param[i] += delparam;
					}

					++valIt;
					++vecIt;
				}
			}

			for (vec_t& p : param) {
				(*output) = p;
				++output;
			}
		};
	};
};