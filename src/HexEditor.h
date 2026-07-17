//
// Created by arthu on 17/07/2026.
//

#ifndef HEXEDITOR_HEXEDITOR_H
#define HEXEDITOR_HEXEDITOR_H

#include <string>
#ifdef IMGUI_ENABLED
#include "HexEditor_ImGUI.h"
#elif defined(QT_ENABLED)
#include "HexEditor_QT.h"
#endif

class Buffer;
class HexEditor 
{
	public:

		std::string Format( const char* sFormat,... );
		void Display( const Buffer& oBuffer );
		void DisplayDebugText( const Buffer& oBuffer );

	private:

#ifdef IMGUI_ENABLED
	HexEditor_ImGUI m_oImGUIEditor;
#elif defined(QT_ENABLED)
	HexEditor_QT m_oQTEditor;
#endif
};


#endif //HEXEDITOR_HEXEDITOR_H
