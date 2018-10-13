// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

enum class Direction
{
	// Here are the enumerators
	// These define all the possible values this type can hold
	// Each enumerator is separated by a comma, not a semicolon
	None,
	X,
	Y,
	Z,
	Uniform,
};

enum class Operation
{
	// Here are the enumerators
	// These define all the possible values this type can hold
	// Each enumerator is separated by a comma, not a semicolon
	None,
	Translate,
	Rotate,
	Scale,
};

