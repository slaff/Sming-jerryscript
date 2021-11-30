#include <SmingCore.h>
#include <Jsvm.h>
#include <vm_functions.h>

namespace
{

Jsvm jsVm;
Timer tempTimer;

IMPORT_FSTR(main_snap, PROJECT_DIR "/out/jerryscript/main.js.snap")

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
