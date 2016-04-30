#include "stdafx.h"


#include "glsCV.h"


//-----------------------------------------------------------------------------
//global 

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
		assert(0);
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
		assert(0);
	}
	glfwMakeContextCurrent(window);

#if defined _WIN32
	// Initialize GLEW
	glewExperimental = GL_TRUE;			///!!!! important for core profile // コアプロファイルで必要となります
	if (glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW." << endl;
		glfwTerminate();
		assert(0);
	}
#endif


	{
		cout << "GL_VENDOR:" << glGetString(GL_VENDOR) << endl;
		cout << "GL_RENDERER:" << glGetString(GL_RENDERER) << endl;
		cout << "GL_VERSION:" << glGetString(GL_VERSION) << endl;
		cout << "GL_SHADING_LANGUAGE_VERSION:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	}

	glsConvertInit();
	glsDrawInit();
	glsBasicOperationInit();
	glsFftInit();

	return window;
}

//-----------------------------------------------------------------------------
//Terminate glsFft
void glsCvTerminate(void){

	glsConvertTerminate();
	glsDrawTerminate();
	glsBasicOperationTerminate();
	glsFftTerminate();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}


