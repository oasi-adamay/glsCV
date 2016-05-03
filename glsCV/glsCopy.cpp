#include "stdafx.h"


#include "glsCopy.h"
//#include "Timer.h"



//-----------------------------------------------------------------------------
class glsShaderCopyBase : public glsShaderBase
{
public:
};


//-----------------------------------------------------------------------------
// glsShaderCopy
class glsShaderCopy : public glsShaderCopyBase
{
public:
	glsShaderCopy(void);

};

//-----------------------------------------------------------------------------
// glsShaderCopyU unsigned
class glsShaderCopyU : public glsShaderCopyBase
{
public:
	glsShaderCopyU(void);
};

//-----------------------------------------------------------------------------
// glsShaderCopyS unsigned
class glsShaderCopyS : public glsShaderCopyBase
{
public:
	glsShaderCopyS(void);
};


//-----------------------------------------------------------------------------
//global 
glsShaderCopy* shaderCopy = 0;
glsShaderCopyU* shaderCopyU = 0;
glsShaderCopyS* shaderCopyS = 0;

void glsCopyInit(void){
	shaderCopy = new glsShaderCopy();
	shaderCopyU = new glsShaderCopyU();
	shaderCopyS = new glsShaderCopyS();
}

void glsCopyTerminate(void){
	delete shaderCopy;
	delete shaderCopyU;
	delete shaderCopyS;
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
//glsShaderCopy
glsShaderCopy::glsShaderCopy(void)
	:glsShaderCopyBase()
{
	const char fragmentShaderCode[] = 
"#version 330 core\n"
"precision highp float;\n"
"uniform ivec2	offset;\n"
"uniform sampler2D	texSrc;\n"
"layout (location = 0) out vec4 dst;\n"
"void main(void)\n"
"{\n"
"	dst = texelFetch(texSrc, ivec2(gl_FragCoord.xy)+offset,0);\n"
"\n"
"}\n"
;
	// Create and compile our GLSL program from the shaders
	LoadShadersCode(vertexShaderCode, fragmentShaderCode);
}


//-----------------------------------------------------------------------------
//glsShaderCopyU
glsShaderCopyU::glsShaderCopyU(void)
	:glsShaderCopyBase()
{
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform ivec2	offset;\n"
"uniform usampler2D	texSrc;\n"
"layout (location = 0) out uvec4 dst;\n"
"void main(void)\n"
"{\n"
"	dst = texelFetch(texSrc, ivec2(gl_FragCoord.xy)+offset,0);\n"
"}\n"
;
	// Create and compile our GLSL program from the shaders
	LoadShadersCode(vertexShaderCode, fragmentShaderCode);
}

//-----------------------------------------------------------------------------
//glsShaderCopyS
glsShaderCopyS::glsShaderCopyS(void)
	:glsShaderCopyBase()
{
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform ivec2	offset;\n"
"uniform isampler2D	texSrc;\n"
"layout (location = 0) out ivec4 dst;\n"
"void main(void)\n"
"{\n"
"	dst = texelFetch(texSrc, ivec2(gl_FragCoord.xy)+offset,0);\n"
"}\n"
;
	// Create and compile our GLSL program from the shaders
	LoadShadersCode(vertexShaderCode, fragmentShaderCode);
}




//---------------------------------------------------------------------------
//
static void glsCopyProcess(
	const glsShaderCopyBase* shader,	//progmra ID
	const GLuint& texSrc,			//src texture IDs
	const Rect& rect				// copy rectangel
	)
{
	int width = rect.width;
	int height = rect.height;
	const int offset[2] = { rect.x, rect.y };

	//program
	{
		glUseProgram(shader->program);
	}

	//uniform
	{
		glUniform2iv(glGetUniformLocation(shader->program, "offset"), 1,&offset[0]);
	}


	//Bind Texture
	{
		int id = 0;
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, texSrc);
		glUniform1i(glGetUniformLocation(shader->program, "texSrc"), id);
	}

	glsVAO vao(glGetAttribLocation(shader->program,"position"));

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

//copy texture with rect
void glsCopy(const glsMat& src, glsMat& dst){
	glsMat _dst = glsMat(src.size(), src.type(),src.blkNum());

	glsShaderCopyBase* shader = 0;

	Rect rect(0, 0, src.cols,src.rows);


	switch (CV_MAT_DEPTH(src.type())){
	case(CV_32F) : shader = shaderCopy; break;
	case(CV_8U) :
	case(CV_16U) : shader = shaderCopyU; break;
	case(CV_8S) :
	case(CV_16S) :
	case(CV_32S) : shader = shaderCopyS; break;
	default: GLS_Assert(0);		//not implement
	}

	glsFBO fbo(1);

	for (int i = 0; i < src.texArray.size(); i++){
		//dst texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _dst.texArray[i], 0);
		GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glsCopyProcess(shader, src.texArray[i], rect);
	}

	dst = _dst;
}


//copy texture with rect
void glsCopy(const glsMat& src, glsMat& dst, const Rect& rect, const Size& blkNum){
	GLS_Assert(src.isContinuous());
	GLS_Assert(rect.width % blkNum.width == 0);
	GLS_Assert(rect.height % blkNum.height == 0);

	glsMat _dst = glsMat(rect.size(), src.type(), blkNum);

	glsShaderCopyBase* shader = 0;


	switch (CV_MAT_DEPTH(src.type())){
	case(CV_32F) : shader = shaderCopy; break;
	case(CV_8U) :
	case(CV_16U) : shader = shaderCopyU; break;
	case(CV_8S) :
	case(CV_16S) :
	case(CV_32S) : shader = shaderCopyS; break;
	default: GLS_Assert(0);		//not implement
	}

	glsFBO fbo(1);

	for (int by = 0; by < _dst.blkNumY(); by++){
		for (int bx = 0; bx < _dst.blkNumX(); bx++){
			int x = rect.x + bx* _dst.texWidth();
			int y = rect.y + by* _dst.texHeight();
			Rect _rect(x, y, _dst.texWidth(), _dst.texHeight());

			//dst texture
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _dst.at(by, bx), 0);
			GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

			glsCopyProcess(shader, src.texArray[0], _rect);
		}
	}
	
	dst = _dst;
}





