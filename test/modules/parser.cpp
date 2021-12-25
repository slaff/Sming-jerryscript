#include <JerryTest.h>

namespace
{
class ParserTest : public TestGroup
{
public:
	ParserTest() : TestGroup(_F("Parser"))
	{
	}

	void execute() override
	{
		initContext();

		TEST_CASE("eval")
		{
			REQUIRE(JS::eval("12 + 7 * 3").as<int>() == 33);
		}

#ifdef JERRY_ESNEXT
		TEST_CASE("String templates")
		{
			DEFINE_FSTR_LOCAL(FS_test, "function test() {\n"
									   " var a = 12;\n"
									   " var b = 13;\n"
									   " return `${a} ${b}`;"
									   "}\n"
									   "\n"
									   "test();\n");
			auto res = JS::eval(FS_test);
			printValue("res", res);
			REQUIRE_EQ(String("12 13"), res);
		}
#endif

		JS::cleanup();
	}
};

} // namespace

void REGISTER_TEST(parser)
{
	if(JS::isFeatureEnabled(JS::Feature::JsParser)) {
		registerGroup<ParserTest>();
	}
}
