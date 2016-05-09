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


#include "glsCopy.h"
//#include "Timer.h"


namespace gls
{



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

void copyInit(void){
	shaderCopy = new glsShaderCopy();
	shaderCopyU = new glsShaderCopyU();
	shaderCopyS = new glsShaderCopyS();
}

void copyTerminate(void){
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
		glUniform2iv(glGetUniformLocation(shader->program, "offset"), 1, &offset[0]);
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
void copy(const GlsMat& src, GlsMat& dst){
	GlsMat _dst = GlsMat(src.size(), src.type());
	glsShaderBase* shader = selectShader(src.type());
	Rect rectSrc(0, 0, src.cols, src.rows);
	Rect rectDst = rectSrc;
	glsFBO fbo(1);
	//dst texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _dst.texid(), 0);
	GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glsCopyProcess(shader, src.texid(), rectSrc, rectDst);
	dst = _dst;
}


//copy texture with rect
void copyRect(const GlsMat& src, GlsMat& dst, const Rect& rect){
	GlsMat _dst = GlsMat(rect.size(), src.type());
	glsShaderBase* shader = selectShader(src.type());

	glsFBO fbo(1);

	Rect rectSrc(rect.x, rect.y, _dst.cols, _dst.rows);
	Rect rectDst(0, 0, _dst.cols, _dst.rows);

	//dst texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _dst.texid(), 0);
	GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glsCopyProcess(shader, src.texid(), rectSrc, rectDst);

	dst = _dst;
}


void tiled(const GlsMat& src, vector<vector<GlsMat>>& dst, const Size& blkNum){
	GLS_Assert(src.isContinuous());
	GLS_Assert(blkNum.width >= 1);
	GLS_Assert(blkNum.height >= 1);
	GLS_Assert(src.cols % blkNum.width == 0);
	GLS_Assert(src.rows % blkNum.height == 0);

	Size sizeDst = Size(src.cols / blkNum.width, src.rows / blkNum.height);
	dst = vector<vector<GlsMat>>(blkNum.height, vector<GlsMat>(blkNum.width));


	glsShaderBase* shader = selectShader(src.type());

	glsFBO fbo(1);

	for (int by = 0; by < blkNum.height; by++){
		for (int bx = 0; bx < blkNum.width; bx++){
			dst[by][bx] = GlsMat(sizeDst, src.type());

			Rect rectSrc(bx* sizeDst.width, by* sizeDst.height, sizeDst.width, sizeDst.height);
			Rect rectDst(0, 0, sizeDst.width, sizeDst.height);

			//dst texture
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst[by][bx].texid(), 0);
			GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

			glsCopyProcess(shader, src.texid(), rectSrc, rectDst);
		}
	}

}

void untiled(const vector<vector<GlsMat>>& src, GlsMat& dst){

	Size blkNum((int)src[0].size(), (int)src.size());
	Size sizeSrc = Size(src[0][0].cols, src[0][0].rows);
	Size sizeDst = Size(sizeSrc.width * blkNum.width, sizeSrc.height * blkNum.height);


	dst = GlsMat(sizeDst, src[0][0].type());

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

}//namespace gls



