//
// Created by arthu on 17/07/2026.
//
#include "Buffer.h"

#ifdef IMGUI_ENABLED
#include "HexEditor_ImGUI.h"
#elif QT_ENABLED
#include "HexEditor_QT.h"
#endif


int main( int argc, char *argv[] )
{
	Buffer oBuffer;
	if( argc <= 1 || oBuffer.LoadFromFile( argv[ 1 ] ) != 0 )
		return -1;

#ifdef IMGUI_ENABLED
	HexEditor_ImGUI oEditor;
#elif QT_ENABLED
	HexEditor_QT oEditor;
#endif
	if( oEditor.Init() != 0 )
		return -1;

	bool bQuit = false;
	while( !bQuit )
		oEditor.Render( oBuffer, bQuit );

	return 0;
}