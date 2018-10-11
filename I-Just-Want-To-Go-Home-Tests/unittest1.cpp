#include "stdafx.h"
#include "CppUnitTest.h"
#include "../I-Just-Want-To-Go-Home/TestableClass.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace IJustWantToGoHomeTests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		/*
			EXAMPLE TEST
			Note: Remember to add the header file of the class you're testing to the header files of I-Just-Want-To-Go-Home-Tests
			by right clicking and going Add -> Existing Item. You will also need to include the header to the test file itself 
		*/
		TEST_METHOD(TestMethod1)
		{
			TestableClass* t = new TestableClass();
			Assert::IsTrue(t->getTrue());
			delete t;
		}

	};
}