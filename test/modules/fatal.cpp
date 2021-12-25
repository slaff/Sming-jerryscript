#include <JerryTest.h>

namespace
{
IMPORT_FSTR(fatalSnap, PROJECT_DIR "/out/jerryscript/fatal.js.snap")
IMPORT_FSTR(fatalEsNextSnap, PROJECT_DIR "/files/fatal.es.next.js.snap")

JS_DEFINE_FUNCTION(throwTantrum, JS::Value& reason)
{
	// todo
	return true;
}

class FatalTest : public TestGroup
{
public:
	FatalTest() : TestGroup(_F("Fatal errors"))
	{
	}

	void execute() override
	{
		initContext();

		auto object = JS::global();

		TEST_CASE("Watchdog")
		{
			auto res = JS::Snapshot::load(fatalSnap);
			REQUIRE(!res.isError());
			JS::Watchdog::setPeriod(100);
			uint32_t startTime = system_get_time();
			JS_TRY()
			{
				auto object = JS::global();
				object.runFunction(F("infiniteLoop"));
				TEST_ASSERT(false);
			}
			JS_CATCH()
			{
				auto wdtElapsed = JS::Watchdog::read().as<NanoTime::Microseconds>();
				uint32_t timeElapsed = system_get_time() - startTime;
				debug_i("Time %u, Watchdog %s, %s", timeElapsed, wdtElapsed.toString().c_str(), String(e).c_str());
			}
		}

#ifndef JERRY_ESNEXT
		TEST_CASE("Bad bytecode")
		{
			// Attempt to load & execute snapshot build with es.next profile
			JS_TRY()
			{
				auto res = JS::Snapshot::load(fatalEsNextSnap);
				TEST_ASSERT(false);
			}
			JS_CATCH()
			{
				debug_i("%s", String(e).c_str());
			}
			auto res = object.runFunction("init");
			printValue("runFunction", res);
			REQUIRE(res.isError());
		}
#endif

		TEST_CASE("Start VM")
		{
			auto res = JS::Snapshot::load(fatalSnap);
			printValue("res", res);
			REQUIRE(!res.isError());
			REQUIRE(object.registerFunction("throwTantrum", throwTantrum));
		}

		TEST_CASE("Memory")
		{
			JS::Object dbg;
			JS_TRY()
			{
				object.runFunction("allocateArray", {4000, 3.5, dbg});
				TEST_ASSERT(false);
			}
			JS_CATCH()
			{
				auto wdt = JS::Watchdog::read().as<NanoTime::Microseconds>();
				debug_i("%s, watchdog %s", String(e).c_str(), wdt.toString().c_str());
				debug_i("Function failed @ i = %u", JS::Value(dbg["count"]).as<int>());
			}
		}

		TEST_CASE("Shutdown VM")
		{
			object.reset();
			JS::cleanup();
			auto heapUsed = JS::getHeapUsed();
			debug_i("Heap used: %u", heapUsed);
		}

		TEST_CASE("Use un-initialised VM")
		{
			JS::Object value{3.5};
			printValue("value", value);
		}
	}
};

} // namespace

void REGISTER_TEST(fatal)
{
// Assertions prevent testing of fatal error paths
#ifndef JERRY_ENABLE_DEBUG
	registerGroup<FatalTest>();
#endif
}
