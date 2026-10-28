//
// Created by arthu on 17/07/2026.
//

#ifndef HEXEDITOR_HEXEDITOR_IMGUI_H
#define HEXEDITOR_HEXEDITOR_IMGUI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Buffer.h"
#include "HexEditor.h"
#include "iostream"

struct ImDrawList;
class HexEditor_ImGUI : public HexEditor
{
	public:
		HexEditor_ImGUI();
		~HexEditor_ImGUI(){};

		int  Init( GLFWwindow* mainWindow );
		void Update();
		void UpdateWithDrawList();

	private:
		void SelectAddrToEdit();
		void DrawAddrSelected( ImDrawList* draw_list, const float fWindowPosX,const float fWindowPosY );
		void DrawOptions();
		static uint8_t char_to_hex( char c );
		static void framebuffer_size_callback(  GLFWwindow* m_pWindow,int width,int height );
		static void character_callback( GLFWwindow* window,unsigned int codepoint );

		template <typename T>
		void SetAdressSelection( const T iAdress )
		{
			if( m_pBuffer == nullptr || iAdress < 0 || iAdress >= m_pBuffer->GetSize() )
			{
				std::cout << "ERROR::ADRESS_INVALID" << std::endl;
				return;
			}

			if( m_sZoneEditing.empty() == false && iAdress != m_iAdressSelected )
			{
				//Get back to the original value
				m_pBuffer->SetValueAtAdress( m_iAdressSelected,std::stoi( m_sZoneEditing,nullptr,16 ) );
				m_sZoneEditing.clear();
			}

			m_iAdressSelected = iAdress;
		}

		bool m_bScrollToFocus;
};


#endif //HEXEDITOR_HEXEDITOR_IMGUI_H