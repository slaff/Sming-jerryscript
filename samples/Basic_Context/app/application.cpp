#include <SmingCore.h>
#include <Jerryscript.h>

namespace
{
/*
 * This is our own Context implementation.
 */
class MyContext : public JS::ContextTemplate<MyContext>
{
public:
	MyContext(const String& name) : ContextTemplate(2048), name(name)
	{
	}

	void init();

	void notifyTemperature(int temp, const String& origin)
	{
		// Must select before creating values so correct heap is used
		select();

		JS::Object params;
		params["temp"] = temp;
		params["origin"] = origin;
		triggerEvent(F("EVENT_TEMP"), params);
	}

	const String& getName() const
	{
		return name;
	}

protected:
	bool triggerEvent(const String& name, const JS::Object& params);

	JS_DEFINE_METHOD(addEventListener, JS::Value& eventName, JS::Callable& function)
	{
		debug_i("%s.addListener('%s', %p)", getName().c_str(), String(eventName).c_str(), function.get());
		events[eventName].add(function);
		return true;
	}

	JS_DEFINE_METHOD_VAR(myPrint)
	{
		debug_i("%s called with %u arguments", __FUNCTION__, argCount);
		Serial.print(F("[myPrint] "));
		for(unsigned i = 0; i < argCount; ++i) {
			if(i > 0) {
				Serial.print(", ");
			}
			Serial.print(args[i]);
		}
		Serial.println();
		return JS::Undefined();
	}

private:
	String name;
	HashMap<String, JS::Callable::List> events;
};

JS::ContextList<MyContext> contexts;
SimpleTimer timer;
IMPORT_FSTR(main_snap, PROJECT_DIR "/out/jerryscript/main.js.snap")

/*
 * Called at startup to initialise our jerryscript engine,
 * Also called on fatal exceptiuon to re-initialise.
 */
void MyContext::init()
{
	debug_i("%s.init()", getName().c_str());

	select();

	// Release any allocated jerryscript values
	events.clear();

	JS::initialise();

	auto object = JS::global();

	/*
	 * Register external function for script use
	 */
	object.registerFunction(F("addEventListener"), addEventListener);
	object.registerFunction(F("myPrint"), myPrint);

	/*
	 * Script calls it's own init() code on loading
	 */
	if(JS::Snapshot::load(main_snap).isError()) {
		debug_e("Failed to load snapshot");
	}
}

/*
 * Dispatch an event to all registered listeners
 *
 * Event = {
 * 		name: "eventName"
 * 		params: {
 * 			"property1": value1,
 * 			"property2": value2,
 * 			...
 * 		}
 * }
 */
bool MyContext::triggerEvent(const String& name, const JS::Object& params)
{
	if(!events.contains(name)) {
		debug_e("%s: Unknown event '%s'", __FUNCTION__, name.c_str());
		return false;
	}

	JS_TRY()
	{
		// Build the event object...
		JS::Object event;
		event["name"] = name;
		event["params"] = params;

		auto object = JS::global();
		for(auto& listener : events[name]) {
			debug_i("%s.triggerEvent('%s', %p)", getName().c_str(), name.c_str(), listener.get());
			listener.call(object, event);
		}

		return true;
	}
	JS_CATCH()
	{
		debug_i("%s: %s", getName().c_str(), String(e).c_str());
		debug_i("System heap %u", system_get_free_heap_size());
		System.queueCallback(TaskDelegate(&MyContext::init, this));
		return false;
	}
}

void startJsvm()
{
	// Allow 100ms max. for each call
	JS::Watchdog::setPeriod(100);

	contexts.add(new MyContext(F("First")));
	contexts.add(new MyContext(F("Second")));

	for(auto& ctx : contexts) {
		ctx.init();
	}

	/*
	 * Tell javascript code to force an out-of-memory fatal error.
	 * It will get reset but the second context will be unaffected.
	 */
	contexts.head()->select();
	JS::global()[F("exceptionEnabled")] = true;

	/*
	 * Here we trigger every 2 seconds an event inside the JavaScript code.
	 */
	timer.initializeMs<2000>([]() {
		int temp = random(25, 100);
		String origin = F("cpu/temp");
		contexts.foreach([=](auto& ctx) { ctx.notifyTemperature(temp, origin); });
	});
	timer.start();
}

} // namespace

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	startJsvm();
}
