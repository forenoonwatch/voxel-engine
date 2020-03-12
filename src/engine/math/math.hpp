#pragma once

#include "engine/core/common.hpp"

#include <cstdlib>
#include <cmath>

#define MATH_PI 3.1415926535897932f
#define MATH_TWO_PI 6.28318530717959f
#define MATH_HALF_PI 1.57079632679f
#define MATH_R_PI 0.31830988618f
#define MATH_R_TWO_PI 0.159154943091895f
#define MATH_R_HALF_PI 0.636619772367581f

#define MATH_E 2.71828182845904523536f
#define MATH_R_LN_2 1.44269504088896f
#define MATH_RAD_TO_DEG_CONV 57.2957795130823f
#define MATH_DEG_TO_RAD_CONV 0.0174532925199433f

namespace Math {
	FORCEINLINE float exp(float val) { return expf(val); }
	FORCEINLINE float ln(float val) { return logf(val); }
	FORCEINLINE float logx(float base, float val) { return ln(val) / ln(base); }
	FORCEINLINE float log2(float val) { return ln(val) * MATH_R_LN_2; }

	FORCEINLINE float fmod(float num, float den) { return fmodf(num, den); }
	FORCEINLINE float sin(float val) { return sinf(val); }
	FORCEINLINE float asin(float val) { return asinf(val); }
	FORCEINLINE float cos(float val) { return cosf(val); }
	FORCEINLINE float acos(float val) { return acosf(val); }
	FORCEINLINE float tan(float val) { return tanf(val); }
	FORCEINLINE float atan(float val) { return atanf(val); }
	FORCEINLINE float atan(float val, float val2) { return atan2f(val, val2); }
	FORCEINLINE float pow(float val, float power) { return powf(val, power); }
	FORCEINLINE float sqrt(float val) { return sqrtf(val); }

	FORCEINLINE int32 rand() { return std::rand(); }
	FORCEINLINE void seedRand(int32 seed) { srand((uint32)seed); }
	FORCEINLINE float randf() { return std::rand() / (float)RAND_MAX; }
	FORCEINLINE float randf(float min, float max) { return min + (max - min) * randf(); }

	FORCEINLINE uint32 floorLog2(uint32 val) {
		return std::ilogb(val);
	}

	template<typename T>
	constexpr FORCEINLINE T abs(const T& val) {
		return val >= (T)(0) ? val : -val;
	}

	template<typename T>
	constexpr FORCEINLINE T min(const T& val1, const T& val2) {
		return val1 <= val2 ? val1 : val2;
	}

	template<typename T>
	constexpr FORCEINLINE T max(const T& val1, const T& val2) {
		return val1 >= val2 ? val1 : val2;
	}

	template<typename T>
	FORCEINLINE T toDegrees(const T& val) {
		return val * MATH_RAD_TO_DEG_CONV;
	}

	template<typename T>
	FORCEINLINE T toRadians(const T& val) {
		return val * MATH_DEG_TO_RAD_CONV;
	}

	template<typename T>
	FORCEINLINE T clamp(const T& val, const T& min, const T& max) {
		if (val > max) {
			return max;
		}
		else if (val > min) {
			return val;
		}
		else {
			return min;
		}
	}
};

template <>
FORCEINLINE float Math::abs(const float& val) {
	return fabsf(val);
}
