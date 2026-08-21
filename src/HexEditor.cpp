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
	CleanMemory();
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

void HexEditor::DisplayDebugText( const Buffer& oBuffer )
{
	std::ostringstream oss;
	for( int i = 0; i < oBuffer.GetSize(); ++i )
	{
		if( i % 16 == 0 )
			oss << ( i != 0 ? "\n" : "" ) << FormatDebug( "%06x ",i );
		oss << FormatDebug( "%02X ",oBuffer.ReadAtAdress( i ) );
	}
	std::cout << oss.str() << std::endl;
}

void HexEditor::FillDataToProcess( Buffer& oDataBuffer,int iStart,int iEnd )
{
	CleanMemory();

	for( int i = iStart; i < iEnd; ++i )
	{
		char* aBuffer = new char[ 8 ];
		uint16_t iAdress = i * m_oVisualVariable.iBytesPerLine;
		uint8_t* value = oDataBuffer.Get() + ( iAdress );

		snprintf( aBuffer, sizeof( aBuffer ), "0X%04X:", iAdress );
		m_oDataFormat[ i - iStart ].m_aAdress = aBuffer;

		for( int k = 0; k < m_oVisualVariable.iBytesPerLine; ++k )
		{
			aBuffer = new char[ 8 ];
			uint8_t* data = value + k;

			snprintf( aBuffer,sizeof( aBuffer ),"%02X",( *data ) );
			m_oDataFormat[ i - iStart ].m_aHexData[ k ] = aBuffer;
		}
	}
	m_oDataFormat->m_iStart = iStart;
	m_oDataFormat->m_iSize = iEnd - iStart;
}

void HexEditor::CleanMemory()
{
	int iSize = m_oDataFormat->m_iSize;
	for( int i = 0; i < iSize; ++i )
	{
		for( int k = 0; k < m_oVisualVariable.iBytesPerLine; ++k )
		{
			delete[] m_oDataFormat[ i ].m_aHexData[ k ];
			m_oDataFormat[ i ].m_aHexData[ k ] = nullptr;
		}
		delete[] m_oDataFormat[ i ].m_aAdress;
		m_oDataFormat[ i ].m_aAdress = nullptr;
		m_oDataFormat[ i ].m_iSize = 0;
		m_oDataFormat[ i ].m_iStart = 0;
	}
}