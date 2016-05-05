#include "stdafx.h"


#include "glsCV.h"


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

	glsCopyInit();
	glsConvertInit();
	glsDrawInit();
	glsBasicOperationInit();
	glsMergeInit();
	glsFftInit();

	GL_CHECK_ERROR();

	return window;
}

//-----------------------------------------------------------------------------
//Terminate glsFft
void glsCvTerminate(void){

	glsCopyTerminate();
	glsConvertTerminate();
	glsDrawTerminate();
	glsBasicOperationTerminate();
	glsMergeTerminate();
	glsFftTerminate();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}


