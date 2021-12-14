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
VirtualMachine::VirtualMachine(jerry_init_flag_t flags /* =JERRY_INIT_EMPTY */)
{
	jerry_init(flags);

	registerFunction("print", printFunction);
#ifdef DEBUG
	registerFunction("alert", alertFunction);
#endif
}

VirtualMachine::~VirtualMachine()
{
	jerry_cleanup();
}

bool VirtualMachine::eval(const String& jsCode)
{
	Value res = OwnedValue{jerry_eval(reinterpret_cast<const jerry_char_t*>(jsCode.c_str()), jsCode.length(), false)};
	if(res.isError()) {
		debug_e("[JS] eval failed: %s", String(Error(res)).c_str());
		return false;
	}

	return true;
}

bool VirtualMachine::load(const uint32_t* snapshot, size_t size)
{
	Value res = OwnedValue{jerry_exec_snapshot(snapshot, size, 0, JERRY_SNAPSHOT_EXEC_COPY_DATA, nullptr)};
	if(res.isError()) {
		debug_e("[JS] execute snapshot failed: %s", String(Error(res)).c_str());
		return false;
	}

	return true;
}

bool VirtualMachine::runFunction(const String& functionName)
{
	auto realm = global();
	Callable func = realm.getProperty(functionName);
	if(func.isError()) {
		debug_e("[JS] %s error: '%s' not found", String(Error(func)).c_str(), functionName.c_str());
		return false;
	}

	if(!func.isCallable()) {
		debug_e("[JS] error '%s': not a function", functionName.c_str());
		return false;
	}

	auto res = func.call(realm);
	if(res.isError()) {
		debug_e("[JS] %s error calling '%s'", String(Error(res)).c_str(), functionName.c_str());
		return false;
	}

	return true;
}

} // namespace Jerryscript
