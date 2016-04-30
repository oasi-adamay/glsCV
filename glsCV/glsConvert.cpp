#include "stdafx.h"


#include "glsConvert.h"
#include "Timer.h"

//-----------------------------------------------------------------------------
// glsDraw shader
class glsShaderConvertBase : public glsShaderBase
{
public:

	//attribute location
	GLuint position;

	//uniform  location
	GLuint texSrc;
	GLuint f_scl;
};


//-----------------------------------------------------------------------------
// glsShaderConvert
class glsShaderConvert : public glsShaderConvertBase
{
public:
	glsShaderConvert(void);

};

//-----------------------------------------------------------------------------
// glsShaderConvertU unsigned to float
class glsShaderConvertU : public glsShaderConvertBase
{
public:
	glsShaderConvertU(void);

};

//-----------------------------------------------------------------------------
// glsShaderConvertS signed to float
class glsShaderConvertS : public glsShaderConvertBase
{
public:
	glsShaderConvertS(void);

};


//-----------------------------------------------------------------------------
//global 
glsShaderConvert* shaderConvert = 0;
glsShaderConvertU* shaderConvertU = 0;
glsShaderConvertS* shaderConvertS = 0;

void glsConvertInit(void){
	shaderConvert = new glsShaderConvert();
	shaderConvertU = new glsShaderConvertU();
	shaderConvertS = new glsShaderConvertS();

}

void glsConvertTerminate(void){
	delete shaderConvert;
	delete shaderConvertU;
	delete shaderConvertS;
}


static const char vertexShaderCode[] =
"#version 330 core\n"
"layout (location = 0)in  vec2 position;\n"
"void main(void)\n"
"{\n"
"   gl_Position  = vec4(position,0.0,1.0);\n"
"}\n"
;


//-----------------------------------------------------------------------------
//glsShaderConvert
glsShaderConvert::glsShaderConvert(void)
	:glsShaderConvertBase()
{
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
"	dst = vec4(data.r*scl,data.g*scl,data.b*scl,data.a*scl);\n"
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
//glsShaderConvertU
glsShaderConvertU::glsShaderConvertU(void)
	:glsShaderConvertBase()
{
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform usampler2DRect	texSrc;\n"
"uniform float	scl;\n"
"layout (location = 0) out vec4 dst;\n"
"void main(void)\n"
"{\n"
"	vec4 data = vec4(texture(texSrc, gl_FragCoord.xy));\n"
"	dst = vec4(data.r*scl,data.g*scl,data.b*scl,data.a*scl);\n"
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
//glsShaderConvertS
glsShaderConvertS::glsShaderConvertS(void)
	:glsShaderConvertBase()
{
	const char fragmentShaderCode[] =
		"#version 330 core\n"
		"precision highp float;\n"
		"uniform isampler2DRect	texSrc;\n"
		"uniform float	scl;\n"
		"layout (location = 0) out vec4 dst;\n"
		"void main(void)\n"
		"{\n"
		"	vec4 data = vec4(texture(texSrc, gl_FragCoord.xy));\n"
		"	dst = vec4(data.r*scl,data.g*scl,data.b*scl,data.a*scl);\n"
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
static void glsConvertProcess(
	const glsShaderConvertBase* shader,	//progmra ID
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
	glEnableVertexAttribArray(shaderConvert->position);	//enable attribute Location
	glVertexAttribPointer(
		shaderConvert->position,	// attribute location.
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


//conver texture format
void glsConvert(const glsMat& src, const glsMat& dst, const float scl){
	assert(dst.type == GL_FLOAT);

	glsShaderConvertBase* shader = 0;

	switch (src.type){
	case(GL_FLOAT) : shader = shaderConvert; break;
	case(GL_UNSIGNED_BYTE) :
	case(GL_UNSIGNED_SHORT) :
	case(GL_UNSIGNED_INT) :	shader = shaderConvertU; break;
	case(GL_BYTE) :
	case(GL_SHORT) :
	case(GL_INT) : shader = shaderConvertS; break;
	default: assert(0);		//not implement
	}

	//FBO 
	GLuint fbo = 0;
	//---------------------------------
	// FBO
	// create FBO (off-screen framebuffer)
	glGenFramebuffers(1, &fbo);

	// bind offscreen framebuffer (that is, skip the window-specific render target)
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GLenum bufs[] =
	{
		GL_COLOR_ATTACHMENT0,
	};
	glDrawBuffers(1, bufs);


	for (int i = 0; i < src.texArray.size(); i++){
		//dst texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, dst.texArray[i], 0);
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glsConvertProcess(shader, src.texArray[i], scl);
	}

	glDeleteFramebuffers(1, &fbo);

}




