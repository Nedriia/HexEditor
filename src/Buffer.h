//
// Created by arthu on 17/07/2026.
//

#ifndef HEXEDITOR_BUFFER_H
#define HEXEDITOR_BUFFER_H
#include <cstdint>
#include <memory>
#include <climits>
#include <iostream>

# ifndef  _WIN32
#include <string.h>
#endif

namespace MemoryMap
{
	constexpr long long MEMORY_SIZE_LIMIT = LLONG_MAX;
}

class Buffer
{
	public:
		Buffer();

		int LoadFromFile( const char* sPathFile );
		template <class T, std::size_t N>
		int LoadFromMemory( const std::array<T, N>& aData )
		{
			m_aDataBuffer = std::make_unique<uint8_t[]>( N );

			for (std::size_t i = 0; i < N; ++i)
				m_aDataBuffer[i] = static_cast<uint8_t>( aData[i] );

			m_iSize = N;

			return 0;
		}

		template<typename AdressType>
		uint8_t ReadAtAdress( const AdressType iAdress ) const
		{
			if( iAdress >= m_iSize )
			{
				std::cout << "ERROR::ADRESS_INVALID" << std::endl;
				return 0xFF;
			}

			return m_aDataBuffer[ iAdress ];
		}

		template<typename AdressType>
		void SetValueAtAdress( const AdressType iAdress,uint8_t iValue )
		{
			if( iAdress < 0 || iAdress >= m_iSize )
			{
				std::cout << "ERROR::ADRESS_INVALID" << std::endl;
				return;
			}

			memcpy( &m_aDataBuffer[ iAdress ],&iValue,sizeof( iValue ) );
		}

		long long GetSize() const { return m_iSize; }
		uint8_t* Get() { return m_aDataBuffer.get(); }

	private:
		std::unique_ptr<uint8_t[]> m_aDataBuffer;
		long long m_iSize;
};


#endif //HEXEDITOR_BUFFER_H
