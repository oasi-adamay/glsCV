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
	GLuint i_flag;
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
		"uniform sampler2D	texSrc;\n"
		"uniform float	scl;\n"
		"uniform int 	flag;\n"
		"\n"
		"layout (location = 0) out vec4 dst;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	vec4 data = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0);\n"
		"	vec4 color;\n"
		"	switch(flag&3){\n"
		"	case(0):color = vec4(data.r*scl,data.g*scl,data.b*scl,data.g*scl);break;\n"
		"	case(1):color = vec4(data.r*scl,data.r*scl,data.r*scl,1.0);break;\n"
		"	case(2):color = vec4(data.r*scl,data.r*scl,data.r*scl,1.0);break;\n"
		"	case(3):color = vec4(data.r*scl,data.g*scl,data.b*scl,1.0);break;\n"
		"	}\n"
		"	dst = color;\n"
		"\n"
		"}\n"
;
	
	

	// Create and compile our GLSL program from the shaders
	LoadShadersCode(vertexShaderCode, fragmentShaderCode);

	// Attribute & Uniform location
	position = glGetAttribLocation(program, "position");
	texSrc = glGetUniformLocation(program, "texSrc");
	f_scl = glGetUniformLocation(program, "scl");
	i_flag = glGetUniformLocation(program, "flag");
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
		"uniform usampler2D	texSrc;\n"
		"uniform float	scl;\n"
		"uniform int 	flag;\n"
		"\n"
		"layout (location = 0) out vec4 dst;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	vec4 data = vec4(texelFetch(texSrc, ivec2(gl_FragCoord.xy),0));\n"
		"	vec4 color;\n"
		"	switch(flag&3){\n"
		"	case(0):color = vec4(data.r*scl,data.g*scl,data.b*scl,data.g*scl);break;\n"
		"	case(1):color = vec4(data.r*scl,data.r*scl,data.r*scl,1.0);break;\n"
		"	case(2):color = vec4(data.r*scl,data.r*scl,data.r*scl,1.0);break;\n"
		"	case(3):color = vec4(data.r*scl,data.g*scl,data.b*scl,1.0);break;\n"
		"	}\n"
		"	dst = color;\n"
		"}\n"
		;



	// Create and compile our GLSL program from the shaders
	LoadShadersCode(vertexShaderCode, fragmentShaderCode);

	// Attribute & Uniform location
	position = glGetAttribLocation(program, "position");
	texSrc = glGetUniformLocation(program, "texSrc");
	f_scl = glGetUniformLocation(program, "scl");
	i_flag = glGetUniformLocation(program, "flag");

}





static Size getTextureSize(GLuint tex){
	int width;
	int height;

	//get texture size

	glBindTexture(GL_TEXTURE_2D, tex);
	glGetTexLevelParameteriv(
		GL_TEXTURE_2D, 0,
		GL_TEXTURE_WIDTH, &width
		);

	glGetTexLevelParameteriv(
		GL_TEXTURE_2D, 0,
		GL_TEXTURE_HEIGHT, &height
		);

	glBindTexture(GL_TEXTURE_2D, 0);

	return Size(width, height);
}




//---------------------------------------------------------------------------
//
static void glsDrawProcess(
	const glsShaderDrawBase* shader,	//progmra ID
	const GLuint& texSrc,			//src texture IDs
	const float scl,					// scaling 
	const int   flag					// flag [1:0] channnel 
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
		glUniform1i(shader->i_flag, flag);
	}


	//Bind Texture
	{
		int id = 0;
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, texSrc);
		glUniform1i(shader->texSrc, id);
	}

	glsVAO vao(shader->position);
	

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

}

//-----------------------------------------------------------------------------
// execute Draw 
void glsDraw(glsMat& src){
	// Clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glsShaderDrawBase* shader = 0;
	float scl = 0;
	int flag = 0;

	switch (src.glType()){
	case(GL_FLOAT) :
		scl = 1.0f;
		shader = shaderDraw;
		break;
	case(GL_UNSIGNED_BYTE) :
		scl = 1.0f / 256.0f;
		shader = shaderDrawU;
		break;
	case(GL_UNSIGNED_SHORT) :
		scl = 1.0f / 65536.0f;
		shader = shaderDrawU;
		break;
	case(GL_UNSIGNED_INT) :
		scl = 1.0f / (65536.0f * 65536.0f);
		shader = shaderDrawU;
		break;
	//case(GL_BYTE) :
	//case(GL_SHORT) :
	//case(GL_INT) : shader = shaderDrawI; break;
	default: assert(0);		//not implement
	}

	switch (src.glFormat()){
	case(GL_RED) :
	case(GL_RED_INTEGER) :
		flag |= 1;
		break;
	case(GL_RG) :
	case(GL_RG_INTEGER) :
	    flag |= 2;
		break;
	case(GL_RGB) :
	case(GL_RGB_INTEGER) :
		flag |= 3;
		break;
	case(GL_RGBA) :
	case(GL_RGBA_INTEGER) :
		break;
	default: assert(0);		//not implement
	}



	glsDrawProcess(shader, src.texArray[0], scl,flag);

}




