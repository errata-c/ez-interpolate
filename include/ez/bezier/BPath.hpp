#pragma once
#include <algorithm>
#include <ez/meta.hpp>
#include <cinttypes>
#include <cassert>
#include <vector>
#include <array>
#include <glm/vec2.hpp>

namespace ez {
	template<typename vec_t>
	class BPath {
	public:
		using value_type = vec_t;
		using real_t = ez::vec_value_t<vec_t>;

		static_assert(std::is_floating_point_v<real_t>, "ez::BPath requires floating point type!");

		using Point = value_type;

		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		using reference = value_type&;
		using pointer = value_type*;
		using const_reference = const value_type&;
		using const_pointer = const value_type*;

		using Container = std::vector<Point>;

		using iterator = typename Container::iterator;
		using const_iterator = typename Container::const_iterator;
		using reverse_iterator = typename Container::reverse_iterator;
		using const_reverse_iterator = typename Container::const_reverse_iterator;

		class segment_generator;

		using Segment = std::array<Point, 4>;
		struct Index {
			size_type index;
			real_t delta;
		};

		BPath()
			: open(true)
		{}

		BPath(size_type n, const_reference point, bool _open = true)
			: open(_open)
			, points(n, point)
		{}

		template<typename Iter>
		BPath(Iter first, Iter last, bool _open = true)
			: open(_open)
			, points(first, last)
		{
			static_assert(ez::is_input_iterator_v<Iter>, "ez::BPath::BPath requires an input iterator!");
		}

		BPath(const BPath& other)
			: open(other.open)
			, points(other.points)
		{}

		BPath(BPath&& other) noexcept
			: open(other.open)
			, points(std::move(other.points))
		{}

		BPath& operator=(const BPath& other) {
			points = other.points;
			open = other.open;

			return *this;
		}

		BPath& operator=(BPath&& other) noexcept {
			points = std::move(other.points);
			open = other.open;

			return *this;
		}

		// Return the segment at the interpolation value.
		Index indexAt(real_t t) const {
			t = std::min(static_cast<real_t>(1), std::max(t, static_cast<real_t>(0)));
			Index index;

			index.index = numPoints();
			if (isClosed()) {
				index.index -= N1;
			}

			index.delta = static_cast<real_t>(index.index)* t;
			t = std::floor(index.delta);
			index.delta = index.delta - t;
			index.index = static_cast<size_type>(t);

			return index;
		}

		Segment segmentAt(size_type i) const {
			static constexpr real_t lower(8 / 12.0);
			static constexpr real_t upper = real_t(1) - lower;

			static constexpr real_t b = real_t{ 9 } / real_t{ 12 };
			static constexpr real_t a = (1.0 - b) / 2.0;

			assert(i < numSegments());
			Segment seg;

			vec_t p0, p1, p2;

			if (isClosed()) {
				size_type
					i1 = pointIndex(i + 1),
					i2 = pointIndex(i + 2),
					i3 = pointIndex(i + 3),
					i4 = pointIndex(i + 4);

				p0 = points[i ] * a + points[i1] * b + points[i2] * a;
				p1 = points[i1] * a + points[i2] * b + points[i3] * a;
				p2 = points[i2] * a + points[i3] * b + points[i4] * a;
			}
			else {
				if (i == 0) {
					// first segment
					p0 = points[0];
					p1 = points[0]*a + points[1]*b + points[2]*a;
					p2 = points[1]*a + points[2]*b + points[3]*a;
				}
				else if (i == (numPoints() - 3)) {
					// last segment
					p0 = points[points.size() - 4]*a + points[points.size() - 3]*b + points[points.size() - 2]*a;
					p1 = points[points.size() - 3]*a + points[points.size() - 2]*b + points[points.size() - 1]*a;
					p2 = points[points.size() - 1];
				}
				else {
					// middle segment
					i -= 1;
					size_type
						i1 = (i + 1),
						i2 = (i + 2),
						i3 = (i + 3),
						i4 = (i + 4);

					p0 = points[i ] * a + points[i1] * b + points[i2] * a;
					p1 = points[i1] * a + points[i2] * b + points[i3] * a;
					p2 = points[i2] * a + points[i3] * b + points[i4] * a;
				}
			}

			seg[0] = ez::bezier::interpolate(p0, p1, real_t{ 0.5 });
			seg[3] = ez::bezier::interpolate(p1, p2, real_t{ 0.5 });

			seg[1] = ez::bezier::interpolate(seg[0], p1, lower);
			seg[2] = ez::bezier::interpolate(p1, seg[3], upper);

			return seg;
		}

		size_type numSegments() const {
			if (points.size() > 3) {
				if (isOpen()) {
					return points.size() - size_type(2);
				}
				else {
					return points.size();
				}
			}
			else {
				return 0;
			}
		}

		real_t length() const {
			real_t total = real_t(0);

			size_type count = numSegments();
			for (size_type i = 0; i < count; ++i) {
				Segment seg = segmentAt(i);
				total += seg.length();
			}

			return total;
		}

		Point evalAt(Index index) const {
			Segment seg = segmentAt(index.index);
			return seg.evalAt(index.delta);
		}

		Point evalAt(real_t t) const {
			return evalAt(indexAt(t));
		}

		reference operator[](size_type i) {
			assert(i < points.size());
			return points[i];
		}
		const_reference operator[](size_type i) const {
			assert(i < points.size());
			return points[i];
		}
		reference at(size_type i) {
			return points.at(i);
		}
		const_reference at(size_type i) const {
			return points.at(i);
		}

		pointer data() {
			return points.data();
		}
		const_pointer data() const {
			return points.data();
		}

		constexpr bool empty() const {
			return false;
		}

		template<typename Iter>
		void assign(Iter first, Iter last) {
			static_assert(ez::is_input_iterator_v<Iter>, "ez::BPath::assign requires an input iterator!");

			points.assign<Iter>(first, last);
		}

		void assign(size_type n, const_reference point) {
			points.assign(n, point);
		}

		void assign(std::initializer_list<Point> il) {
			points.assign(il);
		}

		void insert(const_iterator it, const_reference point) {
			points.insert(it, point);
		}
		void insert(const_iterator it, size_type n, const_reference point) {
			points.insert(it, n, point);
		}
		template<typename Iter>
		void insert(const_iterator it, Iter first, Iter last) {
			static_assert(ez::is_input_iterator_v<Iter>, "ez::BPath::insert requires an input iterator!");

			points.insert<Iter>(it, first, last);
		}
		void insert(const_iterator it, std::initializer_list<Point> il) {
			points.insert(it, il);
		}

		void erase(const_iterator it) {
			points.erase(it);
		}
		void erase(const_iterator first, const_iterator last) {
			points.erase(first, last);
		}

		void resize(size_type n) {
			points.resize(n);
		}
		void resize(size_type n, const_reference point) {
			points.resize(n, point);
		}

		size_type size() const {
			return points.size();
		}
		size_type numPoints() const {
			return points.size();
		}
		size_type max_size() const {
			return points.max_size();
		}

		reference front() {
			return points.front();
		}
		const_reference front() const {
			return points.front();
		}

		reference back() {
			return points.back();
		}
		const_reference back() const {
			return points.back();
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

		const_iterator cbegin() const {
			return points.cbegin();
		}
		const_iterator cend() const {
			return points.cend();
		}

		reverse_iterator rbegin() {
			return points.rbegin();
		}
		reverse_iterator rend() {
			return points.rend();
		}

		const_reverse_iterator crbegin() const {
			return points.crbegin();
		}
		const_reverse_iterator crend() const {
			return points.crend();
		}

		void swap(BPath& other) noexcept {
			points.swap(other.points);
			std::swap(open, other.open);
		}

		void clear() {
			open = false;
			points.clear();
		}

		void append(const_reference point) {
			points.push_back(point);
		}
		void push_back(const_reference point) {
			points.push_back(point);
		}
		void pop_back() {
			points.pop_back();
		}

		bool isOpen() const {
			return open;
		}
		bool isClosed() const {
			return !open;
		}
		void setOpen(bool value) {
			open = value;
		}
		void setClosed(bool value) {
			open = !value;
		}

		BPath clone() const {
			return BPath{ *this };
		}

	private:
		bool open;
		Container points;
		
		size_type pointIndex(size_type i) const noexcept {
			if (i >= numPoints()) {
				i -= numPoints();
			}
			return i;
		}
	};
};