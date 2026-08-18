//
// Created by arthu on 17/07/2026.
//

#ifndef HEXEDITOR_HEXEDITOR_IMGUI_H
#define HEXEDITOR_HEXEDITOR_IMGUI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "HexEditor.h"

class ImDrawList;
class HexEditor_ImGUI : public HexEditor
{
	public:
		HexEditor_ImGUI();
		~HexEditor_ImGUI();

		int Init() override;
		int InitWindow();
		void InitImGUI();
		void Update( Buffer& oBuffer );
		void UpdateWithDrawList( Buffer& oBuffer );
		void Render( Buffer& oBuffer, bool& bQuit ) override;

		void Quit();
	private:
		GLFWwindow*	m_pWindow;

		void SelectAddrToEdit();
		void DrawAddrSelected( ImDrawList* draw_list, const float fWindowPosX,const float fWindowPosY );
		static void framebuffer_size_callback(  GLFWwindow* m_pWindow,int width,int height );
		uint16_t	m_iAdressSelected;
		int			m_iStartIndex;

		typedef struct
		{
			void SetSizes( const float fDPI_Scale, const float fItemSpacing );

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
};


#endif //HEXEDITOR_HEXEDITOR_IMGUI_H
