#pragma once

#include <Maths/Vector3.h>

namespace Utils
{
	bool Normalize(gce::Vector3f32& vector);
	float GetDistance(gce::Vector3f32 const& vector1, gce::Vector3f32 const& vector2);
	float GetDistance(float x1, float y1, float z1, float x2, float y2, float z3);
	float GetAngleDegree(const gce::Vector3f32& v1, const gce::Vector3f32& v2);
}