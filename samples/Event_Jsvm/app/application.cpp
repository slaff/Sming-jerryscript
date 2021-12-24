#include <SmingCore.h>
#include <Jerryscript.h>

namespace
{
SimpleTimer timer;
HashMap<String, JS::Callable::List> events;

IMPORT_FSTR(main_snap, PROJECT_DIR "/out/jerryscript/main.js.snap")

namespace JS
{
using namespace Jerryscript;

/**
 * @brief Function to register event listeners
 *
 * This example considers handling notifications from one or more temperature sensors,
 * identified by the "origin" property.
 *
 * @code {.javascript}
 * 
 * event = {
 *    name => "TEMP_CHANGE",
 *    params = {
 *    },
 *    "origin" => "The\Creator\Of\The\Event" 
 * };
 *
 * addEventListener("TEMP_CHANGE", function(event) {
 *      console.log("Got Event" + event.name);
 * });
 *
 * @endcode
 *
 */
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

void startJsvm();

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
bool triggerEvent(const String& name, const JS::Object& params)
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
			listener.call(object, event);
		}

		return true;
	}
	JS_CATCH()
	{
		debug_i("%s", String(e).c_str());
		debug_i("System heap %u", system_get_free_heap_size());
		System.queueCallback(startJsvm);
		return false;
	}
}

/*
 * Called at startup to initialise our jerryscript engine,
 * Also called on fatal exceptiuon to re-initialise.
 */
void startJsvm()
{
	// Release any allocated jerryscript values
	events.clear();

	JS::initialise();

	auto context = JS::global();

	/*
	 * This is how we register a new function in JavaScript
	 * that will communicate directly with our C/C++ code.
	 */
	context.registerFunction("addEventListener", addEventListener);

	if(JS::Snapshot::load(main_snap).isError()) {
		debug_e("Failed to load snapshot");
		return;
	}

	// Now you can initialize your script by calling the init() JavaScript function
	if(context.runFunction("init").isError()) {
		debug_e("Failed executing the init function.");
	}

	/*
	 * Here we trigger every 2 seconds an event inside the JavaScript code.
	 */
	timer.initializeMs<2000>([]() {
		JS::Object params;
		params["temp"] = int(random(25, 100));
		params["origin"] = F("cpu/temp");
		triggerEvent(F("EVENT_TEMP"), params);
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
