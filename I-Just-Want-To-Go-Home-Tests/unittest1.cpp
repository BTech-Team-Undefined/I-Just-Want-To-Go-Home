#include "stdafx.h"
#include "CppUnitTest.h"
#include "../I-Just-Want-To-Go-Home/TestableClass.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace IJustWantToGoHomeTests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		TEST_METHOD(TestMethod1)
		{
			TestableClass* t = new TestableClass();
			Assert::IsTrue(t->getTrue());
			delete t;
		}
	};
}