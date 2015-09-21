#ifndef _ENGINE_MATH_MATRIXMATH_H
#define _ENGINE_MATH_MATRIXMATH_H

#include "Vector2.h"
#include "Vector3.h"
#include "Matrix.h"

namespace Engine
{
	namespace Math
	{
		namespace Matrix
		{
			//Mathematical Matrix Constructors
			inline Engine::Matrix<4, 4> CreateRotationX(float i_angle_degrees);
			inline Engine::Matrix<4, 4> CreateRotationY(float i_angle_degrees);
			inline Engine::Matrix<4, 4> CreateRotationZ(float i_angle_degrees);
			inline Engine::Matrix<4, 4> CreateRotation(float i_x_deg, float i_y_deg, float i_z_deg);
			inline Engine::Matrix<4, 4> CreateRotation(const Vector3 &i_rot);
			inline Engine::Matrix<4, 4> CreateTranslation(float i_x, float i_y, float i_z);
			inline Engine::Matrix<4, 4> CreateTranslation(const Vector3 &i_pos);

			//Vector manipulation with math matrices
			inline Engine::Matrix<4, 1> AsMatrix(const Engine::Vector3 &i_vector, float fourthVal);
			inline Vector3 AsVector(const Engine::Matrix<4, 1> &i_matrix);

			inline void Translate(Vector3 &o_vec, float i_x, float i_y, float i_z);
			inline void Translate(Vector3 &o_vec, Vector3 i_translation);
			inline void Rotate(Vector3 &o_vec, float i_x_deg, float i_y_deg, float i_z_deg);
			inline void Rotate(Vector3 &o_vec, Vector3 i_rotation);

			inline Vector3 RotationOnly(const Engine::Matrix<4, 4> &i_matrix, const Vector3 &i_vector);
			inline Vector2 RotationOnly(const Engine::Matrix<4, 4> &i_matrix, const Vector2 &i_vector);
			inline Vector3 operator*(const Engine::Matrix<4, 4> &i_matrix, const Vector3 &i_vector);
			inline Vector2 operator*(const Engine::Matrix<4, 4> &i_matrix, const Vector2 &i_vector);

			//Vector Helper Functions
			inline Vector2 AbsoluteValues(const Vector2 &i_vec);
			inline Vector3 AbsoluteValues(const Vector3 &i_vec);

			//reflects a vector around a given normal
			inline Vector3 Reflect(const Vector3 &i_direction, const Vector3 &i_normal);

			inline void PrintMatrix(char *i_name, const Engine::Matrix<4, 4> &i_matrix);

			inline static void UnitTest();
		}
	}
}
#include "MatrixMath.inl"

#endif //_ENGINE_MATH_MATRIXMATH_H