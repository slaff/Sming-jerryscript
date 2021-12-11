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
		vm.reset(new JS::VirtualMachine);

		TEST_CASE("Start VM")
		{
			REQUIRE(vm->load(eventSnap));
			REQUIRE(vm->registerFunction("addEventListener", addEventListener));
			REQUIRE(vm->runFunction("init"));
		}

		TEST_CASE("Create error")
		{
			JS::Error error(JS::ErrorType::Range, "Bad range");
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

			auto realm = JS::global();
			JS::Object arg2;
			JS::Value res;
			JS::Value prop;

			res = listeners[0].call(realm, event);
			printValue("listeners[0].call", res);
			REQUIRE_EQ(res.type(), JS::Type::String);
			REQUIRE_EQ(res.as<String>(), "one");

			arg2["value"] = true;
			res = listeners[1].call(realm, {event, arg2});
			printValue("listeners[1].call", res);
			REQUIRE_EQ(res.type(), JS::Type::Number);
			REQUIRE_EQ(res.as<unsigned>(), 2);
			prop = arg2["value"];
			REQUIRE(prop.isNumber());
			REQUIRE_EQ(prop.as<unsigned>(), 2);

			arg2["value"] = 12.5;
			res = listeners[2].call(realm, {event, arg2});
			printValue("listeners[2].call", res);
			REQUIRE_EQ(res.type(), JS::Type::Number);
			REQUIRE_EQ(res.as<float>(), 3.5);
			prop = arg2.getProperty("value");
			REQUIRE(prop.isBoolean());
			REQUIRE_EQ(prop.as<bool>(), true);
		}

		JS::printHeap();

		TEST_CASE("Shutdown VM")
		{
			events.clear();
			vm.reset();

			REQUIRE_EQ(JS::getHeapUsed(), 0);
		}
	}

private:
	std::unique_ptr<JS::VirtualMachine> vm;
};

} // namespace

void REGISTER_TEST(event)
{
	registerGroup<EventTest>();
}
