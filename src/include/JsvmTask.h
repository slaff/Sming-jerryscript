/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * JsvmTask.h
 *
 * @author Nov 2021 - Slavey Karadzhov <slav@attachix.com>
 */

#pragma once

#include <Task.h>
#include "include/Jsvm.h"

/**
 * @brief - Task that runs the `loop` JavaScript function in the background
 */
class JsvmTask : public Task
{
public:
	JsvmTask(Jsvm& jsvm, unsigned intervalMs = 500) : jsvm(jsvm), intervalMs(intervalMs)
	{
	}

	void loop() override
	{
		if(!jsvm.runFunction("loop")) {
			error = F("Failed running loop function. Suspending task");
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
	Jsvm& jsvm;
	unsigned intervalMs;
	String error;
};
