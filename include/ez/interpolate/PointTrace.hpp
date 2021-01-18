#pragma once
#include <cassert>
#include <vector>
#include <cinttypes>
#include <algorithm>
#include "intern/PointType.hpp"


namespace ez {
	template<typename T, int N>
	class PointTrace {
	public:
		using index_t = typename std::ptrdiff_t;

		using real_t = T;
		using vec_t = typename intern::Point<T, N>::type;

		struct Knot {
			real_t position;
			vec_t tangent;
		};

		struct Point {
			Point()
				: value(0)
				, knot(pos, tang)
			{}
			Point(real_t pos, vec_t tang, vec_t val)
				: value(val)
				, knot{ pos, tang }
			{}

			Knot knot;
			vec_t value;

			bool operator<(const Point & other) const {
				return knot.position < other.knot.position;
			}
			bool operator<=(const Point& other) const {
				return knot.position <= other.knot.position;
			}
			bool operator>(const Point& other) const {
				return knot.position > other.knot.position;
			}
			bool operator>=(const Point& other) const {
				return knot.position >= other.knot.position;
			}
		};

		using container_t = std::vector<Point>;
		using iterator = typename container_t::iterator;
		using const_iterator = typename container_t::const_iterator;


		PointTrace() = default;
		PointTrace(const PointTrace&) = default;
		PointTrace(PointTrace&&) noexcept = default;
		PointTrace& operator=(const PointTrace&) = default;
		PointTrace& operator=(PointTrace&&) noexcept = default;

		PointTrace(index_t pointCount)
			: points(pointCount)
		{}

		void sort() {
			std::sort(points.begin(), points.end());
		}

		Point& insert(Point p) {
			iterator it = std::lower_bound(points.begin(), points.end(), p);
			it = points.insert(it, p);
			return *it;
		}

		iterator nearestPoint(real_t position) {
			if (numPoints() == 0) {
				return end();
			}

			iterator found = std::lower_bound(points.begin(), points.end(), position);

			if ((found +1) == end()) {
				return found;
			}

			if ((position - found->knot.position) < ((found+1)->knot.position - position)) {
				return found;
			}
			else {
				return found + 1;
			}
		}

		const vec_t& operator[](index_t index) const {
			assert(index >= 0 && index < numPoints());
			return points[index].value;
		}
		vec_t& operator[](index_t index) {
			assert(index >= 0 && index < numPoints());
			return points[index].value;
		}

		vec_t eval(T t) const {
			if (numPoints() == 0) {
				return vec_t(0);
			}

			// First element greater or equal.
			iterator found = std::upper_bound(points.begin(), points.end(), t);
			
			if ((found + 1) >= end()) {
				vec_t p0 = points.back().value;
				vec_t tangent = points.back().knot.tangent;

				t = t - points.back().knot.position;
				return p0 + tangent * t;
			}
			else if(found == begin()) {
				vec_t p0 = points.front().value;
				vec_t tangent = points.front().knot.tangent;

				t = t - points.front().knot.position;
				return p0 + tangent * t;
			}
			else {
				static constexpr real_t i0 = real_t(1) / real_t(3), i1 = real_t(2) / real_t(3);

				const Point& lower = *(found - 1);
				const Point& upper = *(found);

				t = (t - lower.knot.position) / (upper.knot.position - lower.knot.position);

				vec_t p1 = lower.value + lower.knot.tangent * i0;
				vec_t p2 = upper.value - upper.knot.tangent * i0;

				real_t t1 = real_t(1) - t;
				real_t tt = t * t;
				real_t t1t1 = t1 * t1;

				return lower.value * t1t1 * t1 + p1 * real_t(3) * t1t1 * t + p2 * real_t(3) * tt * t1 + upper.value * tt * t;
			}
		}

		index_t numPoints() const {
			return static_cast<index_t>(points.size());
		}
		index_t size() const {
			return static_cast<index_t>(points.size());
		}

		void clear() {
			points.clear();
		}

		iterator begin() {
			return points.begin();
		}
		iterator end() {
			return points.end();
		}

		const_iterator begin() const {
			return points.begin();
		}
		const_iterator end() const {
			return points.end();
		}
	private:
		// The interpolation points.
		container_t points;
	};

	template<typename T, int N>
	bool operator<(const typename PointTrace<T,N>::Point& point, float other) {
		return point.knot.position < other;
	}
	template<typename T, int N>
	static bool operator<=(const typename PointTrace<T, N>::Point& point, float other) {
		return point.knot.position <= other;
	}
	template<typename T, int N>
	static bool operator>(const typename PointTrace<T, N>::Point& point, float other) {
		return point.knot.position > other;
	}
	template<typename T, int N>
	static bool operator>=(const typename PointTrace<T, N>::Point& point, float other) {
		return point.knot.position >= other;
	}

	template<typename T, int N>
	static bool operator<(float other, const typename PointTrace<T, N>::Point& point) {
		return other < point.knot.position;
	}
	template<typename T, int N>
	static bool operator<=(float other, const typename PointTrace<T, N>::Point& point) {
		return other <= point.knot.position;
	}
	template<typename T, int N>
	static bool operator>(float other, const typename PointTrace<T, N>::Point& point) {
		return other > point.knot.position;
	}
	template<typename T, int N>
	static bool operator>=(float other, const typename PointTrace<T, N>::Point& point) {
		return other >= point.knot.position;
	}
};