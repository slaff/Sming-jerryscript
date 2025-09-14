/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * jerry-port.cpp
 *
 * @author Nov 2021 - Slavey Karadzhov <slav@attachix.com>
 */

#include <stdarg.h>
#include <sys/time.h>

#include "include/Jerryscript/Except.h"
#include "include/Jerryscript/Context.h"
#include <jerry-core/include/jerryscript-port.h>
#include <jerry-core/include/jerryscript-core.h>
#include "include/jerry_port_vm.h"
#include <debug_progmem.h>
#include <Platform/RTC.h>

struct jerry_port_watchdog_t jerry_port_watchdog;

void jerry_port_watchdog_set_period(uint32_t milliseconds)
{
	jerry_port_watchdog.period = uint64_t(milliseconds) * HW_TIMER2_CLK / 1000U;
}

/**
 * Provide log message implementation for the engine.
 */
void jerry_port_log(const char* msg)
{
	m_puts(msg);
}

/**
 * Default implementation of jerry_port_current_time.
 */
double jerry_port_current_time()
{
	return RTC.getRtcNanoseconds() / 1e9;
}

/**
 * Default implementation of jerry_port_get_local_time_zone_adjustment. Uses the 'tm_gmtoff' field
 * of 'struct tm' (a GNU extension) filled by 'localtime_r' if available on the
 * system, does nothing otherwise.
 *
 * @return offset between UTC and local time at the given unix timestamp, if
 *         available. Otherwise, returns 0, assuming UTC time.
 */
double jerry_port_get_local_time_zone_adjustment(double unix_ms, /**< ms since unix epoch */
												 bool is_utc)	/**< is the time above in UTC? */
{
	// TODO
	return 0.0;
}

/**
 * Get the current system time in UTC.
 *
 * This port function is called by jerry-core when JERRY_BUILTIN_DATE is enabled.
 * It can also be used in the implementing application to initialize the random number generator.
 *
 * @return milliseconds since Unix epoch
 */
int32_t jerry_port_local_tza(double unix_ms)
{
	// TODO
	return 0;
}

/**
 * Default implementation of jerry_port_fatal.
 */
void jerry_port_fatal(jerry_fatal_code_t code)
{
	Jerryscript::Except::raise(code);
}

/**
 * Default module resolver.
 *
 * @return a module object if resolving is successful, an error otherwise
 */
jerry_value_t jerry_port_module_resolve(const jerry_value_t specifier, /**< module specifier string */
										const jerry_value_t referrer,  /**< parent module */
										void* user_p)				   /**< user data */
{
	(void)specifier;
	(void)referrer;
	(void)user_p;

	return jerry_error(JERRY_ERROR_REFERENCE, {});
}

size_t jerry_port_context_alloc(size_t context_size)
{
	return context_size;
}

void jerry_port_context_free(void)
{
}

/**
 * Get the current context.
 *
 * @return the pointer to the current context
 */
jerry_context_t* jerry_port_context_get(void)
{
	return Jerryscript::Context::getCurrent();
}

/**
 * Init the program
 */
void jerry_port_init(void)
{
}
