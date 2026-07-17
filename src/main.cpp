//
// Created by arthu on 17/07/2026.
//
#include "Buffer.h"

#ifdef LEAK_DETECTOR
	#include <vld.h>
	#define ENABLE_GLOBAL_LEAK_DETECTION() VLDGlobalEnable()
	#define DISABLE_GLOBAL_LEAK_DETECTION() VLDGlobalDisable()

	#define ENABLE_SPECIFIC_LEAK_DETECTION() VLDEnable()
	#define DISABLE_SPECIFIC_LEAK_DETECTION() VLDDisable()
#else
	#define ENABLE_GLOBAL_LEAK_DETECTION() ((void)0)
	#define DISABLE_GLOBAL_LEAK_DETECTION() ((void)0)

	#define ENABLE_SPECIFIC_LEAK_DETECTION() ((void)0)
	#define DISABLE_SPECIFIC_LEAK_DETECTION() ((void)0)
#endif

int main( int argc, char *argv[] )
{
	Buffer oBuffer;
	if( argc <= 1 || oBuffer.LoadFromFile( argv[ 1 ] ) != 0 )
		return -1;

	oBuffer.DisplayDebug();
	return 0;
}