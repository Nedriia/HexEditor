//
// Created by arthu on 17/07/2026.
//
#include "Buffer.h"
#include "HexEditor.h"

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

	std::unique_ptr<HexEditor> oEditor;
#ifdef IMGUI_ENABLED
	oEditor = std::make_unique<HexEditor_ImGUI>();
#elif QT_ENABLED
	oEditor = std::make_unique<HexEditor_QT>();
#endif
	if( oEditor == nullptr || oEditor->Init() != 0 )
		return -1;

	bool bQuit = false;
	while( !bQuit )
		oEditor->Render( oBuffer, bQuit );

	return 0;
}