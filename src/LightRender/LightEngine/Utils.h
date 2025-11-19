#pragma once

#include <Maths/Vector3.h>

namespace Utils
{
	bool Normalize(gce::Vector3f32& vector);
	float GetDistance(int x1, int y1, int z1, int x2, int y2, int z3);
	float GetAngleDegree(const gce::Vector3f32& v1, const gce::Vector3f32& v2);
}