//
// Created by arthu on 17/07/2026.
//

#ifndef HEXEDITOR_HEXEDITOR_IMGUI_H
#define HEXEDITOR_HEXEDITOR_IMGUI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "HexEditor.h"

class HexEditor_ImGUI : public HexEditor
{
	public:
		HexEditor_ImGUI();
		~HexEditor_ImGUI();

		int Init() override;
		int InitWindow();
		void InitImGUI();
		void Update( const Buffer& oBuffer );
		void Render( const Buffer& oBuffer, bool& bQuit ) override;

		void Quit();
	private:
		GLFWwindow*	m_pWindow;
		static void framebuffer_size_callback( GLFWwindow* m_pWindow,int width,int height );
};


#endif //HEXEDITOR_HEXEDITOR_IMGUI_H
