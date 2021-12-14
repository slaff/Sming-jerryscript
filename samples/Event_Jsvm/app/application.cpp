#include <SmingCore.h>
#include <Jerryscript.h>

namespace
{
JS::VirtualMachine vm;
SimpleTimer timer;
HashMap<String, JS::Callable::List> events;

IMPORT_FSTR(main_snap, PROJECT_DIR "/out/jerryscript/main.js.snap")

namespace JS
{
using namespace Jerryscript;

/**
 * @brief Function to register event listeners
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

	// Build the event object...
	JS::Object event;
	event["name"] = name;
	event["params"] = params;

	auto realm = JS::global();
	for(auto& listener : events[name]) {
		listener.call(realm, event);
	}

	return true;
}

void startJsvm()
{
	/*
	 * This is how we register a new function in JavaScript
	 * that will communicate directly with our C/C++ code.
	 */
	vm.registerFunction("addEventListener", addEventListener);

	if(!vm.load(main_snap)) {
		debug_e("Failed to load snapshot");
		return;
	}

	// Now you can initialize your script by calling the init() JavaScript function
	if(!vm.runFunction("init")) {
		debug_e("Failed executing the init function.");
	}

	/*
	 * Here we trigger every 2 seconds an event inside the JavaScript code.
	 */
	timer.initializeMs<2000>([]() {
		JS::Object params;
		params["temp"] = 20;
		triggerEvent("EVENT_TEMP", params);
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
