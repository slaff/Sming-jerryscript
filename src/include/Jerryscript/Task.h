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
	Task(const Object& context, unsigned intervalMs = 500) : intervalMs(intervalMs)
	{
		this->context.reset(new Object(context));
	}

	Task(unsigned intervalMs = 500) : intervalMs(intervalMs)
	{
	}

	void loop() override
	{
		if(!context) {
			context.reset(new Object(global()));
		}

		auto res = context->runFunction("loop");
		if(res.isError()) {
			error = String(Error(res)) + FS(" error running loop function: Suspending task.");
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
	std::unique_ptr<Object> context;
	unsigned intervalMs;
	String error;
};

} // namespace Jerryscript
