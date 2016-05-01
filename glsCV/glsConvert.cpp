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
	GLuint i_flag;
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


/* Constants for color conversion */
//	CV_BGR2BGRA = 0,
//	CV_RGB2RGBA = CV_BGR2BGRA,
//
//	CV_BGRA2BGR = 1,
//	CV_RGBA2RGB = CV_BGRA2BGR,
//
//	CV_BGR2RGBA = 2,
//	CV_RGB2BGRA = CV_BGR2RGBA,
//
//	CV_RGBA2BGR = 3,
//	CV_BGRA2RGB = CV_RGBA2BGR,
//
//	CV_BGR2RGB = 4,
//	CV_RGB2BGR = CV_BGR2RGB,
//
//	CV_BGRA2RGBA = 5,
//	CV_RGBA2BGRA = CV_BGRA2RGBA,
//
//	CV_BGR2GRAY = 6,
//	CV_RGB2GRAY = 7,
//	CV_GRAY2BGR = 8,
//	CV_GRAY2RGB = CV_GRAY2BGR,
//	CV_GRAY2BGRA = 9,
//	CV_GRAY2RGBA = CV_GRAY2BGRA,
//	CV_BGRA2GRAY = 10,
//	CV_RGBA2GRAY = 11,


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
"uniform int	flag;\n"
"layout (location = 0) out vec4 dst;\n"
"void main(void)\n"
"{\n"
"	vec4 data = texture(texSrc, gl_FragCoord.xy);\n"
"	vec4 color = vec4(data.r*scl,data.g*scl,data.b*scl,data.a*scl);\n"
"	float gray;\n"
"	switch(flag){\n"
"	case(0)://CV_BGR2BGRA\n"
"	case(1)://CV_BGRA2BGR\n"
"		color = vec4(color.r,color.g,color.b,1.0);break;\n"
"	case(2)://CV_BGR2RGBA\n"
"	case(3)://CV_RGBA2BGR\n"
"	case(4)://CV_BGR2RGB\n"
"	case(5)://CV_BGRA2RGBA\n"
"		color = vec4(color.b,color.g,color.r,1.0);break;\n"
"	case(6)://CV_BGR2GRAY\n"
"		gray = color.b * 0.299 + color.g * 0.587 + color.r * 0.114;\n"
"		color = vec4(gray,gray,gray,1.0);break;\n"
"	case(7)://CV_RGB2GRAY\n"
"		gray = color.r * 0.299 + color.g * 0.587 + color.b * 0.114;\n"
"		color = vec4(gray,gray,gray,1.0);break;\n"
"	case(8)://CV_GRAY2BGR\n"
"	case(9)://CV_GRAY2BGRA\n"
"	case(10)://CV_BGRA2GRAY\n"
"	case(11)://CV_RGBA2GRAY\n"
"		gray = color.r;\n"
"		color = vec4(gray,gray,gray,1.0);break;\n"
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
//glsShaderConvertU
glsShaderConvertU::glsShaderConvertU(void)
	:glsShaderConvertBase()
{
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform usampler2DRect	texSrc;\n"
"uniform float	scl;\n"
"uniform int	flag;\n"
"layout (location = 0) out vec4 dst;\n"
"void main(void)\n"
"{\n"
"	vec4 data = vec4(texture(texSrc, gl_FragCoord.xy));\n"
"	vec4 color = vec4(data.r*scl,data.g*scl,data.b*scl,data.a*scl);\n"
"	float gray;\n"
"	switch(flag){\n"
"	case(0)://CV_BGR2BGRA\n"
"	case(1)://CV_BGRA2BGR\n"
"		color = vec4(color.r,color.g,color.b,1.0);break;\n"
"	case(2)://CV_BGR2RGBA\n"
"	case(3)://CV_RGBA2BGR\n"
"	case(4)://CV_BGR2RGB\n"
"	case(5)://CV_BGRA2RGBA\n"
"		color = vec4(color.b,color.g,color.r,1.0);break;\n"
"	case(6)://CV_BGR2GRAY\n"
"		gray = color.b * 0.299 + color.g * 0.587 + color.r * 0.114;\n"
"		color = vec4(gray,gray,gray,1.0);break;\n"
"	case(7)://CV_RGB2GRAY\n"
"		gray = color.r * 0.299 + color.g * 0.587 + color.b * 0.114;\n"
"		color = vec4(gray,gray,gray,1.0);break;\n"
"	case(8)://CV_GRAY2BGR\n"
"	case(9)://CV_GRAY2BGRA\n"
"	case(10)://CV_BGRA2GRAY\n"
"	case(11)://CV_RGBA2GRAY\n"
"		gray = color.r;\n"
"		color = vec4(gray,gray,gray,1.0);break;\n"
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
"uniform int	flag;\n"
"layout (location = 0) out vec4 dst;\n"
"void main(void)\n"
"{\n"
"	vec4 data = vec4(texture(texSrc, gl_FragCoord.xy));\n"
"	vec4 color = vec4(data.r*scl,data.g*scl,data.b*scl,data.a*scl);\n"
"	float gray;\n"
"	switch(flag){\n"
"	case(0)://CV_BGR2BGRA\n"
"	case(1)://CV_BGRA2BGR\n"
"		color = vec4(color.r,color.g,color.b,1.0);break;\n"
"	case(2)://CV_BGR2RGBA\n"
"	case(3)://CV_RGBA2BGR\n"
"	case(4)://CV_BGR2RGB\n"
"	case(5)://CV_BGRA2RGBA\n"
"		color = vec4(color.b,color.g,color.r,1.0);break;\n"
"	case(6)://CV_BGR2GRAY\n"
"		gray = color.b * 0.299 + color.g * 0.587 + color.r * 0.114;\n"
"		color = vec4(gray,gray,gray,1.0);break;\n"
"	case(7)://CV_RGB2GRAY\n"
"		gray = color.r * 0.299 + color.g * 0.587 + color.b * 0.114;\n"
"		color = vec4(gray,gray,gray,1.0);break;\n"
"	case(8)://CV_GRAY2BGR\n"
"	case(9)://CV_GRAY2BGRA\n"
"	case(10)://CV_BGRA2GRAY\n"
"	case(11)://CV_RGBA2GRAY\n"
"		gray = color.r;\n"
"		color = vec4(gray,gray,gray,1.0);break;\n"
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
	const float scl,				// scaling 
	const int flag	= -1			// flag 
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
void glsConvert(const glsMat& src, glsMat& dst, const float scl){

	glsMat _dst = glsMat(src.size(), CV_MAKETYPE(CV_32F, CV_MAT_CN(src.ocvtype())), src.blkNum());
	assert(_dst.type == GL_FLOAT);

	glsShaderConvertBase* shader = 0;

	//switch (src.type){
	//case(GL_FLOAT) : shader = shaderConvert; break;
	//case(GL_UNSIGNED_BYTE) :
	//case(GL_UNSIGNED_SHORT) :
	//case(GL_UNSIGNED_INT) :	shader = shaderConvertU; break;
	//case(GL_BYTE) :
	//case(GL_SHORT) :
	//case(GL_INT) : shader = shaderConvertS; break;
	//default: assert(0);		//not implement
	//}

	switch (CV_MAT_DEPTH(src.ocvtype())){
	case(CV_32F) : shader = shaderConvert; break;
	case(CV_8U) :
	case(CV_16U) : shader = shaderConvertU; break;
	case(CV_8S) :
	case(CV_16S) :
	case(CV_32S) : shader = shaderConvertS; break;
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
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, _dst.texArray[i], 0);
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glsConvertProcess(shader, src.texArray[i], scl);
	}

	glDeleteFramebuffers(1, &fbo);
	
	dst = _dst;

}

void glsCvtColor(const glsMat& src, glsMat& dst, const int code){
	assert(src.type == GL_FLOAT);

	int ch=1;
	switch (code){
	case(CV_BGR2BGRA) :
//	case(CV_RGB2RGBA) :
	case(CV_BGR2RGBA) :
//	case(CV_RGB2BGRA) :
	case(CV_BGRA2RGBA) :
//	case(CV_RGBA2BGRA) :
	case(CV_GRAY2BGRA) :
//	case(CV_GRAY2RGBA) :
		ch = 4; break;
	case(CV_BGRA2BGR) :
//	case(CV_RGBA2RGB) :
	case(CV_RGBA2BGR) :
//	case(CV_BGRA2RGB) :
	case(CV_BGR2RGB) :
//	case(CV_RGB2BGR) :
	case(CV_GRAY2BGR) :
//	case(CV_GRAY2RGB) :
		ch = 3; break;
	case(CV_BGR2GRAY) :
	case(CV_RGB2GRAY) :
	case(CV_BGRA2GRAY) :
	case(CV_RGBA2GRAY) :
		ch = 1; break;
	default:
		assert(0);
	}

	glsMat _dst = glsMat(src.size(), CV_MAKETYPE(CV_MAT_DEPTH(src.ocvtype()), ch), src.blkNum());

	glsShaderConvertBase* shader = 0;

	switch (CV_MAT_DEPTH(src.ocvtype())){
	case(CV_32F) : shader = shaderConvert; break;
	case(CV_8U) :
	case(CV_16U) : shader = shaderConvertU; break;
	case(CV_8S) :
	case(CV_16S) :
	case(CV_32S) : shader = shaderConvertS; break;
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
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, _dst.texArray[i], 0);
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		float scl = 1.0f;
		glsConvertProcess(shader, src.texArray[i], scl, code);
	}

	glDeleteFramebuffers(1, &fbo);

	dst = _dst;

}





