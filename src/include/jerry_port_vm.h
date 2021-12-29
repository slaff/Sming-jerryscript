/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * jerry_port_vm.h - Additional Sming port functions used by jerryscript VM
 *
 * @author: Dec 2021 - Mikee47 <mike@sillyhouse.net>
 *
 */

#pragma once

#include <driver/hw_timer.h>
#include <jrt.h>

/**
 * @brief Watchdog state information
 */
struct jerry_port_watchdog_t {
	uint32_t period; ///< Watchdog timeout in hardware timer units
	uint32_t start;  ///< Time of last call to jerry_port_watchdog_reset()
};

extern struct jerry_port_watchdog_t jerry_port_watchdog;

/**
 * @brief Set watchdog period in milliseconds
 */
void jerry_port_watchdog_set_period(uint32_t milliseconds);

/**
 * @brief Reset watchdog timer before calling into jerryscript (load, run, etc)
 */
__forceinline static void jerry_port_watchdog_reset()
{
	jerry_port_watchdog.start = hw_timer2_read();
}

/**
 * @brief Get elapsed watchdog time (in ticks) since last reset
 */
__forceinline static uint32_t jerry_port_watchdog_read()
{
	return hw_timer2_read() - jerry_port_watchdog.start;
}

/**
 * @brief Called by jerryscript VM loop. Generates fatal error on timeout.
 */
__forceinline static bool jerry_port_watchdog_poll()
{
	if(jerry_port_watchdog.period == 0) {
		return true;
	}
	if(jerry_port_watchdog_read() < jerry_port_watchdog.period) {
		return true;
	}
	jerry_fatal(ERR_WATCHDOG_TIMEOUT);
	return false;
}
