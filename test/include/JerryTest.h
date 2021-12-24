#pragma once

#define DEBUG_BUILD 1

#include <Jerryscript.h>
#include <SmingTest.h>

void printValue(const String& name, const JS::Value& value);

inline void initContext()
{
#ifdef JERRY_EXTERNAL_CONTEXT
	extern JS::Context globalContext;
	globalContext.select();
#endif
	JS::initialise();
}
