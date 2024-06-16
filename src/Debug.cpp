/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Debug.cpp
 */

#include "include/Jerryscript/Debug.h"
#include <debug_progmem.h>

namespace Jerryscript
{
bool printHeap()
{
	jerry_heap_stats_t stats;
	if(!jerry_get_memory_stats(&stats)) {
		m_printf(_F("[JS] Heap stats disabled"));
		return false;
	}

	m_printf(_F("[JS] Heap size %u, allocated %u, peak %u\r\n"), unsigned(stats.size), unsigned(stats.allocated_bytes),
			 unsigned(stats.peak_allocated_bytes));
	return true;
}

size_t getHeapUsed()
{
	HeapStats stats;
	return getHeapStats(stats) ? stats.allocated_bytes : 0;
}

} // namespace Jerryscript
