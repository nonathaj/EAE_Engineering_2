
#include "Math.h"
#include <math.h>

namespace Engine
{
	namespace Math
	{
		namespace Matrix
		{
			inline Engine::Matrix<4, 4> CreateRotationX(float i_angle_degrees)
			{
				float radians = Math::ToRadians(i_angle_degrees), v_sin = sin(radians), v_cos = cos(radians);
				Engine::Matrix<4, 4> matrix;
				Engine::Matrix<4, 4>::SetIdentity(matrix);
				matrix.Set(1, 1, v_cos);
				matrix.Set(1, 2, 0 - v_cos);
				matrix.Set(2, 1, v_sin);
				matrix.Set(2, 2, v_sin);
				return matrix;
			}

			inline Engine::Matrix<4, 4> CreateRotationY(float i_angle_degrees)
			{
				float radians = Math::ToRadians(i_angle_degrees), v_sin = sin(radians), v_cos = cos(radians);
				Engine::Matrix<4, 4> matrix;
				Engine::Matrix<4, 4>::SetIdentity(matrix);
				matrix.Set(0, 0, v_cos);
				matrix.Set(0, 2, v_sin);
				matrix.Set(2, 0, 0 - v_sin);
				matrix.Set(2, 2, v_cos);
				return matrix;
			}

			inline Engine::Matrix<4, 4> CreateRotationZ(float i_angle_degrees)
			{
				float radians = Math::ToRadians(i_angle_degrees), v_sin = sin(radians), v_cos = cos(radians);
				Engine::Matrix<4, 4> matrix;
				Engine::Matrix<4, 4>::SetIdentity(matrix);
				matrix.Set(0, 0, v_cos);
				matrix.Set(0, 1, 0 - v_sin);
				matrix.Set(1, 0, v_sin);
				matrix.Set(1, 1, v_cos);
				return matrix;
			}

			inline Engine::Matrix<4, 4> CreateRotation(float i_x_deg, float i_y_deg, float i_z_deg)
			{
				Engine::Matrix<4, 4> rotations;
				Engine::Matrix<4, 4>::SetIdentity(rotations);
				if (i_x_deg != 0)
					rotations *= CreateRotationX(i_x_deg);
				if (i_y_deg != 0)
					rotations *= CreateRotationY(i_y_deg);
				if (i_z_deg != 0)
					rotations *= CreateRotationZ(i_z_deg);
				return rotations;
			}

			inline Engine::Matrix<4, 4> CreateRotation(const Vector3 &i_rot)
			{
				return CreateRotation(i_rot.x(), i_rot.y(), i_rot.z());
			}

			inline Engine::Matrix<4, 4> CreateTranslation(float i_x, float i_y, float i_z)
			{
				Engine::Matrix<4, 4> matrix;
				Engine::Matrix<4, 4>::SetIdentity(matrix);
				matrix.Set(0, 3, i_x);
				matrix.Set(1, 3, i_y);
				matrix.Set(2, 3, i_z);
				return matrix;
			}

			inline Engine::Matrix<4, 4> CreateTranslation(const Vector3 &i_pos)
			{
				return CreateTranslation(i_pos.x(), i_pos.y(), i_pos.z());
			}

			inline Engine::Matrix<4, 1> AsMatrix(const Engine::Vector3 &i_vector, float fourthVal)
			{
				Engine::Matrix<4, 1> matrix;
				matrix.Set(0, 0, i_vector.x());
				matrix.Set(1, 0, i_vector.y());
				matrix.Set(2, 0, i_vector.z());
				matrix.Set(3, 0, fourthVal);
				return matrix;
			}

			inline Engine::Vector3 AsVector(const Engine::Matrix<4, 1> &i_matrix)
			{
				return Vector3(i_matrix.Get(0, 0), i_matrix.Get(1, 0), i_matrix.Get(2, 0));
			}

			inline Engine::Vector3 RotationOnly(const Engine::Matrix<4, 4> &i_matrix, const Engine::Vector3 &i_vector)
			{
				return AsVector(i_matrix * AsMatrix(i_vector, 0));
			}

			inline Engine::Vector2 RotationOnly(const Engine::Matrix<4, 4> &i_matrix, const Engine::Vector2 &i_vector)
			{
				Vector3 rhs(i_vector.x(), i_vector.y(), 0);
				Vector3 temp = RotationOnly(i_matrix, rhs);
				return Vector2(temp.x(), temp.y());
			}

			inline Engine::Vector3 operator*(const Engine::Matrix<4, 4> &i_matrix, const Engine::Vector3 &i_vector)
			{
				return AsVector(i_matrix * AsMatrix(i_vector, 1));
			}

			inline Engine::Vector2 operator*(const Engine::Matrix<4, 4> &i_matrix, const Engine::Vector2 &i_vector)
			{
				Vector3 rhs(i_vector.x(), i_vector.y(), 0);
				Vector3 temp = i_matrix * rhs;
				return Vector2(temp.x(), temp.y());
			}

			inline void Translate(Engine::Vector3 &o_vec, float i_x, float i_y, float i_z)
			{
				o_vec = CreateTranslation(i_x, i_y, i_z) * o_vec;
			}

			inline void Translate(Vector3 &o_vec, Vector3 i_translation)
			{
				Translate(o_vec, i_translation.x(), i_translation.y(), i_translation.z());
			}

			inline void Rotate(Engine::Vector3 &o_vec, float i_x_deg, float i_y_deg, float i_z_deg)
			{
				o_vec = CreateRotation(i_x_deg, i_y_deg, i_z_deg) * o_vec;
			}

			inline void Rotate(Vector3 &o_vec, Vector3 i_rotation)
			{
				Rotate(o_vec, i_rotation.x(), i_rotation.y(), i_rotation.z());
			}

			inline Vector2 AbsoluteValues(const Vector2 &i_vec)
			{
				Vector2 vec;
				vec.x(fabs(i_vec.x()));
				vec.y(fabs(i_vec.y()));
				return vec;
			}

			inline Vector3 AbsoluteValues(const Vector3 &i_vec)
			{
				Vector3 vec;
				vec.x(fabs(i_vec.x()));
				vec.y(fabs(i_vec.y()));
				vec.z(fabs(i_vec.z()));
				return vec;
			}

			//reflects a vector around a given normal
			inline Vector3 Reflect(const Vector3 &i_direction, const Vector3 &i_normal)
			{
				Vector3 norm = i_normal.normalized();		//ensure that normal is length 1
				return i_direction - 2.0f * i_direction.dot(norm) * norm;
			}
		}
	}
}
