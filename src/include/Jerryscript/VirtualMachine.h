/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * VirtualMachine.h
 *
 * @author Nov 2021 - Slavey Karadzhov <slav@attachix.com>
 */

#pragma once

#include "Types.h"
#include <FileSystem.h>
#include <Platform/Clocks.h>

namespace Jerryscript
{
/**
* @brief Initializes the JavaScript VM
*/
void initialise(jerry_init_flag_t flags = JERRY_INIT_EMPTY);

/**
 * @brief Clean up the virtual machine by unloading snapshots, freeing allocated memory, etc.
 *
 * Note that this does not release Jerryscript heap memory allocated to existing values.
 * This is done via `Value` destructor or by manually calling `Value::reset()`.
 */
void cleanup();

/**
 * @brief Check if optional feature is available
 * @param feature
 * @retval bool true if library has been compiled with requested feature
 */
inline bool isFeatureEnabled(Feature feature)
{
	return jerry_is_feature_enabled(jerry_feature_t(feature));
}

namespace Watchdog
{
/**
 * @brief Set watchdog period
 * @param milliseconds Timeout period, use 0 to disable
 */
void setPeriod(unsigned milliseconds);

/**
 * @brief Get elapsed watchdog time since last reset
 * @retval uint32_t Time in internal timer ticks
 * @note For debugging and inspection use only
 */
Timer2Clock::Ticks<uint32_t> read();

}; // namespace Watchdog

/*
 * @brief Parses the JavaScript code and prepares it for execution
 * @retval bool true on success
 * 
 * Requires jerryscript to be compiled with parsing enabled.
 */
Value eval(const String& jsCode);

/**
 * @brief Snapshot management functions
 */
namespace Snapshot
{
/**
 * @name Load a snapshot into the virtual machine and execute it
 * @{
 */

/**
 * @brief Load from memory buffer
 * @param snapshot Points to snapshot content
 * @param snapshotSize Number of bytes in snapshot
 * @retval Value Result from execution, or Error
 */
Value load(const uint32_t* snapshot, size_t snapshotSize);

/**
 * @brief Load snapshot from String and execute it
 * @param snapshot Snapshot content
 * @retval Value Result from execution, or Error
 */
inline Value load(const String& snapshot)
{
	return load(reinterpret_cast<const uint32_t*>(snapshot.c_str()), snapshot.length());
}

/**
 * @brief Load a snapshot from file and execute it
 * @param fileName Path to snapshot file
 * @retval Value Result from execution, or Error
 */
inline Value loadFromFile(const String& fileName)
{
	return load(fileGetContent(fileName));
}

/** @} */

} // namespace Snapshot

/**
 * @brief Perform memory garbage collection
 */
inline void gc(bool maximumEffort = false)
{
	jerry_gc(maximumEffort ? JERRY_GC_PRESSURE_HIGH : JERRY_GC_PRESSURE_LOW);
}

} // namespace Jerryscript
