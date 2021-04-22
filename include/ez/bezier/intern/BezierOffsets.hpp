#pragma once
#include <ez/meta.hpp>
#include "BezierDerivatives.hpp"
#include "BezierInterpolation.hpp"

namespace ez {
	namespace bezier {

		namespace intern {

			// Offset for quadratic
			template<typename T, typename Iter>
			void simplePixelOffsetCurve(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, T delta, Iter output) {
				static constexpr T threshold = T(0.921);

				// To support negative offsets
				T compare = std::abs(delta);

				// offset the points first,
				// Check the middle, if its the right distance FIN
				// else subdivide and repeat.
				std::array<glm::tvec2<T>, 3> base{ p0, p1, p2 };

				T range = T(1);
				T start = T(0);

				while (start < range) {
					glm::tvec2<T> n1 = base[2] - base[1];
					n1 = glm::normalize(n1);

					glm::vec<2, T> n0 = base[1] - base[0];
					n0 = glm::normalize(n0);

					if (glm::dot(n0, n1) < threshold) {
						// Subdivide
						range = (start + range) * T(0.5);
						bezier::leftSplit(base[0], base[1], base[2], T(0.5), base.begin());
						continue;
					}
					else {
						n0 = glm::vec<2, T>(-n0.y, n0.x);
						n1 = glm::vec<2, T>(-n1.y, n1.x);

						glm::vec<2, T> m = glm::normalize(n0 + n1);

						n0 = base[0] + n0 * delta;
						m = interpolate(base[0], base[1], base[2], T(0.5)) + m * delta;
						n1 = base[2] + n1 * delta;

						// Finalize the current offset segment, check if we are done.
						(*output) = n0;
						++output;

						bezier::curveThrough(n0, m, n1, output);

						(*output) = n1;
						++output;

						if (range < T(1)) {
							// Move to next segment.
							start = range;
							range = T(1);
							bezier::rightSplit(p0, p1, p2, start, base.begin());
							continue;
						}
						else {
							// Finish.
							break;
						}
					}
				} // End While
			}// End simplePixelOffsetCurve

			// Offset for cubic
			template<typename T, typename Iter>
			void simplePixelOffsetCurve(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, const glm::vec<2, T>& p3, T delta, Iter output) {
				static constexpr std::array<T, 4> interp{ T(0.0), T(1.0 / 3.0), T(2.0 / 3.0), T(1.0) };

				static constexpr T threshold = T(0.93);
				
				// offset the points first,
				// Check the middle, if its the right distance FIN
				// else subdivide and repeat.
				std::array<glm::tvec2<T>, 4> base{ p0, p1, p2, p3 };
				bezier::leftSplit(base[0], base[1], base[2], base[3], T(0.5), base.begin());

				T range = T(0.5);
				T start = T(0.0);

				while (start < range) {
					glm::tvec2<T> n1 = base[2] - base[1];
					n1 = glm::normalize(n1);

					glm::vec<2, T> n0 = base[1] - base[0];
					n0 = glm::normalize(n0);

					if (glm::dot(n0, n1) < threshold) {
						// Subdivide
						range = (start + range) * T(0.5);
						bezier::leftSplit(base[0], base[1], base[2], base[3], T(0.5), base.begin());
						continue;
					}
					else {
						n0 = glm::vec<2, T>(-n0.y, n0.x);
						n1 = glm::vec<2, T>(-n1.y, n1.x);

						glm::vec<2, T> n2 = base[3] - base[2];
						n2 = glm::vec<2, T>(-n2.y, n2.x);
						n2 = glm::normalize(n2);

						glm::vec<2, T> m0 = glm::normalize(n0 + n1);
						glm::vec<2, T> m1 = glm::normalize(n2 + n1);

						n0 = base[0] + n0 * delta;

						m0 = bezier::interpolate(base[0], base[1], base[2], base[3], interp[1])
							+ m0 * delta;

						m1 = bezier::interpolate(base[0], base[1], base[2], base[3], interp[2])
							+ m1 * delta;

						n2 = base[3] + n2 * delta;

						*output++ = n0;

						bezier::curveThrough(n0, m0, m1, n2, output);

						*output++ = n2;

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
			} // End simplePixelOffsetCurve

			// Tapered offset for cubic 
			template<typename T, typename Iter>
			void taperedPixelOffsetCurve(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, const glm::vec<2, T>& p3, const glm::vec<4, T>& taper, Iter output) {
				static constexpr std::array<T, 4> interp{ T(0.0), T(1.0 / 3.0), T(2.0 / 3.0), T(1.0) };
				
				// Since its tapered, use slightly tighter bounds
				static constexpr T threshold = T(0.97);

				// offset the points first,
				// Check the middle, if its the right distance FIN
				// else subdivide and repeat.
				std::array<T, 4> taperBase{ taper.x, taper.y, taper.z, taper.w };
				std::array<glm::tvec2<T>, 4> base{ p0, p1, p2, p3 };
				bezier::leftSplit(base[0], base[1], base[2], base[3], T(0.5), base.begin());
				bezier::leftSplit(taperBase[0], taperBase[1], taperBase[2], taperBase[3], T(0.5), taperBase.begin());

				T range = T(0.5);
				T start = T(0.0);

				while (start < range) {
					glm::tvec2<T> n1 = base[2] - base[1];
					n1 = glm::normalize(n1);

					glm::vec<2, T> n0 = base[1] - base[0];
					n0 = glm::normalize(n0);

					if (glm::dot(n0, n1) < threshold) {
						// Subdivide
						range = (start + range) * T(0.5);
						bezier::leftSplit(base[0], base[1], base[2], base[3], T(0.5), base.begin());
						bezier::leftSplit(taperBase[0], taperBase[1], taperBase[2], taperBase[3], T(0.5), taperBase.begin());
						continue;
					}
					else {
						n0 = glm::vec<2, T>(-n0.y, n0.x);
						n1 = glm::vec<2, T>(-n1.y, n1.x);

						glm::vec<2, T> n2 = base[3] - base[2];
						n2 = glm::vec<2, T>(-n2.y, n2.x);
						n2 = glm::normalize(n2);

						glm::vec<2, T> m0 = glm::normalize(n0 + n1);
						glm::vec<2, T> m1 = glm::normalize(n2 + n1);

						n0 = base[0] + n0 * interpolate(taperBase[0], taperBase[1], taperBase[2], taperBase[3], interp[0]);

						m0 = bezier::interpolate(base[0], base[1], base[2], base[3], interp[1]) 
							+ m0 * interpolate(taperBase[0], taperBase[1], taperBase[2], taperBase[3], interp[1]);

						m1 = bezier::interpolate(base[0], base[1], base[2], base[3], interp[2])
							+ m1 * interpolate(taperBase[0], taperBase[1], taperBase[2], taperBase[3], interp[2]);

						n2 = base[3] + n2 * interpolate(taperBase[0], taperBase[1], taperBase[2], taperBase[3], interp[3]);

						*output++ = n0;

						bezier::curveThrough(n0, m0, m1, n2, output);

						*output++ = n2;

						// Finalize the current offset segment, check if we are done.

						if (range < T(1)) {
							// Move to next segment.
							start = range;
							range = T(1);
							bezier::rightSplit(p0, p1, p2, p3, start, base.begin());
							bezier::rightSplit(taper[0], taper[1], taper[2], taper[3], start, taperBase.begin());
							continue;
						}
						else {
							// Finish.
							break;
						}
					}
				} // End While
			} // End taperedPixelOffsetCurve

		}; // End namespace intern

		// Offset a quadratic bezier curve, assuming that all coordinates are in pixel scale (essentially integer precision).
		template<typename T, typename Iter>
		void pixelOffset(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, T delta, Iter output) {
			static_assert(is_output_iterator_v<Iter>, "The iterator passed in is not a proper output iterator!");
			using vec_t = glm::vec<2, T>;
			static_assert(is_iterator_writable_v<Iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

			intern::simplePixelOffsetCurve(p0, p1, p2, -delta, output);
		}

		// Offset a cubic bezier curve, assuming that all coordinates are in pixel scale (essentially integer precision).
		template<typename T, typename Iter>
		void pixelOffset(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, const glm::vec<2, T>& p3, T delta, Iter output) {
			static_assert(is_output_iterator_v<Iter>, "The iterator passed in is not a proper output iterator!");
			using vec_t = glm::vec<2, T>;
			static_assert(is_iterator_writable_v<Iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

			intern::simplePixelOffsetCurve(p0, p1, p2, p3, -delta, output);
		}
		
		template<typename T, typename Iter>
		void taperedPixelOffset(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, const glm::vec<2, T>& p3, glm::vec<4, T> taper, Iter output) {
			static_assert(is_output_iterator_v<Iter>, "The iterator passed in is not a proper output iterator!");
			using vec_t = glm::vec<2, T>;
			static_assert(is_iterator_writable_v<Iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

			intern::taperedPixelOffsetCurve(p0, p1, p2, p3, -taper, output);
		}

	}; // End namespace bezier
}; // End namespace ez
