#pragma once
#include <ez/meta.hpp>
#include "BezierDerivatives.hpp"
#include "BezierInterpolation.hpp"
#include "BezierFitting.hpp"
#include "BezierUtil.hpp"

// The simple offset functions don't push the first point, to allow for chaining multiple calls together

namespace ez::bezier {
	namespace intern {
		// Offset for quadratic or cubic curves
		// 'simple' means it does no checks for degenerate curves or other exceptional circumstances.
		template<std::size_t N, typename T, typename output_iter>
		std::ptrdiff_t simplePixelOffset(const std::array<glm::tvec2<T>, N> & points, T delta, T threshold, output_iter output) {
			using vec_t = glm::tvec2<T>;

			// The number of points written.
			std::ptrdiff_t count = 0;

			// To support negative offsets
			T compare = std::abs(delta);
			std::array<vec_t, N> base = points, n, offset;

			T range = T(1);
			T start = T(0);



			while (start < range) {
				for (int i = 0; i < N; ++i) {
					n[i] = bezier::normalAtStatic<N>(base.begin(), T(i) / T(N-1));
					offset[i] = base[i] + n[i] * delta;
				}

				vec_t bp = bezier::interpolateStatic<N>(base.begin(), T(0.5));
				vec_t op = bezier::interpolateStatic<N>(offset.begin(), T(0.5));

				if (std::abs(glm::length(op - bp) - compare) > threshold) {
					// Subdivide
					range = (start + range) * T(0.5);
					bezier::leftSplitStatic<N>(base.begin(), T(0.5), base.begin());
					continue;
				}
				else {
					// Finalize the current offset segment, check if we are done.

					// Write the next three controls
					for (int i = 1; i < N; ++i) {
						*output++ = offset[i];
					}
					count += N-1;

					if (range < T(1)) {
						// Move to next segment.
						start = range;
						range = T(1);
						bezier::rightSplitStatic<N>(points.begin(), start, base.begin());
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

		// Tapered offset for cubic 
		template<std::size_t N, std::size_t M, typename T, typename output_iter>
		std::ptrdiff_t simpleTaperedPixelOffset(const std::array<glm::tvec2<T>, N> & points, const std::array<T, M>& tapers, T threshold, output_iter output) {
			using vec_t = glm::tvec2<T>;

			// The number of points written.
			std::ptrdiff_t count = 0;

			std::array<vec_t, N> base = points, n, offset;
			std::array<T, M> tbase = tapers;

			T range = T(1);
			T start = T(0);

			while (start < range) {
				for (int i = 0; i < N; ++i) {
					T interp = T(i) / T(N - 1);
					n[i] = bezier::normalAtStatic<N>(base.begin(), interp);
					offset[i] = base[i] + n[i] * bezier::interpolateStatic<M>(tbase.begin(), interp);
				}

				std::array<vec_t, 2> bp{ {
					bezier::interpolateStatic<N>(base.begin(), T(1) / T(3)),
					bezier::interpolateStatic<N>(base.begin(), T(2) / T(3))
				} };
				std::array<vec_t, 2> op{ {
					bezier::interpolateStatic<N>(offset.begin(), T(1) / T(3)),
					bezier::interpolateStatic<N>(offset.begin(), T(2) / T(3))
				} };
				std::array<T, 2> cmp{ {
					std::abs(bezier::interpolateStatic<M>(tbase.begin(), T(1) / T(3))),
					std::abs(bezier::interpolateStatic<M>(tbase.begin(), T(2) / T(3)))
				} };

				if (std::abs(glm::length(op[0] - bp[0]) - cmp[0]) > threshold &&
					std::abs(glm::length(op[1] - bp[1]) - cmp[1]) > threshold) {
					// Subdivide
					range = (start + range) * T(0.5);
					bezier::leftSplitStatic<N>(base.begin(), T(0.5), base.begin());
					bezier::leftSplitStatic<M>(tbase.begin(), T(0.5), tbase.begin());
					continue;
				}
				else {
					// Finalize the current offset segment, check if we are done.
					for (int i = 1; i < N; ++i) {
						*output++ = offset[i];
					}
					count += N - 1;

					if (range < T(1)) {
						// Move to next segment.
						start = range;
						range = T(1);
						bezier::rightSplitStatic<N>(points.begin(), start, base.begin());
						bezier::rightSplitStatic<M>(tapers.begin(), start, tbase.begin());
						continue;
					}
					else {
						// Finish.
						break;
					}
				}
			} // End While

			return count;
		}; // End taperedPixelOffsetCurve

	}; // End namespace intern

	// Offset a quadratic bezier curve, assuming that all coordinates are in pixel scale (essentially integer precision).
	template<typename T, typename output_iter>
	std::ptrdiff_t pixelOffset(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, T delta, output_iter output) {
		static_assert(std::is_floating_point_v<T>, "ez::bezier::pixelOffset requires floating point value types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "ez::bezier::pixelOffset requires an output iterator as its last argument!");
		using vec_t = glm::vec<2, T>;
		static_assert(ez::is_iterator_writable_v<output_iter, vec_t>, "ez::bezier::pixelOffset requires the output iterator to accept vec2 values!");

		std::array<glm::tvec2<T>, 3> points{ p0, p1, p2 };

		{ // Write the first point of the offset
			vec_t startNorm = bezier::normalAtStatic<3>(points.begin(), T(0));
			vec_t startPoint = points[0] + startNorm * delta;
			*output++ = startPoint;
		}

		return intern::simplePixelOffset(points, delta, T(0.5), output) + 1;
	}

	// Offset a cubic bezier curve, assuming that all coordinates are in pixel scale (essentially integer precision).
	template<typename T, typename output_iter>
	std::ptrdiff_t pixelOffset(const glm::tvec2<T>& p0, const glm::tvec2<T>& p1, const glm::tvec2<T>& p2, const glm::tvec2<T>& p3, T delta, output_iter output) {
		static_assert(std::is_floating_point_v<T>, "ez::bezier::pixelOffset requires floating point value types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "ez::bezier::pixelOffset requires an output iterator as its last argument!");
		using vec_t = glm::vec<2, T>;
		static_assert(ez::is_iterator_writable_v<output_iter, vec_t>, "ez::bezier::pixelOffset requires the output iterator to accept vec2 values!");

		std::array<glm::tvec2<T>, 4> points{p0, p1, p2, p3};

		{ // Write the first point of the offset
			vec_t startNorm = bezier::normalAtStatic<4>(points.begin(), T(0));
			vec_t startPoint = points[0] + startNorm * delta;
			*output++ = startPoint;
		}

		return intern::simplePixelOffset(points, delta, T(0.5), output) + 1;
	}
	
	// Offset a quadratic bezier curve, using taper values
	template<typename T, typename output_iter>
	std::ptrdiff_t taperedPixelOffset(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, const std::array<T, 3>& taper, output_iter output) {
		static_assert(std::is_floating_point_v<T>, "ez::bezier::taperedPixelOffset requires floating point value types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "ez::bezier::taperedPixelOffset requires an output iterator as its last argument!");
		using vec_t = glm::vec<2, T>;
		static_assert(ez::is_iterator_writable_v<output_iter, vec_t>, "ez::bezier::taperedPixelOffset requires the output iterator to accept vec2 values!");

		std::array<glm::tvec2<T>, 3> points{ p0, p1, p2 };

		{ // Write the first point of the offset
			vec_t startNorm = bezier::normalAtStatic<3>(points.begin(), T(0));
			vec_t startPoint = points[0] + startNorm * taper[0];
			*output++ = startPoint;
		}

		return intern::simpleTaperedPixelOffset(points, taper, T(0.5), output) + 1;
	}

	// Offset a cubic bezier curve, using taper values
	template<typename T, typename output_iter>
	std::ptrdiff_t taperedPixelOffset(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, const glm::vec<2, T>& p3, const std::array<T, 4>& taper, output_iter output) {
		static_assert(std::is_floating_point_v<T>, "ez::bezier::taperedPixelOffset requires floating point value types!");
		static_assert(ez::is_output_iterator_v<output_iter>, "ez::bezier::taperedPixelOffset requires an output iterator as its last argument!");
		using vec_t = glm::vec<2, T>;
		static_assert(ez::is_iterator_writable_v<output_iter, vec_t>, "ez::bezier::taperedPixelOffset requires the output iterator to accept vec2 values!");

		std::array<glm::tvec2<T>, 4> points{ p0, p1, p2, p3 };

		std::ptrdiff_t count = 0;
		{ // Write the first point of the offset
			vec_t startNorm = bezier::normalAtStatic<4>(points.begin(), T(0));
			vec_t startPoint = points[0] + startNorm * taper[0];
			*output++ = startPoint;
			++count;
		}

		// find the extrema, split at those points, run taper offset for each segment
		int numroots = 0;
		std::array<T, 6> roots;
		numroots = bezier::findExtrema(p0, p1, p2, p3, &roots[0]);
		roots[numroots] = T(0);
		++numroots;
		roots[numroots] = T(1);
		++numroots;

		// Selection sort
		for (int i = 0; i < numroots-1; ++i) {
			int selMin = i;
			for (int j = i + 1; j < numroots; ++j) {
				if (roots[j] < roots[selMin]) {
					selMin = j;
				}
			}
			if (selMin != i) {
				std::swap(roots[i], roots[selMin]);
			}
		}
		// Merge invalid ranges
		{
			int j = 0;
			for (int i = 1; i < numroots; ++i) {
				if (std::abs(roots[i] - roots[j]) > ez::epsilon<T>() * T(10)) {
					++j;
				}
				roots[j] = roots[i];
			}
			numroots = j + 1;
		}

		// Split each subrange defined by the roots array
		for (int i = 0; i < numroots - 1; ++i) {
			std::array<vec_t, 4> seg;
			std::array<T, 4> tap;
			bezier::segmentStatic<4>(points.data(), roots[i], roots[i + 1], &seg[0]);
			bezier::segmentStatic<4>(taper.data(), roots[i], roots[i + 1], &tap[0]);

			count += intern::simpleTaperedPixelOffset(seg, tap, T(0.5), output);
		}

		return count;
	}

}; // End namespace ez
