/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Task.h
 *
 * @author Nov 2021 - Slavey Karadzhov <slav@attachix.com>
 */

#pragma once

#include <Task.h>
#include "VirtualMachine.h"

namespace Jerryscript
{
/**
 * @brief - Task that runs the `loop` JavaScript function in the background
 */
class Task : public ::Task
{
public:
	Task(VirtualMachine& jsvm, unsigned intervalMs = 500) : jsvm(jsvm), intervalMs(intervalMs)
	{
	}

	void loop() override
	{
		if(!jsvm.runFunction("loop")) {
			error = FS("Failed running loop function. Suspending task");
			suspend();
			return;
		}

		if(intervalMs != 0) {
			sleep(intervalMs);
		}
	}

	bool hasError()
	{
		return error.length() > 0;
	}

	String getError()
	{
		return error;
	}

private:
	VirtualMachine& jsvm;
	unsigned intervalMs;
	String error;
};

} // namespace Jerryscript
