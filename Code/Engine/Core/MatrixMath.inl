
#include "..\System\UnitTest.h"
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

			inline void PrintMatrix(char *i_name, const Engine::Matrix<4, 4> &i_matrix)
			{
				char str[128];
				strcpy_s(str, 128, i_name);
				strcat_s(str, 128, ": (%f, %f, %f, %f) (%f, %f, %f, %f) (%f, %f, %f, %f) (%f, %f, %f, %f)");
				DEBUG_PRINT(0, str,
					i_matrix.Get(0, 0), i_matrix.Get(0, 1), i_matrix.Get(0, 2), i_matrix.Get(0, 3),
					i_matrix.Get(1, 0), i_matrix.Get(1, 1), i_matrix.Get(1, 2), i_matrix.Get(1, 3),
					i_matrix.Get(2, 0), i_matrix.Get(2, 1), i_matrix.Get(2, 2), i_matrix.Get(2, 3),
					i_matrix.Get(3, 0), i_matrix.Get(3, 1), i_matrix.Get(3, 2), i_matrix.Get(3, 3));
			}

			inline void UnitTest()
			{
				UnitTest::Begin("Matrix Operations");

				Engine::Matrix<2, 3> A, B;
				A.SetRow(0, {1, 3, 1});
				A.SetRow(1, {1, 0, 0});
				B.SetRow(0, {0, 0, 5});
				B.SetRow(1, {7, 5, 0});
				UnitTest::Test("Comparison", A == A && A != B);

				Engine::Matrix<2, 3> C;
				C.SetRow(0, {1, 3, 6});
				C.SetRow(1, {8, 5, 0});
				UnitTest::Test("Addition", A + B == C);

				Engine::Matrix<3, 2> A_trans;
				A_trans.SetRow(0, {1, 1});
				A_trans.SetRow(1, {3, 0});
				A_trans.SetRow(2, {1, 0});
				UnitTest::Test("Transpose", A.Transposed() == A_trans && A.Transposed().Transposed() == A);

				Engine::Matrix<4, 4> D, D_inverse, identity;
				D.SetRow(0, {4, 0, 0, 0});
				D.SetRow(1, {0, 0, 2, 0});
				D.SetRow(2, {0, 1, 2, 0});
				D.SetRow(3, {1, 0, 0, 1});
				D_inverse.SetRow(0, {0.25f, 0, 0, 0});
				D_inverse.SetRow(1, {0, -1, 1, 0});
				D_inverse.SetRow(2, {0, 0.5f, 0, 0});
				D_inverse.SetRow(3, {-0.25f, 0, 0, 1});
				Engine::Matrix<4, 4>::SetIdentity(identity);
				UnitTest::Test("Inverse", D.Inverse() == D_inverse && D * D_inverse == identity);

				Engine::Matrix<3, 3> D_remove1;
				D_remove1.SetRow(0, { 0, 2, 0 });
				D_remove1.SetRow(1, { 1, 2, 0 });
				D_remove1.SetRow(2, { 0, 0, 1 });
				UnitTest::Test("RemoveRowColumn", D.RemoveRowColumn(0, 0) == D_remove1);

				Engine::Matrix<4, 4> D_cofactor;
				D_cofactor.SetRow(0, { -2, 0, 0, 2 });
				D_cofactor.SetRow(1, { 0, 8, -4, 0 });
				D_cofactor.SetRow(2, { 0, -8, 0, 0 });
				D_cofactor.SetRow(3, { 0, 0, 0, -8 });
				UnitTest::Test("Cofactor", D.Cofactor() == D_cofactor);

				Engine::Matrix<4, 4> D_adjugate;
				D_adjugate.SetRow(0, { -2, 0, 0, 0 });
				D_adjugate.SetRow(1, { 0, 8, -8, 0 });
				D_adjugate.SetRow(2, { 0, -4, 0, 0 });
				D_adjugate.SetRow(3, { 2, 0, 0, -8 });
				UnitTest::Test("Adjugate", D.Adjugate() == D_adjugate);

				Engine::Matrix<4, 4> E;
				E.SetRow(0, {3, 2, 0, 1});
				E.SetRow(1, {4, 0, 1, 2});
				E.SetRow(2, {3, 0, 2, 1});
				E.SetRow(3, {9, 2, 3, 1});
				UnitTest::Test("Determinant", D.Determinant() == -8 && E.Determinant() == 24 && identity.Determinant() == 1);

				UnitTest::Test("Trace", identity.Trace() == 4 && E.Trace() == 6 && D.Trace() == 7);

				Engine::Matrix<4, 4> F;
				F.SetRow(0, { 0, -4, -3, 1 });
				F.SetRow(1, { 4, 0, 1, 2 });
				F.SetRow(2, { 3, -1, 0, -3 });
				F.SetRow(3, { -1, -2, 3, 0 });
				UnitTest::Test("Skew-Symmetric", F.IsSkewSymmetric());
				UnitTest::Test("Symmetric", identity.IsSymmetric());

				UnitTest::End();

				UnitTest::Begin("Matrix Graphics Math");

				UnitTest::Test("Rotate Z", CreateRotationZ(90) * Vector3(1, 0, 0) == Vector3(0, 1, 0));
				UnitTest::Test("Rotate Y", CreateRotationY(90) * Vector3(1, 0, 0) == Vector3(0, 0, -1));
				UnitTest::Test("Rotate X", CreateRotationX(90) * Vector3(0, 1, 0) == Vector3(0, 0, 1));
				UnitTest::Test("Translation", CreateTranslation(1, 2, 3) * Vector3(0, 0, 0) == Vector3(1, 2, 3));

				UnitTest::End();
			}
		}
	}
}