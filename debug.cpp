/*

	debug.cpp

	Copyright (c) 2012, PARASITE NETWORK AT CYPHER.NU

	GNU General Public License 3

*/


#include "debug.h"
#include <cstdarg>
#include <cstdio>

#ifndef NDEBUG

void debugf(char const * format, ...)
{
   va_list args;
   va_start(args, format);
   std::vprintf(format, args);
   va_end(args);
   std::printf("\n");
}

#else

void debugf(char const * format, ...)
{

}

#endif