//
// Created by arthu on 17/07/2026.
//

#ifndef HEXEDITOR_BUFFER_H
#define HEXEDITOR_BUFFER_H
#include <cstdint>
#include <memory>

namespace MemoryMap
{
	constexpr uint16_t MEMORY_SIZE = 0xFFFF;
}

class Buffer
{
	public:
		Buffer();

		int LoadFromFile( const char* sPathFile );

		uint8_t ReadAtAdress( const uint16_t iAdress ) const;
		void SetValueAtAdress( const uint16_t iAdress, uint8_t iValue );
		uint16_t GetSize() const { return m_iSize; }

	private:
		std::unique_ptr<uint8_t[]> m_pBuffer;
		uint16_t m_iSize;
};


#endif //HEXEDITOR_BUFFER_H
