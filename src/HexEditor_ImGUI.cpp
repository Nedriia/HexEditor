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

static void glfw_error_callback( int error,const char* description )
{
	fprintf( stderr,"GLFW Error %d: %s\n",error,description );
}

HexEditor_ImGUI::HexEditor_ImGUI()
	: m_pWindow( nullptr )
	, m_iStartIndex( -1 )
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
	m_pWindow = glfwCreateWindow( 3440,1440,"Hex Editor",nullptr,nullptr );
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

	glfwSwapInterval(1); //Put 0 in case you want to uncap the speed
	return 0;
}

void HexEditor_ImGUI::InitImGUI()
{
	float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor( glfwGetPrimaryMonitor() ); // Valid on GLFW 3.3+ only

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes( main_scale );        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL( m_pWindow,true );
	ImGui_ImplOpenGL3_Init( "#version 330" );
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
		//UpdateWithText( oBuffer );
		UpdateWithDrawList( oBuffer );
	}
	ImGui::End();

	auto end = std::chrono::high_resolution_clock::now();
	iDurationMs = std::chrono::duration<double,std::milli>( end - start ).count();
}

void HexEditor_ImGUI::UpdateWithText( Buffer& oBuffer )
{
	static int iBytesPerLine = 32;
	ImGui::SliderInt( "Bytes per line",&iBytesPerLine,2,32 );

	if( ImGui::BeginListBox( "#",ImVec2( -FLT_MIN,100 * ImGui::GetTextLineHeight() ) ) )
	{
		ImGuiListClipper clipper;
		clipper.Begin( ( oBuffer.GetSize() / iBytesPerLine ),ImGui::GetTextLineHeight() );
		clipper.Step();

		if( m_iStartIndex != clipper.DisplayStart /*|| clipper.DisplayEnd - clipper.DisplayStart != 8*/ )
		{
			m_iStartIndex = clipper.DisplayStart;
			FormatData( oBuffer,m_iStartIndex,clipper.DisplayEnd,iBytesPerLine );
		}

		static int iIndexSelected = -1;
		for( int i = m_iStartIndex; i < clipper.DisplayEnd; ++i )
		{
			ImGui::Text( "0X%04X	: ",m_oDataFormat[ i ].m_aAdress );
			ImGui::SameLine();
			for( int j = 0; j < iBytesPerLine; ++j )
			{
				ImGui::PushID( i * iBytesPerLine + m_iStartIndex + j );
				ImGui::Text( "%02X ",m_oDataFormat[ i ].m_aHexData[ j ],nullptr );
				ImGui::SameLine();
				if( j + 1 == iBytesPerLine / 2 )
				{
					ImGui::Text( " " );
					ImGui::SameLine();
				}
				ImGui::PopID();
			}

			ImGui::Text( " | " );
			ImGui::SameLine();

			for( int j = 0; j < iBytesPerLine; ++j )
			{
				ImGui::PushID( i * iBytesPerLine + m_iStartIndex + j );
				if( m_oDataFormat[ i ].m_aHexData[ j ] < 33 || m_oDataFormat[ i ].m_aHexData[ j ] > 126 )
					ImGui::TextEx( "." );
				else
					ImGui::Text( "%c",m_oDataFormat[ i ].m_aHexData[ j ] );

				ImGui::SameLine();
				if( j + 1 == iBytesPerLine / 2 )
				{
					ImGui::Text( " " );
					ImGui::SameLine();
				}
				ImGui::PopID();
			}

			ImGui::NewLine();
		}

		clipper.End();
		ImGui::EndListBox();
	}
}

void HexEditor_ImGUI::UpdateWithDrawList( Buffer& oBuffer )
{
	static int iBytesPerLine = 32;

	float footer_height = 10.f;
	ImGuiStyle& style = ImGui::GetStyle();
	const float height_separator = style.ItemSpacing.y;
	footer_height += height_separator + ImGui::GetFrameHeightWithSpacing() * 1;

	ImGui::BeginChild( "##scrolling",ImVec2( 0,-footer_height ),false,ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav );
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	
	ImVec2 window_pos = ImGui::GetWindowPos();
	float PosAsciiStart = 800;
	draw_list->AddLine( ImVec2( window_pos.x + PosAsciiStart - 20 ,window_pos.y ),ImVec2( window_pos.x + PosAsciiStart - 20,window_pos.y + 9999 ),ImGui::GetColorU32( ImGuiCol_Border ) );

	const int line_total_count = ( oBuffer.GetSize() / iBytesPerLine );
	ImGuiListClipper clipper;
	clipper.Begin( line_total_count,ImGui::GetTextLineHeight() );

	while( clipper.Step() )
	{
		uint16_t iStartAdress = 0;
		int iStartIndex = 0;
		for( int line_i = clipper.DisplayStart; line_i < clipper.DisplayEnd; line_i++ )
		{
			ImGui::Text( "%04X:",iStartAdress + line_i * iBytesPerLine );
			for( int n = iStartIndex; n < iBytesPerLine + iStartIndex; ++n )
			{
				uint8_t* it = oBuffer.Get() + ( line_i * iBytesPerLine ) + n;
				if( n == iBytesPerLine / 2 )
				{
					ImGui::SameLine();
					ImGui::Text( " " );
				}

				ImGui::SameLine();
				if( ( *it ) == 0 )
					ImGui::TextDisabled( "00" );
				else
					ImGui::Text( "%02X",( *it ) );
			}

			//ASCII
			for( int n = 0; n < iBytesPerLine; ++n )
			{
				uint8_t* it = oBuffer.Get() + ( line_i * iBytesPerLine ) + n;
				ImGui::SameLine( n == 0 ? PosAsciiStart : 0 );
				if( ( *it ) < 33 || ( *it ) > 126 )
					ImGui::TextEx( "." );
				else
					ImGui::Text( "%c",( *it ) );
			}
		}
	}

	ImGui::EndChild();
	ImGui::Separator();

	ImGui::SetNextItemWidth( 10 * 7 + style.FramePadding.x * 2.0f );
	if( ImGui::DragInt( "##cols",&iBytesPerLine,0.2f,4,32,"%d cols" ) ) { if( iBytesPerLine < 1 ) iBytesPerLine = 1; }
	ImGui::SameLine();
	size_t base_display_addr = 0X0000;
	const char* format_range = "Range " "%04X..%04X";
	ImGui::Text( format_range,base_display_addr,base_display_addr + oBuffer.GetSize() );

	//Dec
	//Hex
	//Binary
}

void HexEditor_ImGUI::Render( Buffer& oBuffer, bool& bQuit )
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

void HexEditor_ImGUI::framebuffer_size_callback( GLFWwindow* m_pWindow, int width, int height )
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport( 0,0,width,height );
}
