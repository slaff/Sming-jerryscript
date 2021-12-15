#include <SmingCore.h>
#include <Jerryscript.h>

namespace
{
JS::VirtualMachine vm;
JS::Task task(vm);

IMPORT_FSTR(main_snap, PROJECT_DIR "/out/jerryscript/main.js.snap")

void startJsvm()
{
	if(!vm.load(main_snap)) {
		debug_e("Failed to load snapshot");
		return;
	}

	// Now you can initialize your script by calling a setup() JavaScript function
	if(!vm.runFunction("setup")) {
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
