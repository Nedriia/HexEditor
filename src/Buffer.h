//
// Created by arthu on 17/07/2026.
//

#ifndef HEXEDITOR_BUFFER_H
#define HEXEDITOR_BUFFER_H
#include <cstdint>
#include <memory>

namespace MemoryMap
{
	constexpr long long MEMORY_SIZE_LIMIT = LLONG_MAX;
}

class Buffer
{
	public:
		Buffer();

		int LoadFromFile( const char* sPathFile );

		template<typename AdressType>
		uint8_t ReadAtAdress( const AdressType iAdress ) const
		{
			if( iAdress >= m_iSize )
			{
				std::cout << "ERROR::ADRESS_INVALID" << std::endl;
				return 0xFF;
			}

			return m_pBuffer[ iAdress ];
		}

		template<typename AdressType>
		void SetValueAtAdress( const AdressType iAdress,uint8_t iValue )
		{
			if( iAdress < 0 || iAdress >= m_iSize )
			{
				std::cout << "ERROR::ADRESS_INVALID" << std::endl;
				return;
			}

			memcpy( &m_pBuffer[ iAdress ],&iValue,sizeof( iValue ) );
		}

		long long GetSize() const { return m_iSize; }
		uint8_t* Get() { return m_pBuffer.get(); }

	private:
		std::unique_ptr<uint8_t[]> m_pBuffer;
		long long m_iSize;
};


#endif //HEXEDITOR_BUFFER_H
