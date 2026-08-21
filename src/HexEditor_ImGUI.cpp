//
// Created by arthu on 17/07/2026.
//

#include "HexEditor_ImGUI.h"
#include <cstdio>
#include <iostream>
#include <vector>

#include "Buffer.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui_internal.h>

#include <sstream>
#include <chrono>


#ifdef LEAK_DETECTOR
#include <vld.h>
#define ENABLE_GLOBAL_LEAK_DETECTION() VLDGlobalEnable()
#define DISABLE_GLOBAL_LEAK_DETECTION() VLDGlobalDisable()

#define ENABLE_SPECIFIC_LEAK_DETECTION() VLDEnable()
#define DISABLE_SPECIFIC_LEAK_DETECTION() VLDDisable()
#else
#define ENABLE_GLOBAL_LEAK_DETECTION() ((void)0)
#define DISABLE_GLOBAL_LEAK_DETECTION() ((void)0)

#define ENABLE_SPECIFIC_LEAK_DETECTION() ((void)0)
#define DISABLE_SPECIFIC_LEAK_DETECTION() ((void)0)
#endif

#define NULL_DATA_COLOR IM_COL32( 75,75,75,255 )
#define CHANGE_DATA_COLOR IM_COL32( 255,0,0,255 )
#define DEFAULT_DATA_COLOR IM_COL32( 255,255,255,180 )

int HexEditor_ImGUI::VisualVariable::iBytesPerLine = 32;

static void glfw_error_callback( int error,const char* description )
{
	fprintf( stderr,"GLFW Error %d: %s\n",error,description );
}

HexEditor_ImGUI::HexEditor_ImGUI()
	: m_pWindow( nullptr )
	, m_iStartIndex( -1 )
	,m_iAdressSelected( -1 )
{
}

HexEditor_ImGUI::~HexEditor_ImGUI()
{
	Quit();
}

int HexEditor_ImGUI::Init()
{
	if ( InitWindow() != 0 )
		return -1;

	InitImGUI();

	return 0;
}

int HexEditor_ImGUI::InitWindow()
{
	glfwSetErrorCallback( glfw_error_callback );
	if( !glfwInit() )
		return -1;

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR,3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR,3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE );

	DISABLE_SPECIFIC_LEAK_DETECTION();
	m_pWindow = glfwCreateWindow( 2140,980,"Hex Editor",nullptr,nullptr );
	if( m_pWindow == nullptr )
	{
		std::cout << "DISPLAY::FAILED_TO_CREATE_GLFW_WINDOW" << std::endl;
		return -1;
	}
	ENABLE_SPECIFIC_LEAK_DETECTION();

	glfwMakeContextCurrent( m_pWindow );
	glfwSetFramebufferSizeCallback( m_pWindow,HexEditor_ImGUI::framebuffer_size_callback );

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if( !gladLoadGLLoader( ( GLADloadproc )glfwGetProcAddress ) )
	{
		std::cerr << "DISPLAY::GLAD_FAILED_TO_INIT" << std::endl;
		return -1;
	}

	glfwSwapInterval( 1 ); //Put 0 in case you want to uncap the speed
	return 0;
}

void HexEditor_ImGUI::InitImGUI()
{
	float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor( glfwGetPrimaryMonitor() ); // Valid on GLFW 3.3+ only

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();

	ImGuiStyle& style = ImGui::GetStyle();
	//style.ScaleAllSizes( 2.0f );
	style.FontScaleDpi = 2.0f;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL( m_pWindow,true );
	ImGui_ImplOpenGL3_Init( "#version 330" );
}

void HexEditor_ImGUI::VisualVariable::SetSizes( const float fDPI_Scale,const float fItemSpacing )
{
	if( fDPIScale == fDPI_Scale )
		return;

	fFontChar			= ImGui::CalcTextSize( "F" ).x + 1.0f;
	fFontHex			= ImGui::CalcTextSize( "FF" ).x + 1.0f;
	fFontHeight			= ImGui::CalcTextSize( "F" ).y + 1.0f;
	fFontAdress			= ImGui::CalcTextSize( "FFFFFFFF" ).x + 1.0f;
	fSpaceHex			= fFontHex + ( 3.5f * fDPI_Scale );
	fMidSpaceHex		= fFontHex + ( 15.0f * fDPI_Scale );
	fSpaceASCII			= fFontChar + ( 1.5f * fDPI_Scale );
	fHeightNewLine		= 15.0f * fDPI_Scale;
	iHalfCol			= iBytesPerLine / 2.0f;
	fDPIScale			= fDPI_Scale;
	fTitleHeight		= ImGui::GetTextLineHeightWithSpacing();
	fFooterHeight		= ( 25.f * fDPI_Scale ) + fItemSpacing + ImGui::GetFrameHeightWithSpacing() * 1;

	fXPosStartASCII		= fFontAdress + ( iBytesPerLine * fSpaceHex ) + fMidSpaceHex;
}

void HexEditor_ImGUI::Update( Buffer& oBuffer )
{
	auto start = std::chrono::high_resolution_clock::now();

	glfwPollEvents();
	if( glfwGetWindowAttrib( m_pWindow,GLFW_ICONIFIED ) != 0 )
	{
		ImGui_ImplGlfw_Sleep( 10 );
		return;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	static double iDurationMs;
	char titleBuffer[ 128 ];
	std::snprintf( titleBuffer,sizeof( titleBuffer ),"Hex Editor (%.2f ms)###HexEditorWindow",iDurationMs );
	if( ImGui::Begin( titleBuffer,nullptr ) )
	{
		SelectAddrToEdit();
		UpdateWithDrawList( oBuffer );
	}
	
	ImGui::End();

	auto end = std::chrono::high_resolution_clock::now();
	iDurationMs = std::chrono::duration<double,std::milli>( end - start ).count();
}

void HexEditor_ImGUI::UpdateWithDrawList( Buffer& oBuffer )
{
	ImGuiStyle& style = ImGui::GetStyle();
	m_oVisualVariable.SetSizes( style.FontScaleDpi,style.ItemSpacing.y );

	ImGui::BeginChild( "##scrolling",ImVec2( 0,-m_oVisualVariable.fFooterHeight ),false,ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav );
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImVec2 window_pos = ImGui::GetWindowPos();
	draw_list->AddLine( ImVec2( window_pos.x + m_oVisualVariable.fXPosStartASCII,window_pos.y ),ImVec2( window_pos.x + m_oVisualVariable.fXPosStartASCII,window_pos.y + 9999 ),ImGui::GetColorU32( ImGuiCol_Border ) );
	ImVec2 pos = { window_pos.x, window_pos.y };

	const int line_total_count = ( oBuffer.GetSize() / m_oVisualVariable.iBytesPerLine );
	ImGuiListClipper clipper;
	clipper.Begin( line_total_count,ImGui::GetTextLineHeight() );

	while( clipper.Step() )
	{
		uint16_t iStartAdress = 0; //??

		if( m_oDataFormat->m_iStart != clipper.DisplayStart || m_oDataFormat->m_iSize != ( clipper.DisplayEnd - clipper.DisplayStart ) )
			FillDataToProcess( oBuffer, clipper.DisplayStart, clipper.DisplayEnd );

		for( int line_i = 0; line_i < m_oDataFormat->m_iSize; line_i++ )
		{
			draw_list->AddText( pos,ImGui::GetColorU32( ImGuiCol_TabHovered ), m_oDataFormat[line_i].m_aAdress );
			pos.x += ImGui::CalcTextSize( "FFFFFFFF" ).x + 1;

			for( int n = 0; n < m_oVisualVariable.iBytesPerLine; ++n )
			{
				if( ( *( oBuffer.Get() + ( line_i * m_oVisualVariable.iBytesPerLine ) + n ) ) == 0 )
					draw_list->AddText( pos,ImGui::ColorConvertFloat4ToU32( ImVec4( 0.40f,0.40f,0.40f,1.00f ) ),"00" );
				else
					draw_list->AddText( pos,ImGui::GetColorU32( ImGuiCol_Text ),m_oDataFormat[ line_i ].m_aHexData[ n ] );

				if( n + 1 == m_oVisualVariable.iHalfCol )
					pos.x += m_oVisualVariable.fMidSpaceHex;
				else
					pos.x += m_oVisualVariable.fSpaceHex;
			}
			pos.x = window_pos.x + m_oVisualVariable.fXPosStartASCII + m_oVisualVariable.fFontHex;

			//ASCII
			for( int n = 0; n < m_oVisualVariable.iBytesPerLine; ++n )
			{
				uint8_t* it = oBuffer.Get() + ( line_i * m_oVisualVariable.iBytesPerLine ) + n;
				ImGui::SameLine( n == 0 ? m_oVisualVariable.fSpaceASCII : 0 );

				char display_c = ( ( *it ) < 32 || ( *it ) >= 128 ) ? '.' : ( *it );
				draw_list->AddText( pos,ImGui::GetColorU32( ImGuiCol_Text ), &display_c, &display_c + 1 );

				pos.x += m_oVisualVariable.fSpaceASCII;
			}

			pos.y += m_oVisualVariable.fHeightNewLine;
			pos.x = window_pos.x;
		}
	}

	ImGui::EndChild();
	ImGui::Separator();

	DrawAddrSelected( draw_list, window_pos.x, window_pos.y );

	ImGui::SetNextItemWidth( 25 * 7 + style.FramePadding.x * 2.0f );
	if( ImGui::DragInt( "##cols",&m_oVisualVariable.iBytesPerLine,0.2f,4,32,"%d cols" ) ) 
	{ 
		m_oVisualVariable.iHalfCol = m_oVisualVariable.iBytesPerLine / 2;
		m_oVisualVariable.fXPosStartASCII = m_oVisualVariable.fFontAdress + ( m_oVisualVariable.iBytesPerLine * m_oVisualVariable.fSpaceHex ) + m_oVisualVariable.fMidSpaceHex;
		if( m_oVisualVariable.iBytesPerLine < 1 )
			m_oVisualVariable.iBytesPerLine = 1;
	}
	ImGui::SameLine();
	size_t base_display_addr = 0X0000;
	const char* format_range = "Range " "%04X..%04X";
	ImGui::Text( format_range,base_display_addr,base_display_addr + oBuffer.GetSize() );
	ImGui::DragFloat( "UI Scale##DPI",&style.FontScaleDpi,0.05f,0.6f,2.0f, "%f");
	//Dec
	//Hex
	//Binary
}

void HexEditor_ImGUI::Render( Buffer& oBuffer,bool& bQuit )
{
	if( !glfwWindowShouldClose( m_pWindow ) )
	{
		if( glfwGetKey( m_pWindow,GLFW_KEY_ESCAPE ) == GLFW_PRESS )
			glfwSetWindowShouldClose( m_pWindow,true );

		Update( oBuffer );

		glClearColor( 0.f,0.f,0.f,1.f );
		glClear( GL_COLOR_BUFFER_BIT );

		int display_w,display_h;
		glfwGetFramebufferSize( m_pWindow,&display_w,&display_h );
		glViewport( 0,0,display_w,display_h );

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

		glfwSwapBuffers( m_pWindow );
	}
	else
		bQuit = true;
}

void HexEditor_ImGUI::Quit()
{
	if( m_pWindow != nullptr )
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext();

		glfwDestroyWindow( m_pWindow );
	}

	glfwTerminate();
	m_pWindow = nullptr;
}

void HexEditor_ImGUI::SelectAddrToEdit()
{
	if( ImGui::IsMouseClicked( 0 ) )
	{
		//Determine if the click is in the window and if it's on data
		ImVec2 mouse_pos = ImGui::GetMousePos();
		ImVec2 window_pos = ImGui::GetWindowPos();
		ImVec2 window_size = ImGui::GetWindowSize();

		if( mouse_pos.x >= window_pos.x && mouse_pos.x <= window_pos.x + window_size.x &&
			mouse_pos.y >= window_pos.y + m_oVisualVariable.fTitleHeight && mouse_pos.y <= window_pos.y + window_size.y - m_oVisualVariable.fFooterHeight - m_oVisualVariable.fTitleHeight * 0.5f )
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
				else if( relativeX < firstHalfWidth + secondHalfStartX - m_oVisualVariable.fFontAdress )
				{
					float relativeXSecond = mouse_pos.x - secondHalfStartX;
					int secondHalfCol = static_cast< int >( relativeXSecond / m_oVisualVariable.fSpaceHex );
					hoveredCol = m_oVisualVariable.iHalfCol + secondHalfCol;
				}

				if( hoveredCol != -1 )
				{
					uint16_t iAdress = 0;
					if( hoveredLine > 0 )
						iAdress = 32 * hoveredLine;
					iAdress += hoveredCol;
					m_iAdressSelected = iAdress;
					std::cout << hoveredCol << std::endl;
				}
			}
		}
	}
}

void HexEditor_ImGUI::DrawAddrSelected( ImDrawList* draw_list, const float fWindowPosX,const float fWindowPosY )
{
	if( m_iAdressSelected != -1 )
	{
		ImVec2 vStartPos = { fWindowPosX, fWindowPosY };
		uint16_t iStartAdress = 0x0000;
		iStartAdress += m_iAdressSelected; //Check adress limit

		int line = m_iAdressSelected / m_oVisualVariable.iBytesPerLine;
		int col = m_iAdressSelected % m_oVisualVariable.iBytesPerLine;

		vStartPos.y += line * m_oVisualVariable.fHeightNewLine;
		vStartPos.x += m_oVisualVariable.fFontAdress;

		if( col >= m_oVisualVariable.iHalfCol )
			vStartPos.x += ( ( m_oVisualVariable.iHalfCol - 1 ) * m_oVisualVariable.fSpaceHex + m_oVisualVariable.fMidSpaceHex ) + ( ( col - m_oVisualVariable.iHalfCol ) * m_oVisualVariable.fSpaceHex );
		else
			vStartPos.x += col * m_oVisualVariable.fSpaceHex;

		draw_list->AddRectFilled( vStartPos,ImVec2( vStartPos.x + m_oVisualVariable.fFontHex,vStartPos.y + m_oVisualVariable.fFontHeight ),ImGui::GetColorU32( ImGuiCol_DockingPreview ) );

		ImVec2 vAsciiPos;
		vAsciiPos.x = fWindowPosX + m_oVisualVariable.fXPosStartASCII + m_oVisualVariable.fFontHex + ( col * m_oVisualVariable.fSpaceASCII );
		vAsciiPos.y = vStartPos.y;
		draw_list->AddRectFilled( vAsciiPos,ImVec2( vAsciiPos.x + m_oVisualVariable.fFontChar,vAsciiPos.y + m_oVisualVariable.fFontHeight ),ImGui::GetColorU32( ImGuiCol_DockingPreview ) );
	}
}

void HexEditor_ImGUI::framebuffer_size_callback( GLFWwindow* m_pWindow,int width,int height )
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport( 0,0,width,height );
}