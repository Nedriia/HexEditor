//
// Created by arthu on 17/07/2026.
//

#include "HexEditor.h"
#include <stdarg.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <cstring>
#include "Buffer.h"

HexEditor::~HexEditor()
{
}

std::string HexEditor::FormatDebug( const char* sFormat,... )
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

void HexEditor::FormatData( Buffer& oBuffer,int iStartIndex,int iEndIndex,int iBytesPerLine )
{
	//TODO : Should add offset outside of viewport
	for( int i = iStartIndex; i < iEndIndex; ++i )
	{
		uint8_t iIndex = i - iStartIndex;
		memcpy( m_oDataFormat[i].m_aHexData,oBuffer.Get() + ( i * iBytesPerLine ),iBytesPerLine);

		m_oDataFormat[ i ].m_aAdress = i * iBytesPerLine + iStartIndex;
	}
}

void HexEditor::DisplayDebugText( const Buffer& oBuffer )
{
	std::ostringstream oss;
	for( int i = 0; i < oBuffer.GetSize(); ++i )
	{
		if( i % 16 == 0 )
			oss << ( i != 0 ? "\n" : "" ) << FormatDebug( "%06x ",i );
		oss << FormatDebug( "%02X ", oBuffer.ReadAtAdress( i ) );
	}
	std::cout << oss.str() << std::endl;
}
