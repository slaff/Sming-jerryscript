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
 * @brief JavaScriptVM for Sming
 */
class VirtualMachine
{
public:
	/**
	* @brief Initializes the JavaScript VM
	*/
	VirtualMachine(jerry_init_flag_t flags = JERRY_INIT_EMPTY);

	~VirtualMachine();

	/*
	 * @brief Parses the JavaScript code and prepares it for execution
	 * @retval bool true on success
	 */
	bool eval(const String& jsCode);

	/**
	 * @brief Loads a snapshot from file and executes it.
	 * @param fileName Path to snapshot file
	 * @retval bool true on success
	 */
	bool loadFromFile(const String& fileName)
	{
		return load(fileGetContent(fileName));
	}

	/**
	 * @name Load a snapshot into the virtual machine
	 * @param snapshot Points to snapshot content
	 * @param snapshotSize Number of bytes in snapshot
	 * @retval bool true on success
	 * @{
	 */
	bool load(const uint32_t* snapshot, size_t snapshotSize);

	bool load(const String& snapshot)
	{
		return load(reinterpret_cast<const uint32_t*>(snapshot.c_str()), snapshot.length());
	}
	/** @} */

	/**
	 * @brief Runs a specified JavaScript function
	 * @param functionName Name of function to run
	 * @retval bool true if the function exists and was called successfully
	 */
	bool runFunction(const String& functionName);

	/**
	 * @brief Register an external function so it may be called from javascript
	 * @param name Name of the function
	 * @param handler The function handler, see Function.h for details
	 * @param bool true on success
	 */
	bool registerFunction(const String& name, jerry_external_handler_t handler)
	{
		auto res = global().setProperty(name, ExternalFunction(handler));
		return !res.isError();
	}

	/**
	 * @brief Unregister an external function
	 * @param name Name of the function
	 * @param bool true on success
	 */
	bool unregisterFunction(const String& name)
	{
		return global().removeProperty(name);
	}

	/**
	 * @brief Perform memory garbage collection
	 */
	void gc(bool maximumEffort = false)
	{
		jerry_gc(maximumEffort ? JERRY_GC_PRESSURE_HIGH : JERRY_GC_PRESSURE_LOW);
	}
};

} // namespace Jerryscript
