#pragma once
#include <ez/meta.hpp>
#include "BezierDerivatives.hpp"
#include "BezierInterpolation.hpp"
#include "BezierFitting.hpp"
#include "BezierUtil.hpp"

namespace ez::bezier {
	namespace intern {
		// Offset for quadratic or cubic curves
		// 'simple' means it does no checks for degenerate curves or other exceptional circumstances.
		template<std::size_t N, typename T, typename output_iter>
		std::ptrdiff_t simplePixelOffset(const std::array<glm::tvec2<T>, N> & points, T delta, output_iter output) {
			static constexpr T threshold = T(0.921);
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
					for (int i = 0; i < N; ++i) {
						*output++ = offset[i];
					}
					count += N;

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
		std::ptrdiff_t simpleTaperedPixelOffset(const std::array<glm::tvec2<T>, N> & points, const std::array<T, M>& tapers, output_iter output) {
			static constexpr T threshold = T(0.921);
			using vec_t = glm::tvec2<T>;

			// The number of points written.
			std::ptrdiff_t count = 0;

			// To support negative offsets
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

				vec_t bp = bezier::interpolateStatic<N>(base.begin(), T(0.5));
				vec_t op = bezier::interpolateStatic<N>(offset.begin(), T(0.5));
				T compare = std::abs(bezier::interpolateStatic<M>(tbase.begin(), T(0.5)));

				if (std::abs(glm::length(op - bp) - compare) > threshold) {
					// Subdivide
					range = (start + range) * T(0.5);
					bezier::leftSplitStatic<N>(base.begin(), T(0.5), base.begin());
					bezier::leftSplitStatic<M>(tbase.begin(), T(0.5), tbase.begin());
					continue;
				}
				else {
					// Finalize the current offset segment, check if we are done.
					for (int i = 0; i < N; ++i) {
						*output++ = offset[i];
					}
					count += N;

					if (range < T(1)) {
						// Move to next segment.
						start = range;
						range = T(1);
						bezier::rightSplitStatic<N>(points.begin(), start, base.begin());
						bezier::rightSplitStatic<M>(tbase.begin(), start, tbase.begin());
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
	template<typename T, typename Iter>
	std::ptrdiff_t pixelOffset(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, T delta, Iter output) {
		static_assert(ez::is_output_iterator_v<Iter>, "The iterator passed in is not a proper output iterator!");
		using vec_t = glm::vec<2, T>;
		static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

		std::array<glm::tvec2<T>, 3> points{ p0, p1, p2 };
		return intern::simplePixelOffset(points, -delta, output);
	}

	// Offset a cubic bezier curve, assuming that all coordinates are in pixel scale (essentially integer precision).
	template<typename T, typename Iter>
	std::ptrdiff_t pixelOffset(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, const glm::vec<2, T>& p3, T delta, Iter output) {
		static_assert(ez::is_output_iterator_v<Iter>, "The iterator passed in is not a proper output iterator!");
		using vec_t = glm::vec<2, T>;
		static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

		std::array<glm::tvec2<T>, 4> points{p0, p1, p2, p3};
		return intern::simplePixelOffset(points, -delta, output);
	}
	
	// Offset a cubic bezier curve, using taper values
	template<typename T, typename Iter>
	std::ptrdiff_t taperedPixelOffset(const glm::vec<2, T>& p0, const glm::vec<2, T>& p1, const glm::vec<2, T>& p2, const glm::vec<2, T>& p3, glm::vec<4, T> taper, Iter output) {
		static_assert(ez::is_output_iterator_v<Iter>, "The iterator passed in is not a proper output iterator!");
		using vec_t = glm::vec<2, T>;
		static_assert(ez::is_iterator_writable_v<Iter, vec_t>, "Cannot convert from vector type to iterator value_type!");

		std::array<glm::tvec2<T>, 4> points{ p0, p1, p2, p3 };
		std::array<T, 4> tapers{-taper[0], -taper[1], -taper[2], -taper[3]};
		return intern::simpleTaperedPixelOffset(points, tapers, output);
	}

}; // End namespace ez
