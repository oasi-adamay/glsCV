#include "stdafx.h"


#include "glsCopy.h"
//#include "Timer.h"





//-----------------------------------------------------------------------------
// glsShaderCopy
class glsShaderCopy : public glsShaderBase
{
public:
	glsShaderCopy(void);

};

//-----------------------------------------------------------------------------
// glsShaderCopyU unsigned
class glsShaderCopyU : public glsShaderBase
{
public:
	glsShaderCopyU(void);
};

//-----------------------------------------------------------------------------
// glsShaderCopyS unsigned
class glsShaderCopyS : public glsShaderBase
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
	:glsShaderBase()
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
	:glsShaderBase()
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
	:glsShaderBase()
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
	const glsShaderBase* shader,	//progmra ID
	const GLuint& texSrc,			//src texture IDs
	const Rect& rectSrc,			// copy src rectangel
	const Rect& rectDst 			// copy dst rectangel
	)
{
	const int offset[2] = { rectSrc.x, rectSrc.y };

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

	glsVAO vao(glGetAttribLocation(shader->program, "position"));
	//Viewport
	{
		glViewport(rectDst.x, rectDst.y, rectDst.width, rectDst.height);
	}

	//Render!!
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glFlush();
	}

	GL_CHECK_ERROR();

	//	glFinish();

}

static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = shaderCopy; break;
	case(CV_8U) :
	case(CV_16U) : shader = shaderCopyU; break;
	case(CV_8S) :
	case(CV_16S) :
	case(CV_32S) : shader = shaderCopyS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}

//copy texture
void glsCopy(const glsMat& src, glsMat& dst){
	glsMat _dst = glsMat(src.size(), src.type(),src.blkNum());

	glsShaderBase* shader = selectShader(src.type());

	Rect rectSrc(0, 0, src.texWidth(), src.texHeight());
	Rect rectDst = rectSrc;

	glsFBO fbo(1);

	for (int i = 0; i < src.texArray.size(); i++){
		//dst texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _dst.texArray[i], 0);
		GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glsCopyProcess(shader, src.texArray[i], rectSrc, rectDst);

	}
	dst = _dst;
}

//copy texture to tiled texture
void glsCopyTiled(const glsMat& src, glsMat& dst, const Size& blkNum){
	GLS_Assert(src.isContinuous());
	GLS_Assert(blkNum.width >=1);
	GLS_Assert(blkNum.height >= 1);
	GLS_Assert(src.cols % blkNum.width == 0);
	GLS_Assert(src.rows % blkNum.height == 0);

	glsMat _dst = glsMat(src.size(), src.type(), blkNum);

	glsShaderBase* shader = selectShader(src.type());

	glsFBO fbo(1);

	for (int by = 0; by < _dst.blkNumY(); by++){
		for (int bx = 0; bx < _dst.blkNumX(); bx++){
			Rect rectSrc(bx* _dst.texWidth(), by* _dst.texHeight(), _dst.texWidth(), _dst.texHeight());
			Rect rectDst(0, 0, _dst.texWidth(), _dst.texHeight());

			//dst texture
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _dst.at(by, bx), 0);
			GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

			glsCopyProcess(shader, src.texArray[0], rectSrc, rectDst);
		}
	}

	dst = _dst;

}

//copy tiled texture to untiled texture
void glsCopyUntiled(const glsMat& src, glsMat& dst){

	glsMat _dst = glsMat(src.size(), src.type(), Size(1,1));

	glsShaderBase* shader = selectShader(src.type());	glsFBO fbo(1);

	//dst texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _dst.at(0,0), 0);
	GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	for (int by = 0; by < src.blkNumY(); by++){
		for (int bx = 0; bx < src.blkNumX(); bx++){
			Rect rectSrc(-bx* src.texWidth(), -by* src.texHeight(), src.texWidth(), src.texHeight());
			Rect rectDst(bx* src.texWidth(), by* src.texHeight(), src.texWidth(), src.texHeight());

			glsCopyProcess(shader, src.at(by, bx), rectSrc, rectDst);
		}
	}

	dst = _dst;

}



//copy texture with rect
void glsCopyRect(const glsMat& src, glsMat& dst, const Rect& rect, const Size& blkNum){
	GLS_Assert(src.isContinuous());
	GLS_Assert(rect.width % blkNum.width == 0);
	GLS_Assert(rect.height % blkNum.height == 0);

	glsMat _dst = glsMat(rect.size(), src.type(), blkNum);

	glsShaderBase* shader = selectShader(src.type());

	glsFBO fbo(1);

	for (int by = 0; by < _dst.blkNumY(); by++){
		for (int bx = 0; bx < _dst.blkNumX(); bx++){
			Rect rectSrc(rect.x + bx* _dst.texWidth(), rect.y + by* _dst.texHeight(), _dst.texWidth(), _dst.texHeight());
			Rect rectDst(0, 0, _dst.texWidth(), _dst.texHeight());

			//dst texture
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _dst.at(by, bx), 0);
			GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

			glsCopyProcess(shader, src.texArray[0], rectSrc, rectDst);
		}
	}
	
	dst = _dst;
}


void glsTiled(const glsMat& src, vector<vector<glsMat>>& dst, const Size& blkNum){
	GLS_Assert(src.isContinuous());
	GLS_Assert(blkNum.width >= 1);
	GLS_Assert(blkNum.height >= 1);
	GLS_Assert(src.cols % blkNum.width == 0);
	GLS_Assert(src.rows % blkNum.height == 0);

	Size sizeDst = Size(src.cols / blkNum.width, src.rows / blkNum.height);
	dst = vector<vector<glsMat>>(blkNum.height, vector<glsMat>(blkNum.width));


	glsShaderBase* shader = selectShader(src.type());

	glsFBO fbo(1);

	for (int by = 0; by < blkNum.height; by++){
		for (int bx = 0; bx < blkNum.width; bx++){
			dst[by][bx] = glsMat(sizeDst, src.type());

			Rect rectSrc(bx* sizeDst.width, by* sizeDst.height, sizeDst.width, sizeDst.height);
			Rect rectDst(0, 0, sizeDst.width, sizeDst.height);

			//dst texture
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst[by][bx].texid(), 0);
			GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

			glsCopyProcess(shader, src.texid(), rectSrc, rectDst);
		}
	}

}
void glsUntiled(const vector<vector<glsMat>>& src, glsMat& dst){

	Size blkNum(src[0].size(), src.size());
	Size sizeSrc = Size(src[0][0].cols, src[0][0].rows);
	Size sizeDst = Size(sizeSrc.width * blkNum.width, sizeSrc.height * blkNum.height);


	dst = glsMat(sizeDst, src[0][0].type());

	glsShaderBase* shader = selectShader(src[0][0].type());	
	glsFBO fbo(1);

	//dst texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst.texid(), 0);
	GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	for (int by = 0; by < blkNum.height; by++){
		for (int bx = 0; bx < blkNum.width; bx++){
			Rect rectSrc(-bx* sizeSrc.width, -by* sizeSrc.height, sizeSrc.width, sizeSrc.height);
			Rect rectDst(+bx* sizeSrc.width, +by* sizeSrc.height, sizeSrc.width, sizeSrc.height);

			glsCopyProcess(shader, src[by][bx].texid(), rectSrc, rectDst);
		}
	}

}



