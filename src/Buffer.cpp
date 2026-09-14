//
// Created by arthu on 17/07/2026.
//

#include "Buffer.h"
#include <fstream>
#include <iomanip>

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
			if( size <= 0 || size > MemoryMap::MEMORY_SIZE_LIMIT )
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