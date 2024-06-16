#include <JerryTest.h>

namespace
{
class TypeTest : public TestGroup
{
public:
	TypeTest() : TestGroup(_F("Types"))
	{
	}

	void execute() override
	{
		initContext();

		// Initialise some intrinsics which don't get tidied by garbage collection
		{
			JS::Error error(JS::ErrorType::Range);
			String s(error);
			JS::Object().keys();
		}

		JS::gc();
		auto initialHeapUsed = JS::getHeapUsed();

		TEST_CASE("Strings")
		{
			REQUIRE(!String(JS::Value()));
			REQUIRE(!String(JS::Error()));
			REQUIRE(String(JS::Value("")));
			REQUIRE_EQ(String("undefined"), JS::Value(JS::Undefined{}));
			REQUIRE_EQ(String("null"), JS::Value(JS::Null{}));
			REQUIRE_EQ(String("3.141592653589793"), JS::Value(3.141592653589793));
			REQUIRE_EQ(String("-123456789"), JS::Value(-123456789));
			REQUIRE_EQ(String("123456789"), JS::Value(123456789));
			REQUIRE_EQ(String("true"), JS::Value(true));
		}

		TEST_CASE("Object properties")
		{
			String key1 = "temp";
			const char* key2 = "str";
			DEFINE_FSTR_LOCAL(key3, "Longer keys are possible but of dubious value");

			JS::Object params;
			params[key1] = 20;
			params[key2] = F("1234XX");
			params[key3] = true;

			auto keys = params.keys();
			for(auto s : params.keys()) {
				printValue(String('[') + s.index() + "] " + s, params[s]);
			}
			REQUIRE_EQ(keys.count(), 3);
			REQUIRE_EQ(key1, keys[0]);
			REQUIRE_EQ(String(key2), keys[1]);
			REQUIRE_EQ(String(key3), keys[2]);
			const JS::Object& cparams = params;
			REQUIRE_EQ(cparams[key1].type(), JS::Type::Number);
			REQUIRE_EQ(cparams[key2].type(), JS::Type::String);
			REQUIRE_EQ(cparams[key3].type(), JS::Type::Boolean);
		}

		TEST_CASE("Numeric limits")
		{
			testNumber<int>("int32");
			testNumber<unsigned>("uint32");
			testNumber<double, int>("float");
		}

		TEST_CASE("Error object")
		{
			DEFINE_FSTR_LOCAL(FS_msg, "Some text for the error")
			JS::Error error(JS::ErrorType::Range, FS_msg);
			printValue("error", error);
			REQUIRE_EQ(String(error.message()), FS_msg);
			REQUIRE_EQ(error.errorType(), JS::ErrorType::Range);
		}

		TEST_CASE("Memory leaks")
		{
			for(unsigned i = 0; i < 100; ++i) {
				JS::Value value(F("Test string allocated/deallocated multiple times"));
			}

			JS::gc();
			REQUIRE_EQ(JS::getHeapUsed(), initialHeapUsed);
		}

		JS::cleanup();
	}

	template <typename T> void testNumber(const String& type, T min, T max)
	{
		debug_w(">> %s", type.c_str());
		JS::Value value;
		value = min;
		REQUIRE_EQ(value.as<T>(), min);
		value = max;
		REQUIRE_EQ(value.as<T>(), max);
	}

	template <typename T, typename Q = T> void testNumber(const String& type)
	{
		using Limits = std::numeric_limits<Q>;
		testNumber<T>(type, Limits::min(), Limits::max());
	}
};

} // namespace

void REGISTER_TEST(types)
{
	registerGroup<TypeTest>();
}
