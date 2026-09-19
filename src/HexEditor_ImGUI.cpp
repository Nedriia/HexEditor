//
// Created by arthu on 17/07/2026.
//

#include "HexEditor_ImGUI.h"
#include <cstdio>
#include <vector>
#include <bitset>

#include "Buffer.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui_internal.h>

#include <sstream>
#include <chrono>

#define NULL_DATA_COLOR IM_COL32( 75,75,75,255 )
#define CHANGE_DATA_COLOR IM_COL32( 255,0,0,255 )
#define DEFAULT_DATA_COLOR IM_COL32( 255,255,255,180 )

static void glfw_error_callback( int error,const char* description )
{
	fprintf( stderr,"GLFW Error %d: %s\n",error,description );
}

HexEditor_ImGUI::HexEditor_ImGUI()
	: m_pWindow( nullptr )
	 ,m_bScrollToFocus( false )
{
}

HexEditor_ImGUI::~HexEditor_ImGUI()
{
}

int HexEditor_ImGUI::Init( GLFWwindow* mainWindow )
{
	m_pWindow = mainWindow;

	InitImGUI();

	glfwSetWindowUserPointer( m_pWindow,this );
	glfwSetCharCallback( m_pWindow,character_callback );

	return 0;
}

void HexEditor_ImGUI::InitImGUI()
{
	IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGui::StyleColorsLight();

	ImGuiStyle& style = ImGui::GetStyle();
	style.FontSizeBase = 20.0f;
	style.FontScaleDpi = 1.0f;
	style.ScaleAllSizes( style.FontScaleDpi );
}

void HexEditor_ImGUI::VisualVariable::SetSizes( const float fDPI_Scale,const float fItemSpacing )
{
	if( fDPIScale == fDPI_Scale )
		return;

	fFontChar					= ImGui::CalcTextSize( "F" ).x + 1.0f;
	fFontHex					= ImGui::CalcTextSize( "FF" ).x + 1.0f;
	fFontHeight					= ImGui::CalcTextSize( "F" ).y + 1.0f;
	fFontAdress					= ImGui::CalcTextSize( "FFFFFFFFFFF" ).x + 1.0f;
	fSpaceHex					= fFontHex + ( 3.5f * fDPI_Scale );
	fMidSpaceHex				= fFontHex + ( 15.0f * fDPI_Scale );
	fSpaceASCII					= fFontChar + ( 1.5f * fDPI_Scale );
	fHeightNewLine				= 15.0f * fDPI_Scale;
	iHalfCol					= iBytesPerLine / 2.0f;
	fDPIScale					= fDPI_Scale;
	fTitleHeight				= ImGui::GetTextLineHeightWithSpacing();
	fFooterHeight				= ( 25.f * fDPI_Scale ) + fItemSpacing + ImGui::GetFrameHeightWithSpacing() * 1;
	fFooterHeightExtend			= ( 12.5f * fDPI_Scale ) + fItemSpacing + ImGui::GetFrameHeightWithSpacing() * 4;

	fXPosStartASCII				= fFontAdress + ( iBytesPerLine * fSpaceHex ) + fMidSpaceHex;
}

void HexEditor_ImGUI::Update()
{
	auto start = std::chrono::high_resolution_clock::now();

	glfwPollEvents();
	static double iDurationMs;
	char titleBuffer[ 128 ];
	std::snprintf( titleBuffer,sizeof( titleBuffer ),"Hex Editor (%.2f ms)###HexEditorWindow",iDurationMs );
	if( ImGui::Begin( titleBuffer,nullptr ) )
	{
		SelectAddrToEdit();

		UpdateWithDrawList();
		DrawOptions();
	}

	ImGui::End();

	auto end = std::chrono::high_resolution_clock::now();
	iDurationMs = std::chrono::duration<double,std::milli>( end - start ).count();
}

void HexEditor_ImGUI::UpdateWithDrawList()
{
	ImGuiStyle& style = ImGui::GetStyle();
	m_oVisualVariable.SetSizes( style.FontScaleDpi,style.ItemSpacing.y );

	ImGui::BeginChild( "##scrolling",ImVec2( 0,m_pBuffer && ( m_oVisualVariable.OptShowDataPreview && m_iAdressSelected < m_pBuffer->GetSize() ) ? -m_oVisualVariable.fFooterHeightExtend : -m_oVisualVariable.fFooterHeight ),false,ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav );
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 window_pos = ImGui::GetWindowPos();
	draw_list->AddLine( ImVec2( window_pos.x + m_oVisualVariable.fXPosStartASCII,window_pos.y ),ImVec2( window_pos.x + m_oVisualVariable.fXPosStartASCII,window_pos.y + 9999 ),ImGui::GetColorU32( ImGuiCol_Border ) );
	ImVec2 pos = { window_pos.x, window_pos.y };

	const int line_total_count = m_pBuffer ? ( m_pBuffer->GetSize() / m_oVisualVariable.iBytesPerLine ) : 0;

	if( m_bScrollToFocus )
	{
		int selectedLine = ( m_iAdressSelected / m_oVisualVariable.iBytesPerLine );
		float fDirection = m_oVisualVariable.m_iStart < selectedLine ? 1.0f : 0.0f;
		selectedLine += fDirection;

		float selectedY = selectedLine * m_oVisualVariable.fHeightNewLine;
		if( fDirection == 0.0f )
			selectedY += m_oVisualVariable.fTitleHeight - m_oVisualVariable.fHeightNewLine;
		ImGui::SetScrollFromPosY( ImGui::GetCursorStartPos().y + selectedY,fDirection );
		m_bScrollToFocus = false;
	}

	ImGuiListClipper clipper;
	clipper.Begin( line_total_count,m_oVisualVariable.fHeightNewLine );

	while( clipper.Step() && m_pBuffer )
	{
		if( m_oVisualVariable.m_iStart != clipper.DisplayStart || m_oVisualVariable.m_iSize != ( clipper.DisplayEnd - clipper.DisplayStart ) )
			FillDataToProcess( clipper.DisplayStart,clipper.DisplayEnd );

		for( int line_i = clipper.DisplayStart; line_i < clipper.DisplayEnd; line_i++ )
		{
			int iIndexData = line_i - m_oVisualVariable.m_iStart;
			if( m_oDataFormat[ iIndexData ].m_aAdress.empty() )//TEMP FIX
				break;

			draw_list->AddText( pos,ImGui::GetColorU32( ImGuiCol_TabHovered ),m_oDataFormat[ iIndexData ].m_aAdress.c_str() );
			pos.x += m_oVisualVariable.fFontAdress;

			for( int n = 0; n < m_oVisualVariable.iBytesPerLine; ++n )
			{
				if( m_oVisualVariable.OptGreyOutZeroes && ( *( m_pBuffer->Get() + ( line_i * m_oVisualVariable.iBytesPerLine ) + n ) ) == 0 )
					draw_list->AddText( pos,ImGui::ColorConvertFloat4ToU32( ImVec4( 0.40f,0.40f,0.40f,1.00f ) ),"00" );
				else
					draw_list->AddText( pos,ImGui::GetColorU32( ImGuiCol_Text ),m_oDataFormat[ iIndexData ].m_aHexData[ n ].c_str() );

				if( n + 1 == m_oVisualVariable.iHalfCol )
					pos.x += m_oVisualVariable.fMidSpaceHex;
				else
					pos.x += m_oVisualVariable.fSpaceHex;
			}
			pos.x = window_pos.x + m_oVisualVariable.fXPosStartASCII + m_oVisualVariable.fFontHex;

			if( m_oVisualVariable.OptShowAscii )
			{
				//ASCII
				for( int n = 0; n < m_oVisualVariable.iBytesPerLine; ++n )
				{
					uint8_t* it = m_pBuffer->Get() + ( line_i * m_oVisualVariable.iBytesPerLine ) + n;
					ImGui::SameLine( n == 0 ? m_oVisualVariable.fSpaceASCII : 0 );

					char display_c = ( ( *it ) < 32 || ( *it ) >= 128 ) ? '.' : ( *it );
					draw_list->AddText( pos,ImGui::GetColorU32( ImGuiCol_Text ),&display_c,&display_c + 1 );

					pos.x += m_oVisualVariable.fSpaceASCII;
				}
			}

			pos.y += m_oVisualVariable.fHeightNewLine;
			pos.x = window_pos.x;
		}
	}

	ImGui::EndChild();
	ImGui::Separator();

	DrawAddrSelected( draw_list,window_pos.x,window_pos.y );
	ImGui::PushItemWidth( ( 12 * m_oVisualVariable.fDPIScale ) * 10 + style.FramePadding.x * 2.0f );
	if( ImGui::DragInt( "##cols",&m_oVisualVariable.iBytesPerLine,0.2f,4,32,"%d cols" ) )
	{
		m_oVisualVariable.iHalfCol = m_oVisualVariable.iBytesPerLine / 2;
		m_oVisualVariable.fXPosStartASCII = m_oVisualVariable.fFontAdress + ( m_oVisualVariable.iBytesPerLine * m_oVisualVariable.fSpaceHex ) + m_oVisualVariable.fMidSpaceHex;
		if( m_oVisualVariable.iBytesPerLine < 1 )
			m_oVisualVariable.iBytesPerLine = 1;

		FillDataToProcess( clipper.DisplayStart,clipper.DisplayEnd );
	}
	ImGui::SameLine();
	if ( m_pBuffer )
	{
		const char* format_range = "Range " "%08llX..%08llX";
		ImGui::Text( format_range,0, m_pBuffer->GetSize() - 1 );
		ImGui::SameLine();
	}
	if( ImGui::DragScalar( "##",ImGuiDataType_S64,&m_iAdressSelected,0.2f,NULL,NULL,"%08llX" ) )
	{
		if( m_pBuffer && m_iAdressSelected >= 0 && m_iAdressSelected < m_pBuffer->GetSize() )
		{
			ImGui::BeginChild( "##scrolling" );
			ImGui::SetScrollFromPosY( ImGui::GetCursorStartPos().y + ( m_iAdressSelected / m_oVisualVariable.iBytesPerLine ) * m_oVisualVariable.fHeightNewLine );
			ImGui::EndChild();
		}
		else
		{
			m_iAdressSelected = LLONG_MAX;
		}
	}

	ImGui::DragFloat( "UI Scale##DPI",&style.FontScaleDpi,0.05f,0.6f,2.0f,"%f" );
	ImGui::PopItemWidth();
	ImGui::Separator();

	if( m_pBuffer && m_oVisualVariable.OptShowDataPreview && m_iAdressSelected < m_pBuffer->GetSize() )
	{
		uint8_t iValue = *( m_pBuffer->Get() + m_iAdressSelected );

		char aBuffer[ 24 ];
		std::snprintf( aBuffer,sizeof( aBuffer ),"DEC : %i",iValue );

		ImGui::Text( aBuffer );

		std::snprintf( aBuffer,sizeof( aBuffer ),"HEX : %02X",iValue );
		ImGui::Text( aBuffer );

		ImGui::Text( "Binary : %s",std::bitset<8>( iValue ).to_string().c_str() );
	}
}

void HexEditor_ImGUI::Quit()
{
	if( m_pWindow != nullptr )
		ImGui::DestroyContext();

	m_pWindow = nullptr;
}

void HexEditor_ImGUI::SelectAddrToEdit()
{
	long long iAdress = m_iAdressSelected;

	if( ImGui::IsMouseClicked( 0 ) )
	{
		//Determine if the click is in the window and if it's on data
		ImVec2 mouse_pos = ImGui::GetMousePos();
		ImVec2 window_pos = ImGui::GetWindowPos();
		ImVec2 window_size = ImGui::GetWindowSize();

		float fFooterHeight = m_oVisualVariable.fFooterHeight;
		if( m_oVisualVariable.OptShowDataPreview )
			fFooterHeight = m_oVisualVariable.fFooterHeightExtend;

		if( mouse_pos.x >= window_pos.x && mouse_pos.x <= window_pos.x + window_size.x &&
			mouse_pos.y >= window_pos.y + m_oVisualVariable.fTitleHeight && mouse_pos.y <= window_pos.y + window_size.y - fFooterHeight - m_oVisualVariable.fTitleHeight * 0.5f )
		{
			//Now check if the mouse is on data
			float gridStartX = window_pos.x + m_oVisualVariable.fFontAdress;
			float gridStartY = window_pos.y + m_oVisualVariable.fTitleHeight;

			float relativeX = mouse_pos.x - gridStartX;
			float relativeY = mouse_pos.y - gridStartY;

			if( relativeX >= 0 && relativeY >= 0 )
			{
				int hoveredCol = -1;
				int hoveredLine = static_cast< int >( relativeY / m_oVisualVariable.fHeightNewLine );

				float firstHalfWidth = m_oVisualVariable.iHalfCol * m_oVisualVariable.fSpaceHex;
				float secondHalfStartX = gridStartX + ( ( m_oVisualVariable.iHalfCol - 1 ) * m_oVisualVariable.fSpaceHex ) + m_oVisualVariable.fMidSpaceHex;

				if( relativeX < firstHalfWidth )
				{
					hoveredCol = static_cast< int >( relativeX / m_oVisualVariable.fSpaceHex );
				}
				else if( mouse_pos.x < window_pos.x + ( m_oVisualVariable.fXPosStartASCII - m_oVisualVariable.fFontHex ) )
				{
					float relativeXSecond = mouse_pos.x - secondHalfStartX;
					int secondHalfCol = static_cast< int >( relativeXSecond / m_oVisualVariable.fSpaceHex );
					hoveredCol = m_oVisualVariable.iHalfCol + secondHalfCol;
				}
				else if( mouse_pos.x < window_pos.x + ( m_oVisualVariable.fXPosStartASCII + m_oVisualVariable.fFontHex ) + ( m_oVisualVariable.fSpaceASCII * m_oVisualVariable.iBytesPerLine ) )
				{
					float relativeXSecond = mouse_pos.x - ( window_pos.x + m_oVisualVariable.fXPosStartASCII + m_oVisualVariable.fFontHex );
					int secondHalfCol = static_cast< int >( relativeXSecond / m_oVisualVariable.fSpaceASCII );
					hoveredCol = secondHalfCol;
				}

				if( hoveredCol != -1 )
				{
					if( hoveredLine > 0 )
						iAdress = ( m_oVisualVariable.m_iStart * m_oVisualVariable.iBytesPerLine ) + m_oVisualVariable.iBytesPerLine * hoveredLine;
					else
						iAdress = m_oVisualVariable.m_iStart * m_oVisualVariable.iBytesPerLine;

					iAdress += hoveredCol;

					SetAdressSelection( iAdress );
				}
			}
		}
	}

	if( ImGui::IsMouseClicked( 1 ) )
		ImGui::OpenPopup( "context" );

	int line = 0,col = 0;
	if( ImGui::IsKeyPressed( ImGuiKey_UpArrow ) ) { line--; }
	else if( ImGui::IsKeyPressed( ImGuiKey_DownArrow ) ) { line++; }
	else if( ImGui::IsKeyPressed( ImGuiKey_LeftArrow ) ) { col--; }
	else if( ImGui::IsKeyPressed( ImGuiKey_RightArrow ) ) { col++; }

	if( line != 0 || col != 0 )
	{
		int Hoverline = 0;
		if( line != 0 )
		{
			Hoverline = ( ( iAdress - ( m_oVisualVariable.m_iStart * m_oVisualVariable.iBytesPerLine ) ) / m_oVisualVariable.iBytesPerLine ) + line;

			if( Hoverline > 0 )
			{
				iAdress = m_oVisualVariable.iBytesPerLine * Hoverline;
				iAdress = ( m_oVisualVariable.m_iStart * m_oVisualVariable.iBytesPerLine ) + iAdress;

				iAdress += ( m_iAdressSelected % m_oVisualVariable.iBytesPerLine );
			}
			else
			{
				if( m_oVisualVariable.m_iStart == 0 )
					iAdress %= m_oVisualVariable.iBytesPerLine;
				else
					iAdress += line * m_oVisualVariable.iBytesPerLine;
			}
		}
		if( col != 0 )
		{
			int hoverCol = ( m_iAdressSelected % m_oVisualVariable.iBytesPerLine );
			if( iAdress + col < ( m_oVisualVariable.m_iStart * m_oVisualVariable.iBytesPerLine ) )
				m_bScrollToFocus = true;
			iAdress += col;

			if( hoverCol >= m_oVisualVariable.iBytesPerLine - 1 && col > 0 )
				Hoverline = ( ( iAdress - ( m_oVisualVariable.m_iStart * m_oVisualVariable.iBytesPerLine ) ) / m_oVisualVariable.iBytesPerLine );
		}

		if( Hoverline <= -1 || Hoverline >= m_oVisualVariable.m_iSize )
			m_bScrollToFocus = true;

		SetAdressSelection( iAdress );
	}
}

void HexEditor_ImGUI::DrawAddrSelected( ImDrawList* draw_list,const float fWindowPosX,const float fWindowPosY )
{
	if( m_iAdressSelected != LLONG_MAX )
	{
		//Check if current Addr selected is in current viewport
		int line = m_iAdressSelected / m_oVisualVariable.iBytesPerLine;
		if( line < m_oVisualVariable.m_iStart || line >( m_oVisualVariable.m_iStart + m_oVisualVariable.m_iSize ) )
			return;

		line = ( m_iAdressSelected - ( m_oVisualVariable.m_iStart * m_oVisualVariable.iBytesPerLine ) ) / m_oVisualVariable.iBytesPerLine;
		int col = m_iAdressSelected % m_oVisualVariable.iBytesPerLine;

		ImVec2 vStartPos = { fWindowPosX, fWindowPosY };
		vStartPos.y += line * m_oVisualVariable.fHeightNewLine;
		vStartPos.x += m_oVisualVariable.fFontAdress;

		if( col >= m_oVisualVariable.iHalfCol )
			vStartPos.x += ( ( m_oVisualVariable.iHalfCol - 1 ) * m_oVisualVariable.fSpaceHex + m_oVisualVariable.fMidSpaceHex ) + ( ( col - m_oVisualVariable.iHalfCol ) * m_oVisualVariable.fSpaceHex );
		else
			vStartPos.x += col * m_oVisualVariable.fSpaceHex;

		draw_list->AddRectFilled( vStartPos,ImVec2( vStartPos.x + m_oVisualVariable.fFontHex,vStartPos.y + m_oVisualVariable.fFontHeight ),ImGui::GetColorU32( ImGuiCol_DockingPreview ) );

		if( !m_oVisualVariable.OptShowAscii )
			return;

		ImVec2 vAsciiPos;
		vAsciiPos.x = fWindowPosX + m_oVisualVariable.fXPosStartASCII + m_oVisualVariable.fFontHex + ( col * m_oVisualVariable.fSpaceASCII );
		vAsciiPos.y = vStartPos.y;
		draw_list->AddRectFilled( vAsciiPos,ImVec2( vAsciiPos.x + m_oVisualVariable.fFontChar,vAsciiPos.y + m_oVisualVariable.fFontHeight ),ImGui::GetColorU32( ImGuiCol_DockingPreview ) );
	}
}

void HexEditor_ImGUI::DrawOptions()
{
	if( ImGui::BeginPopup( "context" ) )
	{
		ImGui::Checkbox( "Show Data Preview",&m_oVisualVariable.OptShowDataPreview );
		ImGui::Checkbox( "Show Ascii",&m_oVisualVariable.OptShowAscii );
		ImGui::Checkbox( "Grey out zeroes",&m_oVisualVariable.OptGreyOutZeroes );

		ImGui::EndPopup();
	}
}

void HexEditor_ImGUI::framebuffer_size_callback( GLFWwindow* m_pWindow,int width,int height )
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport( 0,0,width,height );
}

void HexEditor_ImGUI::character_callback( GLFWwindow* window,unsigned int codepoint )
{
	char c = std::toupper( static_cast< char >( codepoint ) );

	if( ( c >= '0' && c <= '9' ) || ( c >= 'A' && c <= 'F' ) )
	{
		HexEditor_ImGUI* pInstance = static_cast< HexEditor_ImGUI* >( glfwGetWindowUserPointer( window ) );
		if( pInstance == nullptr )
			return;

		if( ImGui::GetActiveID() != 0 )
		{
			ImGui::GetIO().AddInputCharacter( codepoint );
		}
		else
		{
			long long iStartAdress = pInstance->m_oVisualVariable.m_iStart * pInstance->m_oVisualVariable.iBytesPerLine;
			if( pInstance->m_iAdressSelected < iStartAdress ||
				pInstance->m_iAdressSelected > iStartAdress + ( pInstance->m_oVisualVariable.m_iSize * pInstance->m_oVisualVariable.iBytesPerLine ) )
			{
				return;
			}

			int line = pInstance->m_iAdressSelected / pInstance->m_oVisualVariable.iBytesPerLine;
			int col = pInstance->m_iAdressSelected % pInstance->m_oVisualVariable.iBytesPerLine;

			line -= pInstance->m_oVisualVariable.m_iStart;
			if( pInstance->m_oDataFormat[ line ].m_aHexData[ col ].size() >= 2 )
			{
				pInstance->m_bIsEditing = true;
				pInstance->m_oDataFormat[ line ].m_aHexData[ col ] = c;
			}
			else
			{
				pInstance->m_oDataFormat[ line ].m_aHexData[ col ] += c;
				//Update memory with new value
				pInstance->m_pBuffer->SetValueAtAdress( pInstance->m_iAdressSelected,std::stoi( pInstance->m_oDataFormat[ line ].m_aHexData[ col ],nullptr,16 ) );
				//pInstance->m_iAdressSelected++;
				pInstance->m_bIsEditing = false;
			}
		}
	}
}