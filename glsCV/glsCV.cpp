/*
Copyright (c) 2016, oasi-adamay
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of glsCV nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "stdafx.h"


#include "glsCV.h"

//#ifdef _DEBUG
#if 0
#include "Timer.h"
#define _TMR_(...)  Timer tmr(__VA_ARGS__)
#else
#define _TMR_(...)
#endif


//-----------------------------------------------------------------------------
//global 

/*!----------------------------------------------------------------------------
@brief
gl error string(gluErrorString compatible)
@return
-----------------------------------------------------------------------------*/
static const char* glErrStr_GL_NO_ERROR = "GL_NO_ERROR";
static const char* glErrStr_GL_INVALID_ENUM = "GL_INVALID_ENUM";
static const char* glErrStr_GL_INVALID_VALUE = "GL_INVALID_VALUE";
static const char* glErrStr_GL_INVALID_OPERATION = "GL_INVALID_OPERATION";
static const char* glErrStr_GL_OUT_OF_MEMORY = "GL_OUT_OF_MEMORY";
static const char* glErrStr_GL_UNDEFINED = "GL_UNDEFINED";

/*!----------------------------------------------------------------------------
@brief
get Error string (gluErrorString compatible)
@return
-----------------------------------------------------------------------------*/
const char* glsErrorString(GLenum err){
	switch (err){
	case(GL_NO_ERROR) : return glErrStr_GL_NO_ERROR;
	case(GL_INVALID_ENUM) : return glErrStr_GL_INVALID_ENUM;
	case(GL_INVALID_VALUE) : return glErrStr_GL_INVALID_VALUE;
	case(GL_INVALID_OPERATION) : return glErrStr_GL_INVALID_OPERATION;
	case(GL_OUT_OF_MEMORY) : return glErrStr_GL_OUT_OF_MEMORY;
	}
	return glErrStr_GL_UNDEFINED;
}


//-----------------------------------------------------------------------------
// GLFWでエラーとなったときに呼び出される関数
static void glfw_error_callback_func(int error, const char* description){
	std::cout << "GLFW Error: " << description << std::endl;
}


//=============================================================================
//-----------------------------------------------------------------------------
//Initialtize glsCV lib
GLFWwindow* glsCvInit(const int _width, const int _height){
	GLFWwindow* window = 0;

	glfwSetErrorCallback(glfw_error_callback_func);

	bool offscreen = !(_width > 0 && _height > 0);
	int width;
	int height;
	if (offscreen){
		width = 1;
		height = 1;
	}
	else{
		width = _width;
		height = _height;
	}

	// Initialise GLFW
	if (!glfwInit())
	{
		cerr<< "Failed to initialize GLFW" << endl;
		GLS_Assert(0);
	}

	//-----------------------------------------------------------------------------
	glfwWindowHint(GLFW_SAMPLES, 4);

	// GL3.3 Core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (offscreen){
		glfwWindowHint(GLFW_VISIBLE, 0);	//オフスクリーン
	}

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(width, height, "glsCV", NULL, NULL);
	if (window == NULL){
		cerr << "Failed to open GLFW window." << endl;
		glfwTerminate();
		GLS_Assert(0);
	}
	glfwMakeContextCurrent(window);

#if defined _WIN32
	// Initialize GLEW
	glewExperimental = GL_TRUE;			///!!!! important for core profile // コアプロファイルで必要となります
	if (glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW." << endl;
		glfwTerminate();
		GLS_Assert(0);
	}
#endif


	glGetError();	// clear error flag!

	{
		cout << "GL_VENDOR:" << glGetString(GL_VENDOR) << endl;
		cout << "GL_RENDERER:" << glGetString(GL_RENDERER) << endl;
		cout << "GL_VERSION:" << glGetString(GL_VERSION) << endl;
		cout << "GL_SHADING_LANGUAGE_VERSION:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	}


	GL_CHECK_ERROR();

	return window;
}

//-----------------------------------------------------------------------------
//Terminate glsFft
void glsCvTerminate(void){


	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}


