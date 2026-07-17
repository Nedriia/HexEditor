//
// Created by arthu on 17/07/2026.
//

#include "HexEditor.h"
#include <stdarg.h>
#include <vector>
#include <sstream>
#include <iostream>
#include "Buffer.h"

HexEditor::~HexEditor()
{
}

std::string HexEditor::Format( const char* sFormat,... )
{
	va_list args;
	va_start( args,sFormat );
	size_t len = std::vsnprintf( nullptr,0,sFormat,args );
	va_end( args );

	std::vector<char> vec( len + 1 );
	va_start( args,sFormat );
	std::vsnprintf( &vec[ 0 ],len + 1,sFormat,args );
	va_end( args );

	return &vec[ 0 ];
}

void HexEditor::DisplayDebugText( const Buffer& oBuffer )
{
	std::ostringstream oss;
	for( int i = 0; i < oBuffer.GetSize(); ++i )
	{
		if( i % 16 == 0 )
			oss << ( i != 0 ? "\n" : "" ) << Format( "%06x ",i );
		oss << Format( "%02X ", oBuffer.ReadAtAdress( i ) );
	}
	std::cout << oss.str() << std::endl;
}
