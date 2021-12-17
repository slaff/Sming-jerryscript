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
#include "jsvm-ext.h"

namespace Jerryscript
{
void initialise(jerry_init_flag_t flags)
{
	jerry_init(flags);

	auto realm = global();
	realm.registerFunction("print", printFunction);
#ifdef DEBUG
	realm.registerFunction("alert", alertFunction);
#endif
}

void cleanup()
{
	jerry_cleanup();
}

Value eval(const String& jsCode)
{
	Value res = OwnedValue{jerry_eval(reinterpret_cast<const jerry_char_t*>(jsCode.c_str()), jsCode.length(), false)};
	if(res.isError()) {
		debug_e("[JS] eval failed: %s", String(Error(res)).c_str());
	}

	return res;
}

namespace Snapshot
{
Value load(const uint32_t* snapshot, size_t size)
{
	Value res = OwnedValue{jerry_exec_snapshot(snapshot, size, 0, JERRY_SNAPSHOT_EXEC_COPY_DATA, nullptr)};
	if(res.isError()) {
		debug_e("[JS] execute snapshot failed: %s", String(Error(res)).c_str());
	}

	return res;
}

} // namespace Snapshot

} // namespace Jerryscript
