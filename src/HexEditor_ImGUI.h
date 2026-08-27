//
// Created by arthu on 17/07/2026.
//

#ifndef HEXEDITOR_HEXEDITOR_IMGUI_H
#define HEXEDITOR_HEXEDITOR_IMGUI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "HexEditor.h"

struct ImDrawList;
class HexEditor_ImGUI : public HexEditor
{
	public:
		HexEditor_ImGUI();
		~HexEditor_ImGUI();

		int Init() override;
		int InitWindow();
		void InitImGUI();
		void Update();
		void UpdateWithDrawList();
		void Render( Buffer& oBuffer, bool& bQuit ) override;

		void Quit();
	private:
		GLFWwindow*	m_pWindow;

		void SelectAddrToEdit();
		void DrawAddrSelected( ImDrawList* draw_list, const float fWindowPosX,const float fWindowPosY );
		void DrawOptions();
		static void framebuffer_size_callback(  GLFWwindow* m_pWindow,int width,int height );
		static void character_callback( GLFWwindow* window,unsigned int codepoint );
		uint16_t	m_iAdressSelected;
};


#endif //HEXEDITOR_HEXEDITOR_IMGUI_H