#include <JerryTest.h>

namespace
{
IMPORT_FSTR(eventSnap, PROJECT_DIR "/out/jerryscript/event.js.snap")
DEFINE_FSTR(testEventName, "EVENT_TEMP")
HashMap<String, Vector<JS::Callable>> events;

namespace JS
{
using namespace Jerryscript;

Value addEventListener(const CallInfo& callInfo, Value& eventName, Callable& function)
{
	if(!eventName.isString() || !function.isCallable()) {
		return ArgumentError(__FUNCTION__);
	}

	events[eventName].add(function);
	return true;
}

} // namespace JS

JS_DEFINE_FUNCTION(addEventListener, 2)

class EventTest : public TestGroup
{
public:
	EventTest() : TestGroup(_F("Events"))
	{
	}

	void execute() override
	{
		JS::initialise();

		auto context = JS::global();

		TEST_CASE("Start VM")
		{
			DEFINE_FSTR_LOCAL(FS_moduleLoaded, "OK, module loaded!");
			DEFINE_FSTR_LOCAL(FS_addEventListener, "addEventListener");
			REQUIRE(FS_moduleLoaded == JS::Snapshot::load(eventSnap));
			REQUIRE(context.registerFunction(FS_addEventListener, addEventListener));
			REQUIRE(!context.runFunction("init").isError());
		}

		TEST_CASE("Create error")
		{
			JS::Error error(JS::ErrorType::Range, F("Bad range"));
			Serial.println(error.message());
			printValue("error", error);
		}

		TEST_CASE("Trigger event")
		{
			JS::Object params;
			params["temp"] = 20;

			auto& listeners = events[testEventName];
			REQUIRE_EQ(listeners.count(), 3);

			JS::Object event;
			event["name"] = testEventName;
			event["params"] = params;

			JS::Object arg2;
			JS::Value res;
			JS::Value prop;

			res = listeners[0].call(context, event);
			printValue(F("listeners[0].call"), res);
			REQUIRE_EQ(res.type(), JS::Type::String);
			REQUIRE_EQ(res.as<String>(), "one");

			arg2["value"] = true;
			res = listeners[1].call(context, {event, arg2});
			printValue(F("listeners[1].call"), res);
			REQUIRE_EQ(res.type(), JS::Type::Number);
			REQUIRE_EQ(res.as<unsigned>(), 2);
			prop = arg2["value"];
			REQUIRE(prop.isNumber());
			REQUIRE_EQ(prop.as<unsigned>(), 2);

			arg2["value"] = 12.5;
			res = listeners[2].call(context, {event, arg2});
			printValue(F("listeners[2].call"), res);
			REQUIRE_EQ(res.type(), JS::Type::Number);
			REQUIRE_EQ(res.as<float>(), 3.5);
			prop = arg2.getProperty("value");
			REQUIRE(prop.isBoolean());
			REQUIRE_EQ(prop.as<bool>(), true);
		}

		JS::printHeap();

		TEST_CASE("Shutdown VM")
		{
			context.reset();
			events.clear();
			JS::cleanup();

			REQUIRE_EQ(JS::getHeapUsed(), 0);
		}
	}
};

} // namespace

void REGISTER_TEST(event)
{
	registerGroup<EventTest>();
}
