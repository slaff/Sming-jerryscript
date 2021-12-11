#pragma once

#ifndef DEBUG_VERBOSE_LEVEL
#define DEBUG_VERBOSE_LEVEL 2
#endif

#include <Jerryscript.h>
#include <SmingTest.h>

void printValue(const String& name, const JS::Value& value);
