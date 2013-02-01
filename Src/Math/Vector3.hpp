#pragma once

namespace Framework
{
	namespace Math
	{
		struct Vector3{
			float x,y,z;

			friend bool operator==(const Vector3& vec, const Vector3& vec2);
			friend bool operator!=(const Vector3& vec, const Vector3& vec2);
		};
	}
}