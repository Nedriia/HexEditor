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
		~HexEditor_ImGUI();

		int  Init( GLFWwindow* mainWindow );
		void InitImGUI();
		void Update();
		void UpdateWithDrawList();

		void Quit();
	private:
		GLFWwindow*	m_pWindow;

		void SelectAddrToEdit();
		void DrawAddrSelected( ImDrawList* draw_list, const float fWindowPosX,const float fWindowPosY );
		void DrawOptions();
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

			if( m_bIsEditing && iAdress != m_iAdressSelected )
			{
				//Get back to the original value
				int lineH = m_iAdressSelected / m_oVisualVariable.iBytesPerLine;
				int colH = m_iAdressSelected % m_oVisualVariable.iBytesPerLine;

				uint8_t* value = m_pBuffer->Get() + m_iAdressSelected;
				std::string hex( 3,'\0' );
				std::snprintf( &hex[ 0 ],hex.size(),"%02X",*( value ) );

				if( lineH < 0 || lineH > m_oVisualVariable.m_iStart + m_oVisualVariable.m_iSize || lineH < m_oVisualVariable.m_iStart )
				{
					std::cout << "ERROR::LINE_INDEX" << std::endl;
					return;
				}
				lineH -= m_oVisualVariable.m_iStart;

				if( colH < 0 || colH > m_oVisualVariable.iBytesPerLine )
				{
					std::cout << "ERROR::COL_INDEX" << std::endl;
					return;
				}

				m_oDataFormat[ lineH ].m_aHexData[ colH ] = hex;
				m_bIsEditing = false;
			}

			m_iAdressSelected = iAdress;
		}

		bool m_bScrollToFocus;
};


#endif //HEXEDITOR_HEXEDITOR_IMGUI_H