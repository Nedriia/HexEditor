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
	m_pWindow = glfwCreateWindow( 1600,600,"Hex Editor",nullptr,nullptr );
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

	if( ImGui::Begin( "Hex Editor",nullptr ) )
	{
		static int iBytesPerLine = 32;
		ImGui::SliderInt( "Bytes per line",&iBytesPerLine,2,32 );

		static ImVec2 vLabelSize = { 0,0 };
		if( ImGui::BeginListBox( "#",ImVec2( -FLT_MIN,80 * ImGui::GetTextLineHeightWithSpacing() ) ) )
		{
			ImGuiListClipper clipper;
			clipper.Begin( ( oBuffer.GetSize() / iBytesPerLine ),ImGui::GetTextLineHeightWithSpacing() );
			clipper.Step();
			
			if( m_iStartIndex != clipper.DisplayStart )
			{
				FormatData( oBuffer,clipper.DisplayStart,clipper.DisplayEnd,iBytesPerLine );
				m_iStartIndex = clipper.DisplayStart;
			}

			for( int i = 0; i < m_oDataFormat.m_aHexData.size(); ++i )
			{
				vLabelSize.x = 0.f;
				vLabelSize.y = 0.f;
				char byteBuffer[ 16 ];
				snprintf( byteBuffer,sizeof( byteBuffer ),"0X%04X :",i * iBytesPerLine + m_iStartIndex );
				ImGui::Text( byteBuffer );
				ImGui::SameLine();
				for( int j = 0; j < m_oDataFormat.m_aHexData[ i ].size(); ++j )
				{
					ImGui::PushID( i * iBytesPerLine + m_iStartIndex + j );
					char byteBuffer[ 4 ];
					snprintf( byteBuffer,sizeof( byteBuffer ),"%02X ",m_oDataFormat.m_aHexData[ i ][ j ] );
					if( vLabelSize.x == 0.0f && vLabelSize.y == 0.0f )
						vLabelSize = ImGui::CalcTextSize( byteBuffer,nullptr,true );
					ImGui::Selectable( byteBuffer,false,0,vLabelSize );
					ImGui::SameLine();
					if( j == iBytesPerLine / 2 )
					{
						ImGui::Text( "|" );
						ImGui::SameLine();
					}
					ImGui::PopID();
				}
				ImGui::Text( " || " );
				ImGui::SameLine();
				vLabelSize.x = 0.f;
				vLabelSize.y = 0.f;
				for( int j = 0; j < m_oDataFormat.m_aHexData[ i ].size(); ++j )
				{
					ImGui::PushID( i * iBytesPerLine + m_iStartIndex + j );
					char byteBuffer[ 4 ];
					if( m_oDataFormat.m_aHexData[ i ][ j ] < 33 || m_oDataFormat.m_aHexData[ i ][ j ] > 126 )
					{
						byteBuffer[ 0 ] = '.';
						byteBuffer[ 1 ] = '\0';
					}
					else
						snprintf( byteBuffer,sizeof( byteBuffer ),"%c",m_oDataFormat.m_aHexData[ i ][ j ] );
					if( vLabelSize.x == 0.0f && vLabelSize.y == 0.0f )
						vLabelSize = ImGui::CalcTextSize( byteBuffer,nullptr,true );
					ImGui::Selectable( byteBuffer,false,0,vLabelSize );
					ImGui::SameLine();
					if( j == iBytesPerLine / 2 )
					{
						ImGui::Text( "|" );
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
	ImGui::End();

	auto end = _STD chrono::high_resolution_clock::now();
	_STD cout << _STD chrono::duration<double,_STD milli>( end - start ).count() << " ms " << _STD endl;
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

void HexEditor_ImGUI::FormatData( Buffer& oBuffer,int iStartIndex,int iEndIndex,int iBytesPerLine )
{
	m_oDataFormat.m_aHexData.resize( iEndIndex - iStartIndex );
	for( int i = iStartIndex; i < iEndIndex; ++i )
	{
		m_oDataFormat.m_aHexData[i - iStartIndex].resize(32);
		memcpy( m_oDataFormat.m_aHexData[ i - iStartIndex ].data(),oBuffer.Get() + ( i * iBytesPerLine ),iBytesPerLine);
	}
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
