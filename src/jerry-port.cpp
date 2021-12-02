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

#include <jerry-core/include/jerryscript-port.h>
#include <debug_progmem.h>
#include <Platform/RTC.h>

/**
 * Provide console message implementation for the engine.
 */
void jerry_port_console(const char* format, /**< format string */
						...)				/**< parameters */
{
	va_list args;
	va_start(args, format);
	m_vprintf(format, args);
	va_end(args);
} /* jerry_port_console */

/**
 * Provide log message implementation for the engine.
 */
void jerry_port_log(jerry_log_level_t level, /**< log level */
					const char* format,		 /**< format string */
					...)					 /**< parameters */
{
	(void)level; /* ignore log level */

	va_list args;
	va_start(args, format);
	/* TODO, uncomment when vprint link is ok */
	/* vprintf (stderr, format, args); */
	va_end(args);
} /* jerry_port_log */

/**
 * Default implementation of jerry_port_get_current_time.
 */
double jerry_port_get_current_time()
{
	return RTC.getRtcNanoseconds() / 1e9;
} /* jerry_port_get_current_time */

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
	return 0.0;
}

static bool abort_on_fail = false;

/**
 * Sets whether 'abort' should be called instead of 'exit' upon exiting with
 * non-zero exit code in the default implementation of jerry_port_fatal.
 */
void jerry_port_default_set_abort_on_fail(bool flag) /**< new value of 'abort on fail' flag */
{
	abort_on_fail = flag;
} /* jerry_port_default_set_abort_on_fail */

/**
 * Check whether 'abort' should be called instead of 'exit' upon exiting with
 * non-zero exit code in the default implementation of jerry_port_fatal.
 *
 * @return true - if 'abort on fail' flag is set,
 *         false - otherwise.
 */
bool jerry_port_default_is_abort_on_fail()
{
	return abort_on_fail;
} /* jerry_port_default_is_abort_on_fail */

/**
 * Default implementation of jerry_port_fatal.
 */
void jerry_port_fatal(jerry_fatal_code_t code)
{
	if(code != 0 && code != ERR_OUT_OF_MEMORY && jerry_port_default_is_abort_on_fail()) {
		abort();
	} else {
		exit(code);
	}
} /* jerry_port_fatal */
