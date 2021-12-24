#include <JerryTest.h>

namespace
{
IMPORT_FSTR(contextSnap, PROJECT_DIR "/out/jerryscript/context.js.snap")

class Context : public JS::ContextTemplate<Context>
{
public:
	bool init()
	{
		select();
		JS::initialise();
		if(!JS::global().registerFunction(F("callback"), callback)) {
			return false;
		}
		return JS::Snapshot::load(contextSnap).isTrue();
	}

protected:
	// This gets invoked when we run the javascriopt 'test' function
	JS_DEFINE_METHOD(callback, JS::Value& param)
	{
		JS::global()["result"] = param;
		return true;
	}
};

class ContextTest : public TestGroup
{
public:
	ContextTest() : TestGroup(_F("Context"))
	{
	}

	void execute() override
	{
		Context ctx1;
		Context ctx2;

		TEST_CASE("Initialise containers")
		{
			REQUIRE(ctx1.init());
			REQUIRE(ctx2.init());
		}

		TEST_CASE("Run first container normally")
		{
			ctx1.select();
			auto res = JS::global().runFunction("test", {12});
			printValue("res", res);
			REQUIRE(res.isTrue());
		}

		TEST_CASE("Run second container with fatal error")
		{
			ctx2.select();
			auto object = JS::global();
			object["forceFatal"] = true;
			JS_TRY()
			{
				object.runFunction("test");
				// Should never reach here
				TEST_ASSERT(false);
			}
			JS_CATCH()
			{
				// Note: Without cleanup we'd get another fatal error
				JS::cleanup();
				// testFunc is no longer registered
				auto resTest = object.runFunction("test");
				REQUIRE(resTest.isError());
				REQUIRE_EQ(JS::Error(resTest).errorType(), JS::ErrorType::Reference);
			}
		}

		TEST_CASE("Ensure first container remains active")
		{
			ctx1.select();
			REQUIRE(JS::global().runFunction("test").isTrue());
		}
	}
};

} // namespace

void REGISTER_TEST(context)
{
#ifdef JERRY_EXTERNAL_CONTEXT
	registerGroup<ContextTest>();
#endif
}
