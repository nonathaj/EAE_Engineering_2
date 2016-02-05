
#include "UnitTest.h"

#include <cassert>

#include "Console.h"

namespace Lame
{
	namespace UnitTest
	{
		namespace //private UnitTest variables
		{
			const char *current_test_item = NULL;
			const char * const pass_value = "PASS";
			const char * const fail_value = "FAIL";
		}

		void Begin(const char *i_object_test_name)
		{
			assert(i_object_test_name);
			//MessagedAssert(i_object_test_name, "Unit Test Begin() must accept a valid test object name.");
			current_test_item = i_object_test_name;
			Lame::ConsolePrint(false, NULL, 0, "/////////////////////////////////////////////////////////");
			Lame::ConsolePrint(false, NULL, 0, "%s Unit Test", current_test_item);
			Lame::ConsolePrint(false, NULL, 0, "---------------------------------------------------------");
		}

		bool Test(const char *i_test_name, bool i_pass)
		{
			assert(i_test_name);
			assert(current_test_item);
			//MessagedAssert(i_test_name, "Unit Test Test() must accept a valid test name.");
			//MessagedAssert(current_test_item, "Unit Test Test() must have an object to test on.  Please call Begin first.");
			Lame::ConsolePrint(false, NULL, 0, "%s %s: %s", current_test_item, i_test_name, i_pass ? pass_value : fail_value);
			return i_pass;
		}

		void End(void)
		{
			assert(current_test_item);
			//MessagedAssert(current_test_item, "Unit Test End() must have an object to test on.  Please call Begin first.");
			Lame::ConsolePrint(false, NULL, 0, "---------------------------------------------------------");
			Lame::ConsolePrint(false, NULL, 0, "%s Unit Test Complete", current_test_item);
			Lame::ConsolePrint(false, NULL, 0, "/////////////////////////////////////////////////////////");
			current_test_item = NULL;
		}
	}
}