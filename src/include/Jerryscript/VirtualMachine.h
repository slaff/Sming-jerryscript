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

/*
 * @brief Parses the JavaScript code and prepares it for execution
 * @retval bool true on success
 */
bool eval(const String& jsCode);

/**
 * @brief Snapshot management functions
 */
namespace Snapshot
{
/**
 * @name Load a snapshot into the virtual machine
 * @brief Load from memory buffer
 * @param snapshot Points to snapshot content
 * @param snapshotSize Number of bytes in snapshot
 * @retval bool true on success
 * @{
 */
bool load(const uint32_t* snapshot, size_t snapshotSize);

/**
 * @brief Load from String
 */
inline bool load(const String& snapshot)
{
	return load(reinterpret_cast<const uint32_t*>(snapshot.c_str()), snapshot.length());
}

/**
 * @brief Load a snapshot from file and execute it
 * @param fileName Path to snapshot file
 * @retval bool true on success
 */
inline bool loadFromFile(const String& fileName)
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
