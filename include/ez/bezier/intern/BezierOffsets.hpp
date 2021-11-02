#pragma once
#include <ez/meta.hpp>
#include "BezierDerivatives.hpp"
#include "BezierInterpolation.hpp"
#include "BezierFitting.hpp"

namespace ez::bezier {
	namespace intern {

		// Quadratic curves offset
		template<typename T, typename output_iter>
		std::ptrdiff_t simplePixelOffsetCurve(const std::array<glm::tvec2<T>, 3> & points, T delta, output_iter output) {
			static constexpr T threshold = T(0.921);
			using vec_t = glm::tvec2<T>;

			std::ptrdiff_t count = 0;

			// To support negative offsets
			T compare = std::abs(delta);

			// offset the points first,
			// Check the middle, if its the right distance FIN
			// else subdivide and repeat.
			std::array<vec_t, 3> base = points;

			T range = T(1);
			T start = T(0);

			while (start < range) {
				vec_t n0 = bezier::tangentAtStatic<3>(base.begin(), T(0));
				vec_t n1 = bezier::tangentAtStatic<3>(base.begin(), T(0.5));

				if (glm::dot(n0, n1) < threshold) {
					// Subdivide
					range = (start + range) * T(0.5);
					bezier::leftSplitStatic<3>(base.begin(), T(0.5), base.begin());
					continue;
				}
				else {
					n0 = vec_t(-n0.y, n0.x);
					n1 = vec_t(-n1.y, n1.x);

					vec_t m = glm::normalize(n0 + n1);

					n0 = base[0] + n0 * delta;
					m = bezier::interpolateStatic<3>(base.begin(), T(0.5)) + m * delta;
					n1 = base[2] + n1 * delta;

					// Finalize the current offset segment, check if we are done.
					*output++ = n0;

					bezier::curveThrough(n0, m, n1, output);

					*output++ = n1;
					count += 3;

					if (range < T(1)) {
						// Move to next segment.
						start = range;
						range = T(1);
						bezier::rightSplitStatic<3>(points.begin(), start, base.begin());
						continue;
					}
					else {
						// Finish.
						break;
					}
				}
			} // End While

			return count;
		}// End simplePixelOffsetCurve

		// Offset for cubic
		template<typename T, typename Iter>
		std::ptrdiff_t simplePixelOffsetCurve(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, const glm::vec<2, T>& p3, T delta, Iter output) {
			static constexpr std::array<T, 4> interp{ T(0.0), T(1.0 / 3.0), T(2.0 / 3.0), T(1.0) };
			static constexpr T threshold = T(0.93);
			using vec_t = glm::tvec2<T>;
			
			std::ptrdiff_t count = 0;
			
			// offset the points first,
			// Check the middle, if its the right distance FIN
			// else subdivide and repeat.
			std::array<vec_t, 4> base{ p0, p1, p2, p3 };
			bezier::leftSplitStatic<4>(base.begin(), T(0.5), base.begin());

			T range = T(0.5);
			T start = T(0.0);

			while (start < range) {
				vec_t n0 = bezier::tangentAtStatic<4>(base.begin(), T(0));
				vec_t n1 = bezier::tangentAtStatic<4>(base.begin(), T(0.5));

				if (glm::dot(n0, n1) < threshold) {
					// Subdivide
					range = (start + range) * T(0.5);
					bezier::leftSplitStatic<4>(base.begin(), T(0.5), base.begin());
					continue;
				}
				else {
					n0 = vec_t(-n0.y, n0.x);
					n1 = vec_t(-n1.y, n1.x);

					vec_t n2 = base[3] - base[2];
					n2 = vec_t(-n2.y, n2.x);
					n2 = glm::normalize(n2);

					vec_t m0 = glm::normalize(n0 + n1);
					vec_t m1 = glm::normalize(n2 + n1);

					n0 = base[0] + n0 * delta;

					m0 = bezier::interpolateStatic<4>(base.begin(), interp[1])
						+ m0 * delta;

					m1 = bezier::interpolateStatic<4>(base.begin(), interp[2])
						+ m1 * delta;

					n2 = base[3] + n2 * delta;

					*output++ = n0;
					bezier::curveThrough(n0, m0, m1, n2, output);
					*output++ = n2;
					count += 4;

					if (range < T(1)) {
						// Move to next segment.
						start = range;
						range = T(1);
						bezier::rightSplit(p0, p1, p2, p3, start, base.begin());
						continue;
					}
					else {
						// Finish.
						break;
					}
				}
			} // End While

			return count;
		} // End simplePixelOffsetCurve

		// Tapered offset for cubic 
		template<typename T, typename Iter>
		std::ptrdiff_t taperedPixelOffsetCurve(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, const glm::vec<2, T>& p3, const glm::vec<4, T>& taper, Iter output) {
			static constexpr std::array<T, 4> interp{ T(0.0), T(1.0 / 3.0), T(2.0 / 3.0), T(1.0) };
			// Since its tapered, use slightly tighter bounds
			static constexpr T threshold = T(0.97);
			using vec_t = glm::tvec2<T>;

			std::ptrdiff_t count = 0;

			// offset the points first,
			// Check the middle, if its the right distance FIN
			// else subdivide and repeat.
			std::array<T, 4> taperBase{ taper.x, taper.y, taper.z, taper.w };
			std::array<vec_t, 4> base{ p0, p1, p2, p3 };
			bezier::leftSplitStatic<4>(base.begin(), T(0.5), base.begin());
			bezier::leftSplitStatic<4>(taperBase.begin(), T(0.5), taperBase.begin());

			T range = T(0.5);
			T start = T(0.0);

			while (start < range) {
				vec_t n0 = bezier::tangentAtStatic<4>(base.begin(), T(0));
				vec_t n1 = bezier::tangentAtStatic<4>(base.begin(), T(0.5));

				// Essentially just checking if the angle is greater than some predetermined angle.
				// In this case arccos(0.97) ~= 14 degrees
				if (glm::dot(n0, n1) < threshold) {
					// Subdivide
					range = (start + range) * T(0.5);
					bezier::leftSplitStatic<4>(base.begin(), T(0.5), base.begin());
					bezier::leftSplitStatic<4>(taperBase.begin(), T(0.5), taperBase.begin());
					continue;
				}
				else {
					// Finalize a segment of the offset curve

					n0 = vec_t(-n0.y, n0.x);
					n1 = vec_t(-n1.y, n1.x);

					vec_t n2 = base[3] - base[2];
					n2 = vec_t(-n2.y, n2.x);
					n2 = glm::normalize(n2);

					vec_t m0 = glm::normalize(n0 + n1);
					vec_t m1 = glm::normalize(n2 + n1);

					n0 = base[0] + n0 * bezier::interpolateStatic<4>(taperBase.begin(), interp[0]);

					m0 = bezier::interpolateStatic<4>(base.begin(), interp[1]) 
						+ m0 * bezier::interpolateStatic<4>(taperBase.begin(), interp[1]);

					m1 = bezier::interpolateStatic<4>(base.begin(), interp[2])
						+ m1 * bezier::interpolateStatic<4>(taperBase.begin(), interp[2]);

					n2 = base[3] + n2 * bezier::interpolateStatic<4>(taperBase.begin(), interp[3]);

					*output++ = n0;

					bezier::curveThrough(n0, m0, m1, n2, output);

					*output++ = n2;
					count += 4;

					// Finalize the current offset segment, check if we are done.

					if (range < T(1)) {
						// Move to next segment.
						start = range;
						range = T(1);
						bezier::rightSplit(p0, p1, p2, p3, start, base.begin());
						bezier::rightSplitStatic<4>(&taper[0], start, taperBase.begin());
						continue;
					}
					else {
						// Finish.
						break;
					}
				}
			} // End While

			return count;
		} // End taperedPixelOffsetCurve

	}; // End namespace intern

	// Offset a quadratic bezier curve, assuming that all coordinates are in pixel scale (essentially integer precision).
	template<typename T, typename Iter>
	std::ptrdiff_t pixelOffset(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, T delta, Iter output) {
		static_assert(ez::is_output_iterator_v<Iter>, "The iterator passed in is not a proper output iterator!");
		using vec_t = glm::vec<2, T>;
		static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

		return intern::simplePixelOffsetCurve({p0, p1, p2}, -delta, output);
	}

	// Offset a cubic bezier curve, assuming that all coordinates are in pixel scale (essentially integer precision).
	template<typename T, typename Iter>
	std::ptrdiff_t pixelOffset(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, const glm::vec<2, T>& p3, T delta, Iter output) {
		static_assert(ez::is_output_iterator_v<Iter>, "The iterator passed in is not a proper output iterator!");
		using vec_t = glm::vec<2, T>;
		static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

		return intern::simplePixelOffsetCurve(p0, p1, p2, p3, -delta, output);
	}
	
	// Offset a cubic bezier curve, using taper values
	template<typename T, typename Iter>
	std::ptrdiff_t taperedPixelOffset(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, const glm::vec<2, T>& p3, glm::vec<4, T> taper, Iter output) {
		static_assert(ez::is_output_iterator_v<Iter>, "The iterator passed in is not a proper output iterator!");
		using vec_t = glm::vec<2, T>;
		static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

		return intern::taperedPixelOffsetCurve(p0, p1, p2, p3, -taper, output);
	}

}; // End namespace ez
