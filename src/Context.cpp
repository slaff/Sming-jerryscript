/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Context.cpp
 *
 * @author: Dec 2021 - Mikee47 <mike@sillyhouse.net>
 *
 */

#include "include/Jerryscript/Context.h"

namespace Jerryscript
{
Context* Context::current;

Context::Context() : Context(JERRY_GLOBAL_HEAP_SIZE * 1024)
{
}

Context::Context(size_t heapSize)
{
	context = std::make_unique<uint8_t[]>(std::max(heapSize, size_t(1024U)));
}

void* Context::alloc(size_t size, void* param)
{
	// Store user parameter (this pointer) after jerryscript context
	auto buf = new uint8_t[size + sizeof(param)];
	if(buf != nullptr) {
		*reinterpret_cast<void**>(buf + size) = param;
	}
	return buf;
}

} // namespace Jerryscript
