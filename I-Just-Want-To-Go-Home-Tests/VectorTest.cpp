#include "stdafx.h"
#include "CppUnitTest.h"
#include "../I-Just-Want-To-Go-Home/Physics/PhysicsVector.h"
#include "../I-Just-Want-To-Go-Home/Physics/PhysicsVector.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace IJustWantToGoHomeTests
{
	TEST_CLASS(VectorTest)
	{
	public:
		TEST_METHOD(VectorCreation)
		{
			PhysicsVector v(10.0f, 23.0f);
			
			Assert::AreEqual(v.x, 10.0f);
			Assert::AreEqual(v.y, 23.0f);
			
		}

		TEST_METHOD(VectorAddition)
		{
			PhysicsVector v(1.0f, 2.0f);
			PhysicsVector v2(3.0f, -4.0f);
			PhysicsVector v3 = v + v2;

			Assert::AreEqual(v3.x, 4.0f);
			Assert::AreEqual(v3.y, -2.0f);
		}

		TEST_METHOD(VectorSubtraction)
		{
			PhysicsVector v(1.0f, 2.0f);
			PhysicsVector v2(3.0f, -4.0f);
			PhysicsVector v3 = v - v2;

			Assert::AreEqual(v3.x, -2.0f);
			Assert::AreEqual(v3.y, 6.0f);
		}

		TEST_METHOD(VectorDot)
		{
			PhysicsVector v(1.0f, 2.0f);
			PhysicsVector v2(3.0f, -4.0f);
			float f = v.dot(v2);

			Assert::AreEqual(f, -5.0f);
		}

		TEST_METHOD(VectorProjection)
		{
			PhysicsVector v(2.0f, 1.0f);
			PhysicsVector v2(3.0f, 4.0f);
			float f = v.project(v2);
			Assert::AreEqual(f, 2.0f);
		}

		TEST_METHOD(VectorScale)
		{
			PhysicsVector v(3.0f, 4.0f);
			float f = 3.0;
			PhysicsVector v2 = v * f;
			Assert::AreEqual(v2.x, 9.0f);
			Assert::AreEqual(v2.y, 12.0f);
		}

		TEST_METHOD(UnitVector)
		{
			PhysicsVector v(3.0, 4.0);
			PhysicsVector v2 = v.unit();
			Assert::AreEqual(v2.x, 0.6f);
			Assert::AreEqual(v2.y, 0.8f);
		}

		TEST_METHOD(VectorLength)
		{
			PhysicsVector v(3.0, 4.0);
			float f = v.length();
			Assert::AreEqual(f, 5.0f);
		}
	};
}