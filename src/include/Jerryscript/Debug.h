/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Debug.h
 */

#pragma once

#include <jerry-core/include/jerryscript.h>

namespace Jerryscript
{
using HeapStats = jerry_heap_stats_t;

inline bool getHeapStats(HeapStats& stats)
{
	return jerry_get_memory_stats(&stats);
}

size_t getHeapUsed();

bool printHeap();

} // namespace Jerryscript
