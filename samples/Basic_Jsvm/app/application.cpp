#include <SmingCore.h>
#include <Jerryscript.h>

namespace
{
JS::Task task;

IMPORT_FSTR(main_snap, PROJECT_DIR "/out/jerryscript/main.js.snap")

void startJsvm()
{
	JS::initialise();

	if(!JS::Snapshot::load(main_snap)) {
		debug_e("Failed to load snapshot");
		return;
	}

	// Now you can initialize your script by calling a setup() JavaScript function
	if(JS::global().runFunction("setup").isError()) {
		debug_e("Failed executing the setup function.");
	}

	// And run the loop JavaScript function in the background as a Sming task.
	task.resume();
}

} // namespace

void init()
{
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.systemDebugOutput(true);

	startJsvm();
}
