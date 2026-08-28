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

Buffer* HexEditor::m_pBuffer = nullptr;

HexEditor::HexEditor()
	: m_iAdressSelected ( UINT16_MAX )
	 ,m_bIsEditing( false )
{
}

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

void HexEditor::DisplayDebugText()
{
	if( m_pBuffer == nullptr )
		return;

	std::ostringstream oss;
	for( int i = 0; i < m_pBuffer->GetSize(); ++i )
	{
		if( i % 16 == 0 )
			oss << ( i != 0 ? "\n" : "" ) << FormatDebug( "%06x ",i );
		oss << FormatDebug( "%02X ",m_pBuffer->ReadAtAdress( i ) );
	}
	std::cout << oss.str() << std::endl;
}

void HexEditor::FillDataToProcess( int iStart,int iEnd )
{
	if( m_pBuffer == nullptr )
		return;

	for( int i = iStart; i < iEnd; ++i )
	{
		uint16_t iAdress = i * m_oVisualVariable.iBytesPerLine;
		if( iAdress >= 0x8000 )
		{
			m_oDataFormat[ i - iStart ].m_aAdress = "";
			continue;
		}

		std::string output( 7,'\0');
		std::snprintf( &output[ 0 ], output.size(),"0X%04X",iAdress);
		m_oDataFormat[ i - iStart ].m_aAdress = output;

		if( m_oDataFormat[ i - iStart ].m_aHexData.empty() )
			m_oDataFormat[ i - iStart ].m_aHexData.resize( m_oVisualVariable.iBytesPerLine );

		uint8_t* value = m_pBuffer->Get() + ( iAdress );
		std::string hex( 3,'\0' );
		for( int k = 0; k < m_oVisualVariable.iBytesPerLine; ++k )
		{
			std::snprintf( &hex[ 0 ],hex.size(),"%02X", *( value + k ) );
			m_oDataFormat[ i - iStart ].m_aHexData[ k ] = hex;
		}
	}
	m_oVisualVariable.m_iSize = iEnd - iStart;
	m_oVisualVariable.m_iStart = iStart;

	if( m_iAdressSelected != UINT16_MAX && m_bIsEditing )
	{
		uint16_t iStartAdress = iStart * m_oVisualVariable.iBytesPerLine;
		if( m_iAdressSelected < iStartAdress ||
			m_iAdressSelected > iStartAdress + ( m_oVisualVariable.m_iSize * m_oVisualVariable.iBytesPerLine ) )
		{
			m_bIsEditing = false;
		}
	}
}