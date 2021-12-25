/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * VirtualMachine.cpp
 *
 * @author Nov 2021 - Slavey Karadzhov <slav@attachix.com>
 */

#include "include/Jerryscript/VirtualMachine.h"
#include "include/jerry_port_vm.h"
#include "jsvm-ext.h"

namespace Jerryscript
{
void initialise(jerry_init_flag_t flags)
{
	jerry_init(flags);

	auto context = global();
	context.registerFunction("print", printFunction);
#if DEBUG_BUILD
	context.registerFunction("alert", alertFunction);
#endif
}

void cleanup()
{
	jerry_cleanup();
}

Value eval(const String& jsCode)
{
	jerry_port_watchdog_reset();
	Value res = OwnedValue{jerry_eval(reinterpret_cast<const jerry_char_t*>(jsCode.c_str()), jsCode.length(), false)};
	if(res.isError()) {
		debug_e("[JS] eval failed: %s", String(Error(res)).c_str());
	}

	return res;
}

namespace Watchdog
{
void setPeriod(unsigned milliseconds)
{
	jerry_port_watchdog_set_period(milliseconds);
}

Timer2Clock::Ticks<uint32_t> read()
{
	return jerry_port_watchdog_read();
}

} // namespace Watchdog

namespace Snapshot
{
Value load(const uint32_t* snapshot, size_t size)
{
	jerry_port_watchdog_reset();
	Value res = OwnedValue{jerry_exec_snapshot(snapshot, size, 0, JERRY_SNAPSHOT_EXEC_COPY_DATA, nullptr)};
	if(res.isError()) {
		debug_e("[JS] execute snapshot failed: %s", String(Error(res)).c_str());
	}

	return res;
}

} // namespace Snapshot

} // namespace Jerryscript
