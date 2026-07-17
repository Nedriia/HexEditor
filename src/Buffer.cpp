//
// Created by arthu on 17/07/2026.
//

#include "Buffer.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <vector>
#include <sstream>

Buffer::Buffer() 
	: m_pBuffer( nullptr )
	, m_iSize( 0 )
{
}

int Buffer::LoadFromFile( const char* sPathFile )
{
	std::ifstream file( sPathFile,std::ios::binary | std::ios::in | std::ios::ate );
	if( file.is_open() )
	{
		std::streamsize size = file.tellg();
		try
		{
			if( size <= 0 || size > MemoryMap::MEMORY_SIZE )
				throw std::runtime_error( "SIZE_INVALID" );

			m_pBuffer = std::make_unique<uint8_t[]>( static_cast< size_t >( size ) );
			m_iSize = size;

			file.seekg( 0,std::ios::beg );
			file.read( reinterpret_cast< char* >( m_pBuffer.get() ),size );

			if( file.gcount() != size )
				throw std::runtime_error( "READ_SIZE_NOT_CONFORM" );

			file.close();
		}
		catch( std::runtime_error e )
		{
			m_pBuffer.reset();
			m_iSize = 0;
			std::cout << "ERROR::LOADING_" << e.what() << std::endl;
			return -1;
		}
	}
	else
	{
		std::cout << "ERROR::FILE_NOT_FOUND : " << sPathFile << std::endl;
		return -1;
	}
	return 0;
}

uint8_t Buffer::ReadAtAdress ( const uint16_t iAdress ) const
{
	if( iAdress < 0 || iAdress > m_iSize )
	{
		std::cout << "ERROR::ADRESS_INVALID" << std::endl;
		return 0xFF;
	}

	return m_pBuffer[ iAdress ];
}

std::string Buffer::Format( const char* sFormat,... )
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

void Buffer::DisplayDebug()
{
	std::ostringstream oss;
	for( int i = 0; i < m_iSize; ++i )
	{
		if( i != 0 && i % 16 == 0 )
			oss << "\n";
		oss << Format( "%02X ",ReadAtAdress( i ) );
	}
	std::cout << oss.str() << std::endl;
}
