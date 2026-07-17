//
// Created by arthu on 17/07/2026.
//

#include "HexEditor_ImGUI.h"
#include <cstdio>
#include <iostream>

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
	glfwSetErrorCallback( glfw_error_callback );
	if( !glfwInit() )
		return -1;

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR,3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR,3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE );

	DISABLE_SPECIFIC_LEAK_DETECTION();
	m_pWindow = glfwCreateWindow( 500,200,"Hex Editor",nullptr,nullptr );
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

	return 0;
}

void HexEditor_ImGUI::Render( const Buffer& oBuffer, bool& bQuit )
{
	if( !glfwWindowShouldClose( m_pWindow ) )
	{
		if( glfwGetKey( m_pWindow,GLFW_KEY_ESCAPE ) == GLFW_PRESS )
			glfwSetWindowShouldClose( m_pWindow,true );

		glfwPollEvents();
	}
	else
		bQuit = true;

	glfwSwapBuffers( m_pWindow );
}

void HexEditor_ImGUI::Quit()
{
	if ( m_pWindow )
		glfwDestroyWindow( m_pWindow );
	glfwTerminate();

	m_pWindow = nullptr;
}

void HexEditor_ImGUI::framebuffer_size_callback( GLFWwindow* m_pWindow, int width, int height )
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport( 0,0,width,height );
}
