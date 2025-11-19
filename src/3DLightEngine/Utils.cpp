
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

	float GetDistance(gce::Vector3f32 const& vector1, gce::Vector3f32 const& vector2)
    {
	    float x = vector1.x - vector2.x;
    	float y = vector1.y - vector2.y;
    	float z = vector1.z - vector2.z;

    	return sqrt(x * x + y * y + z * z);
    }
	
	float GetDistance(float x1, float y1, float x2, float y2, float z1, float z2)
	{
		float x = x2 - x1;
		float y = y2 - y1;
		float z = z2 - z1;

		return sqrt(x * x + y * y + z * z);
	}

	float GetAngleDegree(const gce::Vector3f32& v1, const gce::Vector3f32& v2)
	{
		float dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

		float mag1 = std::sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
		float mag2 = std::sqrt(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z);

		// �vite les divisions par 0
		if (mag1 == 0.0f || mag2 == 0.0f)
			return 0.0f;

		float cosTheta = dot / (mag1 * mag2);

		// Clamp pour �viter les erreurs dues aux flottants
		cosTheta = std::max(-1.0f, std::min(1.0f, cosTheta));

		return std::acos(cosTheta) * 180.0f / 3.14159265f;
	}
}