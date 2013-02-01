#include <cstdint>
#include "Vector3.hpp"

namespace Framework
{
	namespace Math
	{
		bool operator==(const Vector3& vec, const Vector3& vec2){
			return (vec.x == vec2.x && vec.y == vec2.y && vec.z == vec2.z);
		}

		bool operator!=(const Vector3& vec, const Vector3& vec2){
			return !(vec2 == vec);
		}
	}
}
