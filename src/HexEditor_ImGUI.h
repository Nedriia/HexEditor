//
// Created by arthu on 17/07/2026.
//

#ifndef HEXEDITOR_HEXEDITOR_IMGUI_H
#define HEXEDITOR_HEXEDITOR_IMGUI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "HexEditor.h"
#include <vector>

class HexEditor_ImGUI : public HexEditor
{
	public:
		HexEditor_ImGUI();
		~HexEditor_ImGUI();

		int Init() override;
		int InitWindow();
		void InitImGUI();
		void Update( Buffer& oBuffer );
		void Render( Buffer& oBuffer, bool& bQuit ) override;
		void FormatData( Buffer& oBuffer, int iStartIndex, int iEndIndex, int iBytesPerLine );

		void Quit();
	private:
		GLFWwindow*	m_pWindow;
		static void framebuffer_size_callback( GLFWwindow* m_pWindow,int width,int height );

		int m_iStartIndex;

		typedef struct
		{
			std::vector<std::vector<uint8_t>> m_aHexData;
			std::vector<std::string> m_aAscii;
		} MemoryDataFormatted;

		MemoryDataFormatted m_oDataFormat;
};


#endif //HEXEDITOR_HEXEDITOR_IMGUI_H
