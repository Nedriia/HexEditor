//
// Created by arthu on 17/07/2026.
//

#ifndef HEXEDITOR_BUFFER_H
#define HEXEDITOR_BUFFER_H
#include <cstdint>
#include <memory>
#include <climits>
#include <iostream>
#include <vector>

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

		int LoadFromMemory( std::vector<uint8_t>& array )
		{
			m_iSize = array.size();
			m_aDataBuffer = array.data();

			return 0;
		}

		template <class T, std::size_t N>
		int LoadFromMemory( T(&array)[N] )
		{
			m_iSize = array.size();
			m_aDataBuffer = array.data();

			return 0;
		}

		template <class T, std::size_t N>
		int LoadFromMemory( const std::array<T, N>& aData )
		{
			m_iSize = array.size();
			m_aDataBuffer = array.data();

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
		uint8_t* Get() { return m_aDataBuffer; }

	private:
		uint8_t* m_aDataBuffer;
		long long m_iSize;
};


#endif //HEXEDITOR_BUFFER_H
