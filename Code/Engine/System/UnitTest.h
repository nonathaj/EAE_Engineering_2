#ifndef _ENGINE_SYSTEM_UNITTEST_H
#define _ENGINE_SYSTEM_UNITTEST_H

namespace Engine
{
	/*
		UnitTest namespace

		Used for conducting unit tests on classes with a common interface
	*/
	namespace UnitTest
	{
		//Should be called at the beginning of a unit test to begin testing an object type
		void Begin(const char *i_object_test_name);

		//Should be called for each individual unit test to do, including the result to be printed
		bool Test(const char *i_test_name, bool i_pass);

		//Should be called at the end of a unit test to end testing an object type
		void End(void);
	}
}

#endif //_ENGINE_SYSTEM_UNITTEST_H