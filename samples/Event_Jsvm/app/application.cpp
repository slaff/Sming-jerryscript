#include <SmingCore.h>
#include <Jsvm.h>

namespace
{
using JsEventData = HashMap<String, String>;
using EventList = Vector<jerry_value_t>;
using Events = HashMap<String, EventList>;

Jsvm jsVm;
Events events;
Timer tempTimer;

IMPORT_FSTR(main_snap, PROJECT_DIR "/out/jerryscript/main.js.snap")

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
jerry_value_t addEventListener(const jerry_call_info_t* call_info_p, const jerry_value_t args_p[],
							   const jerry_length_t args_count)
{
	(void)call_info_p;

	if(args_count != 2) {
		return jerry_create_boolean(false);
	}

	// First Parameter - Event Name
	jerry_value_t value = jerry_value_to_string(args_p[0]);

	jerry_size_t valueSize = jerry_get_string_size(value);
	jerry_char_t eventName[valueSize + 1];

	jerry_string_to_char_buffer(value, eventName, valueSize);
	eventName[valueSize] = '\0';

	jerry_release_value(value);

	// Second parameter ->  callable
	auto jsFunction = args_p[1];

	if(!jerry_value_is_function(jsFunction)) {
		debug_e("Second parameter is not a function!");

		return jerry_create_boolean(false);
	}

	events[reinterpret_cast<char*>(eventName)].addElement(jerry_value_to_object(jsFunction));

	return jerry_create_boolean(true);
}

void triggerEvent(const String& name, const JsEventData& data)
{
	if(!events.contains(name)) {
		return;
	}

	/*
	 * Event = {
	 * 		name: "eventName"
	 * 		params: {
	 * 			"property1": "value1",
	 * 			"property2": "value2"
	 * 			...
	 * 		}
	 * }
	 */

	// Build the event object...
	jerry_value_t eventObject = jerry_create_object();
	// Name
	jerry_value_t namePropertyName = jerry_create_string((const jerry_char_t*)"name");
	jerry_value_t namePropertyValue = jerry_create_string((const jerry_char_t*)name.c_str());
	jerry_value_t nameProperty = jerry_set_property(eventObject, namePropertyName, namePropertyValue);
	// Params
	jerry_value_t paramsPropertyName = jerry_create_string((const jerry_char_t*)"params");
	jerry_value_t paramsPropertyValue = jerry_create_object();
	jerry_value_t paramsProperty = jerry_set_property(eventObject, paramsPropertyName, paramsPropertyValue);

	// add params...
	Vector<jerry_value_t> props;
	for(unsigned i = 0; i < data.count(); i++) {
		jerry_value_t key = jerry_create_string(reinterpret_cast<const jerry_char_t*>(data.keyAt(i).c_str()));
		jerry_value_t value = jerry_create_string(reinterpret_cast<const jerry_char_t*>(data.valueAt(i).c_str()));
		jerry_value_t property = jerry_set_property(paramsPropertyValue, key, value);
		props.add(key);
		props.add(value);
		props.add(property);
	}

	jerry_value_t globalObject = jerry_get_global_object();

	EventList listeners = events[name];
	for(unsigned i = 0; i < listeners.count(); i++) {
		jerry_value_t res = jerry_call_function(listeners[i], globalObject, &eventObject, 1);
		jerry_release_value(res);
	}

	jerry_release_value(globalObject);

	for(unsigned i = 0; i < props.count(); i++) {
		jerry_release_value(props[i]);
	}

	jerry_release_value(namePropertyName);
	jerry_release_value(namePropertyValue);
	jerry_release_value(nameProperty);

	jerry_release_value(paramsPropertyName);
	jerry_release_value(paramsPropertyValue);
	jerry_release_value(paramsProperty);

	jerry_release_value(eventObject);
}

void startJsvm()
{
	/*
	 * This is how we register a new function in JavaScript
	 * that will communicate directly with our C/C++ code.
	 */
	jsVm.registerFunction("addEventListener", addEventListener);

	if(!jsVm.load(main_snap)) {
		debug_e("Failed to load snapshot");
		return;
	}

	// Now you can initialize your script by calling the init() JavaScript function
	if(!jsVm.runFunction("init")) {
		debug_e("Failed executing the init function.");
	}

	/*
	 * Here we trigger every 2 seconds an event inside the JavaScript code.
	 */
	tempTimer.initializeMs(2000, TimerDelegate([](){
		JsEventData params;
		params["temp"]="20";
		triggerEvent("EVENT_TEMP", params);
	})).start();
}

} // namespace

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	startJsvm();
}
