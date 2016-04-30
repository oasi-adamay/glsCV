#include "stdafx.h"


#include "glsDraw.h"
#include "Timer.h"

//-----------------------------------------------------------------------------
// glsDraw shader
class glsShaderDrawBase : public glsShaderBase
{
public:
//	glsShaderDrawBase(void);

	//attribute location
	GLuint position;

	//uniform  location
	GLuint texSrc;
	GLuint f_scl;
};


//-----------------------------------------------------------------------------
// glsDraw shader
class glsShaderDraw : public glsShaderDrawBase
{
public:
	glsShaderDraw(void);

};

//-----------------------------------------------------------------------------
// glsDraw shader
class glsShaderDrawU : public glsShaderDrawBase
{
public:
	glsShaderDrawU(void);

};



//-----------------------------------------------------------------------------
//global 
glsShaderDraw* shaderDraw = 0;
glsShaderDrawU* shaderDrawU = 0;

void glsDrawInit(void){
	shaderDraw = new glsShaderDraw();
	shaderDrawU = new glsShaderDrawU();

}

void glsDrawTerminate(void){
	delete shaderDraw;
	delete shaderDrawU;
}




//-----------------------------------------------------------------------------
//glsShaderDraw
glsShaderDraw::glsShaderDraw(void)
	:glsShaderDrawBase()
{

	const char vertexShaderCode[] = 
"#version 330 core\n"
"layout (location = 0)in  vec2 position;\n"
"void main(void)\n"
"{\n"
"   gl_Position  = vec4(position,0.0,1.0);\n"
"}\n"
;


	const char fragmentShaderCode[] = 
"#version 330 core\n"
"precision highp float;\n"
"uniform sampler2DRect	texSrc;\n"
"uniform float	scl;\n"
"\n"
"layout (location = 0) out vec4 dst;\n"
"\n"
"void main(void)\n"
"{\n"
"	vec4 data = texture(texSrc, gl_FragCoord.xy);\n"
"	dst = vec4(data.r*scl,data.g*scl,data.b*scl,1.0);\n"
"//	dst = vec4(0.5,0.0,0.0,1.0);\n"
"\n"
"}\n"
;
	
	

	// Create and compile our GLSL program from the shaders
	LoadShadersCode(vertexShaderCode, fragmentShaderCode);

	// Attribute & Uniform location
	position = glGetAttribLocation(program, "position");
	texSrc = glGetUniformLocation(program, "texSrc");
	f_scl = glGetUniformLocation(program, "scl");
}


//-----------------------------------------------------------------------------
//glsShaderDrawU
glsShaderDrawU::glsShaderDrawU(void)
	:glsShaderDrawBase()
{

	const char vertexShaderCode[] =
		"#version 330 core\n"
		"layout (location = 0)in  vec2 position;\n"
		"void main(void)\n"
		"{\n"
		"   gl_Position  = vec4(position,0.0,1.0);\n"
		"}\n"
		;


	const char fragmentShaderCode[] =
		"#version 330 core\n"
		"precision highp float;\n"
		"uniform usampler2DRect	texSrc;\n"
		"uniform float	scl;\n"
		"\n"
		"layout (location = 0) out vec4 dst;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	vec4 data = vec4(texture(texSrc, gl_FragCoord.xy));\n"
		"	dst = vec4(data.r*scl,data.g*scl,data.b*scl,1.0);\n"
		"//	dst = vec4(0.5,0.0,0.0,1.0);\n"
		"\n"
		"}\n"
		;



	// Create and compile our GLSL program from the shaders
	LoadShadersCode(vertexShaderCode, fragmentShaderCode);

	// Attribute & Uniform location
	position = glGetAttribLocation(program, "position");
	texSrc = glGetUniformLocation(program, "texSrc");
	f_scl = glGetUniformLocation(program, "scl");
}





static Size getTextureSize(GLuint tex){
	int width;
	int height;

	//get texture size

	glBindTexture(GL_TEXTURE_RECTANGLE, tex);
	glGetTexLevelParameteriv(
		GL_TEXTURE_RECTANGLE, 0,
		GL_TEXTURE_WIDTH, &width
		);

	glGetTexLevelParameteriv(
		GL_TEXTURE_RECTANGLE, 0,
		GL_TEXTURE_HEIGHT, &height
		);

	glBindTexture(GL_TEXTURE_RECTANGLE, 0);

	return Size(width, height);
}




//---------------------------------------------------------------------------
//
static void glsDrawProcess(
	const glsShaderDrawBase* shader,	//progmra ID
	const GLuint& texSrc,			//src texture IDs
	const float scl					// scaling 
	)
{
	Size size = getTextureSize(texSrc);
	int width = size.width;
	int height = size.height;

	//program
	{
		glUseProgram(shader->program);
	}

	//uniform
	{
		glUniform1f(shader->f_scl, scl);
	}


	//Bind Texture
	{
		int id = 0;
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_RECTANGLE, texSrc);
		glUniform1i(shader->texSrc, id);
	}

	//---------------------------------
	// vbo
	GLuint vao = 0;
	GLuint vbo = 0;

	// [-1, 1] ‚Ì³•ûŒ`
	static GLfloat position[][2] = {
		{ -1.0f, -1.0f },
		{ 1.0f, -1.0f },
		{ 1.0f, 1.0f },
		{ -1.0f, 1.0f }
	};

	// create vao&vbo
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	// bind vao & vbo
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// upload vbo data
	glBufferData(GL_ARRAY_BUFFER, (int)sizeof(position), position, GL_STATIC_DRAW);

	// Set VertexAttribute
	glEnableVertexAttribArray(shaderDraw->position);	//enable attribute Location
	glVertexAttribPointer(
		shaderDraw->position,	// attribute location.
		2,					// size	(Specifies the number of components) x,y
		GL_FLOAT,			// type
		GL_FALSE,			// normalized?
		0,					// stride (Specifies the byte offset between consecutive generic vertex attributes)
		(void*)0			// array buffer offset (Specifies a pointer to the first generic vertex attribute in the array)
		);
	

	//Viewport
	{
		glViewport(0, 0, width, height);
	}

	//Render!!
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glFlush();
	}

	//	glFinish();

	//clean up
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

//-----------------------------------------------------------------------------
// execute Draw 
void glsDraw(glsMat& texture){
	// Clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glsShaderDrawBase* shader = 0;
	float scl = 0;

	switch (texture.internalFormat){
	case(GL_RGB32F) : shader = shaderDraw; scl = 1.0f; break;
	case(GL_RGB16UI) : shader = shaderDrawU; scl = 1.0f / 65536.0f; break;
	case(GL_RGB8UI) : shader = shaderDrawU; scl = 1.0f / 256.0f; break;
	default: assert(0);		//not implement
	}

	glsDrawProcess(shader, texture.texArray[0], scl);

}




