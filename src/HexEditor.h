//
// Created by arthu on 17/07/2026.
//

#ifndef HEXEDITOR_HEXEDITOR_H
#define HEXEDITOR_HEXEDITOR_H

#include <string>
#include <cstdint>

class Buffer;
class HexEditor 
{
	public:
		HexEditor(){};
		virtual ~HexEditor();

		virtual int Init() = 0;
		virtual void Render( Buffer& oBuffer, bool& bQuit ){};

	protected:

		std::string FormatDebug( const char* sFormat,... );

		void DisplayDebugText( const Buffer& oBuffer );

		void FillDataToProcess( Buffer& oBuffer,int iStart,int iEnd );
		typedef struct
		{
			const char*		m_aAdress;
			const char*		m_aHexData[ 32 ];
			int				m_iSize;
			int				m_iStart;
		} MemoryDataFormatted;

		MemoryDataFormatted m_oDataFormat[ 256 ];

		typedef struct
		{
			void SetSizes( const float fDPI_Scale,const float fItemSpacing );

			float fFontChar = 0.0f;
			float fFontHex = 0.0f;
			float fFontHeight = 0.0f;
			float fFontAdress = 0.0f;
			float fXPosStartASCII = 0.0f;
			static int iBytesPerLine;
			float fSpaceHex = 0.0f;
			float fMidSpaceHex = 0.0f;
			float fSpaceASCII = 0.0f;
			float fHeightNewLine = 0.0f;
			int   iHalfCol = 0;
			float fDPIScale = 0.f;
			float fTitleHeight = 0.f;
			float fFooterHeight = 0.f;
		}VisualVariable;
		VisualVariable m_oVisualVariable;
		void CleanMemory();
};


#endif //HEXEDITOR_HEXEDITOR_H
