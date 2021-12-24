#include <JerryTest.h>

#ifdef JERRY_EXTERNAL_CONTEXT
JS::Context globalContext;
#endif

void printValue(const String& name, const JS::Value& value)
{
	Serial.print(name);
	Serial.print(" = ");
	Serial.print(toString(value.type()));
	Serial.print(": ");
	if(value.isError()) {
		Serial.println(JS::Error(value));
	} else {
		Serial.println(value);
	}
}
