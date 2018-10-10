#include "stdafx.h"
#include "CppUnitTest.h"
#include "../I-Just-Want-To-Go-Home/DataTypes/Vector.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace IJustWantToGoHomeTests
{
	TEST_CLASS(VectorTest)
	{
	public:
		TEST_METHOD(VectorCreation)
		{
			Vector v(10.0f, 23.0f);

			Assert::AreEqual(v.x, 10.0f);
			Assert::AreEqual(v.y, 23.0f);
		}

		TEST_METHOD(VectorAddition)
		{
			Vector v(1.0f, 2.0f);
			Vector v2(3.0f, -4.0f);
			Vector v3 = v + v2;

			Assert::AreEqual(v3.x, 4.0f);
			Assert::AreEqual(v3.y, -2.0f);
		}

		TEST_METHOD(VectorSubtraction)
		{
			Vector v(1.0f, 2.0f);
			Vector v2(3.0f, -4.0f);
			Vector v3 = v - v2;

			Assert::AreEqual(v3.x, -2.0f);
			Assert::AreEqual(v3.y, 6.0f);
		}

		TEST_METHOD(VectorDot)
		{
			Vector v(1.0f, 2.0f);
			Vector v2(3.0f, -4.0f);
			float f = v.dot(v2);

			Assert::AreEqual(f, -5.0f);
		}

		TEST_METHOD(VectorProjection)
		{
			Vector v(2.0f, 1.0f);
			Vector v2(3.0f, 4.0f);
			float f = v.project(v2);
			Assert::AreEqual(f, 2.0f);
		}

		TEST_METHOD(VectorScale)
		{
			Vector v(3.0f, 4.0f);
			float f = 3.0;
			Vector v2 = v * f;
			Assert::AreEqual(v2.x, 9.0f);
			Assert::AreEqual(v2.y, 12.0f);
		}

		TEST_METHOD(UnitVector)
		{
			Vector v(3.0, 4.0);
			Vector v2 = v.unit();
			Assert::AreEqual(v2.x, 0.6f);
			Assert::AreEqual(v2.y, 0.8f);
		}

		TEST_METHOD(VectorLength)
		{
			Vector v(3.0, 4.0);
			float f = v.length();
			Assert::AreEqual(f, 5.0f);
		}
	};
}