#ifndef WINDOWS_WINDOW_VECTOR2_HPP
	#define WINDOWS_WINDOW_VECTOR2_HPP

	#include "util.hpp"

	namespace winLib {

		template <class T>
		class vector2 {
		public:
			T x, y;
			vector2() : x(0), y(0) {}
			vector2(T x, T y) : x(x), y(y) {}
			vector2(const vector2& other) : x(other.x), y(other.y) {}

			T dist() const { return std::sqrt(x * x + y * y); }
			T dist2() const { return x * x + y * y; }

			vector2 norm() const { T r = 1 / dist(); return vector2(x * r, y * r); }
			vector2 perp() const { return vector2(-y, x); }
			vector2 floor() const { return vector2(std::floor(x), std::floor(y)); }
			vector2 ceil() const { return vector2(std::ceil(x), std::ceil(y)); }
			vector2 maximum(const vector2& other) const { return vector2(x > other.x ? x : other.x, y > other.y ? y : other.y); }
			vector2 minimum(const vector2& other) const { return vector2(x < other.x ? x : other.x, y < other.y ? y : other.y); }
			vector2 cart() const { return { std::cos(y) * x, std::sin(y) * x }; }
			vector2 polar() const { return { dist(), std::atan2(y, x) }; }
			vector2 clamp(const vector2& max_vec, const vector2& min_vec) const { return this->maximum(max_vec).minimum(min_vec); }
			vector2 lerp(const vector2& other, const double t) const { return this->operator*(T(1.0 - t)) + (other * T(t)); }

			T dot(const vector2& other) const { return this->x * other.x + this->y * other.y; }
			T cross(const vector2& other) const { return this->x * other.x - this->y * other.y; }

			const std::string str() const { return std::string("vector2{ x: ") + std::to_string(x) + ", y:" + std::to_string(y) + "}"; }
			friend std::ostream& operator<<(std::ostream& output, const vector2& vector) { output << vector.str(); return output; }

			template <class C>
			operator vector2<C>() const { return { static_cast<C>(x), static_cast<C>(y) }; }

			vector2& operator= (const vector2& other) = default;

			vector2 operator+ (const vector2& other)	const { return vector2(x + other.x, y + other.y); }
			vector2 operator- (const vector2& other)	const { return vector2(x - other.x, y - other.y); }
			vector2 operator* (const vector2& other)	const { return vector2(x * other.x, y * other.y); }
			vector2 operator/ (const vector2& other)	const { return vector2(x / other.x, y / other.y); }

			vector2 operator+=(const vector2& other)	const { this->x += other.x; this->y += other.y; return *this; }
			vector2 operator-=(const vector2& other)	const { this->x -= other.x; this->y -= other.y; return *this; }
			vector2 operator*=(const vector2& other)	const { this->x *= other.x; this->y *= other.y; return *this; }
			vector2 operator/=(const vector2& other)	const { this->x /= other.x; this->y /= other.y; return *this; }

			vector2 operator* (const T& t)				const { return vector2(x * t, y * t); }
			vector2 operator/ (const T& t)				const { return vector2(x / t, y / t); }

			vector2 operator*=(const T& t)				const { this->x *= t; this->y *= t; return *this; }
			vector2 operator/=(const T& t)				const { this->x /= t; this->y /= t; return *this; }

			vector2 operator+ ()						const { return vector2(+x, +y); }
			vector2 operator- ()						const { return vector2(-x, -y); }

			bool operator==(const vector2& other)		const { return x == other.x && y == other.y; }
			bool operator!=(const vector2& other)		const { return x != other.x && y != other.y; }
		};

		// usings for every useful data type
		using vector2b = vector2<unsigned char>;
		using vector2s = vector2<short>;
		using vector2i = vector2<int>;
		using vector2u = vector2<unsigned int>;
		using vector2l = vector2<long long int>;
		using vector2f = vector2<float>;
		using vector2d = vector2<double>;
	}

#endif