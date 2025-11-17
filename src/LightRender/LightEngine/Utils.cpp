//#include <SFML/System/Vector2.hpp>

#include <cmath>
#include <Maths/Vector3.h>

namespace Utils 
{
    bool Normalize(gce::Vector3f32& vector)
    {
        float magnitude = std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

		if (magnitude != 0)
		{
			vector.x /= magnitude;
			vector.y /= magnitude;
			vector.z /= magnitude;
		
			return true;
		}

		return false;
    }

	float GetDistance(int x1, int y1, int x2, int y2, int z1, int z2)
	{
		int x = x2 - x1;
		int y = y2 - y1;
		int z = z2 - z1;

		return std::sqrt(x * x + y * y + z * z);
	}

	float GetAngleDegree(const gce::Vector3f32& v1, const gce::Vector3f32& v2)
	{
		float dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

		float mag1 = std::sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
		float mag2 = std::sqrt(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z);

		// Évite les divisions par 0
		if (mag1 == 0.0f || mag2 == 0.0f)
			return 0.0f;

		float cosTheta = dot / (mag1 * mag2);

		// Clamp pour éviter les erreurs dues aux flottants
		cosTheta = std::max(-1.0f, std::min(1.0f, cosTheta));

		return std::acos(cosTheta) * 180.0f / 3.14159265f;
	}
}