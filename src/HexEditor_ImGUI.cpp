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

void HexEditor_ImGUI::Update( const Buffer& oBuffer )
{
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

		if( ImGui::BeginListBox( "#",ImVec2( -FLT_MIN,48 * ImGui::GetTextLineHeightWithSpacing() ) ) )
		{
			ImGuiListClipper clipper;
			clipper.Begin( ( oBuffer.GetSize() / iBytesPerLine ),ImGui::GetTextLineHeightWithSpacing() );

			while( clipper.Step() )
			{
				for( int line = clipper.DisplayStart; line < clipper.DisplayEnd; ++line )
				{
					ImGui::PushID( line );

					int iMemoryIndex = line * iBytesPerLine;

					char buffer[ 8 ];
					snprintf( buffer,sizeof( buffer ),"0x%04X : ",iMemoryIndex );

					ImGui::Text( "%s", buffer );
					ImGui::SameLine();

					std::vector<std::string> sAscii;
					for( int i = 0; i < iBytesPerLine; ++i )
					{
						ImGui::PushID( i );
						if( i == iBytesPerLine / 2 )
						{
							ImGui::Text( "|" );
							ImGui::SameLine();
						}

						char byteBuffer[ 4 ];
						uint8_t iValue = oBuffer.ReadAtAdress( line * iBytesPerLine + i );
						ImGui::PushStyleColor( ImGuiCol_Text,iValue == 0 ? NULL_DATA_COLOR : DEFAULT_DATA_COLOR );

						snprintf( byteBuffer,sizeof( byteBuffer ),"%02X ",iValue );

						ImGuiWindow* window = ImGui::GetCurrentWindow();
						ImVec2 label_size = ImGui::CalcTextSize( byteBuffer,nullptr,true );

						ImGui::Selectable( byteBuffer, false,0,label_size );
						ImGui::SameLine();

						if( iValue != 0 )
						{
							char charRepres[ 4 ] {};
							if( iValue < 33 || iValue > 126 )
								snprintf( charRepres,sizeof( charRepres ),"." );
							else
								snprintf( charRepres,sizeof( charRepres ),"%c",iValue );
							sAscii.push_back( charRepres );
						}

						ImGui::PopID();
						ImGui::PopStyleColor();
					}

					for( int i = 0; i < sAscii.size(); ++i )
					{
						ImGui::PushID( i );
						ImGuiWindow* window = ImGui::GetCurrentWindow();
						ImVec2 label_size = ImGui::CalcTextSize( sAscii[i].c_str(),nullptr,true);
						ImGui::SameLine();
						ImGui::Selectable( sAscii[ i ].c_str(),false,0,label_size );
						ImGui::PopID();
					}
					ImGui::PopID();
				}
			}
			clipper.End();
			ImGui::EndListBox();
		}
	}
	ImGui::End();
}

void HexEditor_ImGUI::Render( const Buffer& oBuffer, bool& bQuit )
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
