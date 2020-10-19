#include "geom.hpp"

constexpr float D2R(float degree) {
	return degree * PI / 180.f;
}

constexpr float R2D(float radian) {
	return radian * 180.f / PI;
}

// v = { x, y, z }
// result = { r, theta, phi) = { radius, yaw, pitch }
vec vectorAngle(const vec& v) { // specials values for assaultcube here - not the default formula
	vec result;
	result.x = v.magnitude();
	// normal formula would be without adding (PI/2), but AssaultCube stores it's Pitch from 0 to 360 instead of
	// -180 to 180
	result.y = R2D((PI / 2) + atan2f(v.y, v.x));
	result.z = R2D(asinf(v.z / result.x)); // == RadianToDegree(-(acosf(vec.z / result.z) - (PI / 2))) which is harder to compute
	return result;
}

// v = { radius, phi, theta } = { radius, yaw, pitch }
// result = { x, y, z }
vec angleVector(const vec& v) {
	vec r;
	float sp = sinf(D2R(v.z + 90.f));
	r.x = v.x * sp * sinf(D2R(v.y));
	r.y = v.x * -sp * cosf(D2R(v.y));
	r.z = v.x * sinf(D2R(v.z));
	return r;
}