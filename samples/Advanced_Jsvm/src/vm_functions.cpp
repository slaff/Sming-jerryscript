#include "include/vm_functions.h"

#include <WVector.h>
#include <debug_progmem.h>

namespace {

using Events=HashMap<String,jerry_value_t>;

Events events;

} // namespace

jerry_value_t addEventListener(const jerry_value_t function_obj,
		const jerry_value_t this_val, const jerry_value_t args_p[],
		const jerry_length_t args_cnt) {
	(void) function_obj; /* unused */
	(void) this_val; /* unused */

	// First Parameter - Event Name
	jerry_value_t value = jerry_value_to_string(args_p[0]);

	jerry_size_t valueSize = jerry_get_string_size(value);
	jerry_char_t eventName[valueSize + 1];

	jerry_string_to_char_buffer(value, eventName, valueSize);
	eventName[valueSize] = '\0';

	jerry_release_value (value);

	// Second parameter ->  callable
	auto jsFunction = args_p[1];

	if (!jerry_value_is_function(jsFunction)) {
		debug_e("Second parameter is not a function!");

		return jerry_create_boolean(false);
	}

	events[(char*)eventName] = jerry_value_to_object(jsFunction);

	return jerry_create_boolean(true);
}


void triggerEvent(const String& name, const JsEventData& data)
{
	if(!events.contains(name)) {
		return;
	}

	jerry_value_t jsFunction = events[name];

	/*
	 * Event = {
	 * 		name: "eventName"
	 * 		params: {
	 * 			"property1": "value1",
	 * 			"property2": "value2"
	 * 		}
	 * }
	 */

	// Build the event object...
	jerry_value_t eventObject = jerry_create_object();
	// Name
	jerry_value_t namePropertyName = jerry_create_string ((const jerry_char_t *) "name");
	jerry_value_t namePropertyValue = jerry_create_string ((const jerry_char_t *) name.c_str());
	jerry_value_t nameProperty = jerry_set_property (eventObject, namePropertyName, namePropertyValue);
	// Params
	jerry_value_t paramsPropertyName = jerry_create_string ((const jerry_char_t *) "params");
	jerry_value_t paramsPropertyValue = jerry_create_object();
	jerry_value_t paramsProperty = jerry_set_property (eventObject, paramsPropertyName, paramsPropertyValue);

	// add params...
	Vector<jerry_value_t> props;
	for(unsigned i=0; i< data.count(); i++) {
		jerry_value_t key = jerry_create_string (reinterpret_cast<const jerry_char_t *>(data.keyAt(i).c_str()));
		jerry_value_t value = jerry_create_string (reinterpret_cast<const jerry_char_t *>(data.valueAt(i).c_str()));
		jerry_value_t property = jerry_set_property (paramsPropertyValue, key, value);
		props.add(key);
		props.add(value);
		props.add(property);
	}

	jerry_value_t globalObject = jerry_get_global_object();
	jerry_value_t res = jerry_call_function(jsFunction, globalObject, &eventObject, 1);

	jerry_release_value(res);
	jerry_release_value(globalObject);

	for(unsigned i=0; i< props.count(); i++) {
		jerry_release_value(props[i]);
	}

	jerry_release_value (namePropertyName);
	jerry_release_value (namePropertyValue);
	jerry_release_value (nameProperty);

	jerry_release_value (paramsPropertyName);
	jerry_release_value (paramsPropertyValue);
	jerry_release_value (paramsProperty);

	jerry_release_value (eventObject);

//	bool is_error = jerry_value_is_error(res);
}
