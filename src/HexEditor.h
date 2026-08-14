//
// Created by arthu on 17/07/2026.
//

#ifndef HEXEDITOR_HEXEDITOR_H
#define HEXEDITOR_HEXEDITOR_H

#include <string>

class Buffer;
class HexEditor 
{
	public:
		HexEditor(){};
		virtual ~HexEditor();

		virtual int Init() = 0;
		virtual void Render( Buffer& oBuffer, bool& bQuit ){};
		void FormatData( Buffer& oBuffer,int iStartIndex,int iEndIndex,int iBytesPerLine );

	protected:

		std::string FormatDebug( const char* sFormat,... );

		void DisplayDebugText( const Buffer& oBuffer );

		typedef struct
		{
			uint8_t		m_aHexData[32];
			uint16_t	m_aAdress;
		} MemoryDataFormatted;

		MemoryDataFormatted m_oDataFormat[128];
};


#endif //HEXEDITOR_HEXEDITOR_H
