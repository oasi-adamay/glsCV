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

/*-----------------------------------------------------------------------------
include
*/
#include "glsMacro.h"
#include "GlsMat.h"
#include "glsShader.h"

#include "glsCopy.h"

#ifdef _DEBUG
//#if 1
#include "Timer.h"
#define _TMR_(...)  Timer tmr(__VA_ARGS__)
#else
#define _TMR_(...)
#endif

#define _GLS_COPY_USE_SHADER

namespace gls
{


#ifdef  _GLS_COPY_USE_SHADER


class glsShaderCopyBase : public glsShaderBase
{
protected:
	list<string> UniformNameList(void) {
		list<string> lst;
		lst.push_back("texSrc");
		lst.push_back("ofs");
		return lst;
	}
public:
	glsShaderCopyBase(const string& _name) :glsShaderBase(_name) {}

};


//-----------------------------------------------------------------------------
// glsShaderCopy
class glsShaderCopy : public glsShaderCopyBase
{
protected:
	string FragmentShaderCode(void) {
		const char fragmentShaderCode[] = TO_STR(
			#version 330 core\n
			precision highp float; \n
			uniform sampler2D	texSrc; \n
			uniform vec2 ofs; \n
			layout(location = 0) out vec4 dst; \n
			void main(void)\n
		{ \n
			dst = texelFetch(texSrc, ivec2(gl_FragCoord.xy + ofs),0); \n
			\n
		}\n
		);
		return fragmentShaderCode;
	}

public:
	glsShaderCopy(void) :glsShaderCopyBase(__FUNCTION__) {}

};

//-----------------------------------------------------------------------------
// glsShaderCopyU unsigned
class glsShaderCopyU : public glsShaderCopyBase
{
protected:
	string FragmentShaderCode(void) {
		const char fragmentShaderCode[] = TO_STR(
			#version 330 core\n
			precision highp float; \n
			uniform usampler2D	texSrc; \n
			uniform vec2 ofs; \n
			layout(location = 0) out uvec4 dst; \n
			void main(void)\n
		{ \n
			dst = texelFetch(texSrc, ivec2(gl_FragCoord.xy + ofs),0); \n
		}\n
		);
		return fragmentShaderCode;
	}

public:
	glsShaderCopyU(void) :glsShaderCopyBase(__FUNCTION__) {}
};

//-----------------------------------------------------------------------------
// glsShaderCopyS signed
class glsShaderCopyS : public glsShaderCopyBase
{
protected:
	string FragmentShaderCode(void) {
		const char fragmentShaderCode[] = TO_STR(
			#version 330 core\n
			precision highp float; \n
			uniform isampler2D	texSrc; \n
			uniform vec2 ofs; \n
			layout(location = 0) out ivec4 dst; \n
			void main(void)\n
		{ \n
			dst = texelFetch(texSrc, ivec2(gl_FragCoord.xy+ofs),0); \n
		}\n
		);
		return fragmentShaderCode;
	}

public:
	glsShaderCopyS(void) :glsShaderCopyBase(__FUNCTION__) {}
};


//-----------------------------------------------------------------------------
//global 
glsShaderCopy ShaderCopy;
glsShaderCopyU ShaderCopyU;
glsShaderCopyS ShaderCopyS;


static
glsShaderBase* selectShader(int type) {
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)) {
	case(CV_32F): shader = &ShaderCopy; break;
	case(CV_8U):
	case(CV_16U): shader = &ShaderCopyU; break;
	case(CV_8S):
	case(CV_16S):
	case(CV_32S): shader = &ShaderCopyS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}

#endif

void copy(const GlsMat& src, GlsMat& dst){
#ifdef  _GLS_COPY_USE_SHADER
	GlsMat _dst = GlsMat(src.size(), src.type());
	glsShaderBase* shader = selectShader(src.type());
	Vec2f ofs(0.0, 0.0);
	shader->Execute(src,ofs,_dst);
	dst = _dst;
#else

	GlsMat _dst = getDstMat(src,dst);

	enum {
		DRAW,
		READ,
		SIZEOF
	};

	GLuint fbo[SIZEOF];		// FBO identifier

	glGenFramebuffers(2, fbo);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo[READ]);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, src.texid(), 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo[DRAW]);
	glBindTexture(GL_TEXTURE_2D, _dst.texid());

	{
		_TMR_("glCopyTexSubImage2D:\t");
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _dst.cols, _dst.rows);
		GL_CHECK_ERROR();
	}

	glDeleteFramebuffers(sizeof(fbo) / sizeof(fbo[0]), fbo);

	dst = _dst;

#endif
}


//copy texture with rect
void copyRect(const GlsMat& src, GlsMat& dst, const Rect& rect){
#ifdef  _GLS_COPY_USE_SHADER
	GlsMat _dst = GlsMat(rect.size(), src.type());
	glsShaderBase* shader = selectShader(src.type());
	Vec2f ofs((float)rect.x, (float)rect.y);
	shader->Execute(src, ofs, _dst);
	dst = _dst;
#else
	GlsMat _dst = getDstMat(rect.size(),src.type(),dst);

	enum {
		DRAW,
		READ,
		SIZEOF
	};

	Rect rectSrc(rect.x, rect.y, _dst.cols, _dst.rows);
	Rect rectDst(0, 0, _dst.cols, _dst.rows);

	GLuint fbo[SIZEOF];		// FBO identifier

	glGenFramebuffers(2, fbo);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo[READ]);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, src.texid(), 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo[DRAW]);
	glBindTexture(GL_TEXTURE_2D, _dst.texid());

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, rectDst.x, rectDst.y, rectSrc.x, rectSrc.y, rectDst.width, rectDst.height);
	GL_CHECK_ERROR();

	glDeleteFramebuffers(sizeof(fbo) / sizeof(fbo[0]), fbo);

	dst = _dst;

#endif
}


void tiled(const GlsMat& src, vector<vector<GlsMat>>& dst, const Size& blkNum){
	_TMR_("gls::tiled():\t");

	GLS_Assert(src.isContinuous());
	GLS_Assert(blkNum.width >= 1);
	GLS_Assert(blkNum.height >= 1);
	GLS_Assert(src.cols % blkNum.width == 0);
	GLS_Assert(src.rows % blkNum.height == 0);

	Size sizeDst = Size(src.cols / blkNum.width, src.rows / blkNum.height);
	dst = vector<vector<GlsMat>>(blkNum.height, vector<GlsMat>(blkNum.width));

#ifdef  _GLS_COPY_USE_SHADER
	glsShaderBase* shader = selectShader(src.type());

	for (int by = 0; by < blkNum.height; by++){
		for (int bx = 0; bx < blkNum.width; bx++){
			dst[by][bx] = GlsMat(sizeDst, src.type());

			Rect rect(bx* sizeDst.width, by* sizeDst.height, sizeDst.width, sizeDst.height);
			Vec2f ofs((float)rect.x, (float)rect.y);
			shader->Execute(src, ofs, dst[by][bx]);
		}
	}
#else
	enum {
		DRAW,
		READ,
		SIZEOF
	};

	GLuint fbo[SIZEOF];		// FBO identifier

	glGenFramebuffers(2, fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo[READ]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo[DRAW]);

	for (int by = 0; by < blkNum.height; by++){
		for (int bx = 0; bx < blkNum.width; bx++){

			dst[by][bx] = GlsMat(sizeDst, src.type());

			Rect rectSrc(bx* sizeDst.width, by* sizeDst.height, sizeDst.width, sizeDst.height);
			Rect rectDst(0, 0, sizeDst.width, sizeDst.height);

			glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, src.texid(), 0);
			glBindTexture(GL_TEXTURE_2D, dst[by][bx].texid());

			{
				_TMR_("glCopyTexSubImage2D:\t");
				glCopyTexSubImage2D(GL_TEXTURE_2D, 0, rectDst.x, rectDst.y, rectSrc.x, rectSrc.y, rectDst.width, rectDst.height);
				GL_CHECK_ERROR();
			}

		}
	}

	glDeleteFramebuffers(sizeof(fbo) / sizeof(fbo[0]), fbo);
#endif

}

void untiled(const vector<vector<GlsMat>>& src, GlsMat& dst){
	_TMR_("gls::untiled():\t");

	Size blkNum((int)src[0].size(), (int)src.size());
	Size sizeSrc = Size(src[0][0].cols, src[0][0].rows);
	Size sizeDst = Size(sizeSrc.width * blkNum.width, sizeSrc.height * blkNum.height);


//	dst = GlsMat(sizeDst, src[0][0].type());
	dst = getDstMat(sizeDst, src[0][0].type(),dst);


#ifdef  _GLS_COPY_USE_SHADER
	glsShaderBase* shader = selectShader(src[0][0].type());

	for (int by = 0; by < blkNum.height; by++){
		for (int bx = 0; bx < blkNum.width; bx++){
			Rect rectSrc(-bx* sizeSrc.width, -by* sizeSrc.height, sizeSrc.width, sizeSrc.height);
			Rect rectDst(+bx* sizeSrc.width, +by* sizeSrc.height, sizeSrc.width, sizeSrc.height);
			Vec2f ofs((float)rectSrc.x, (float)rectSrc.y);
			shader->Execute(src[by][bx], ofs, dst , rectDst);
		}
	}
#else
	enum {
		DRAW,
		READ,
		SIZEOF
	};

	GLuint fbo[SIZEOF];		// FBO identifier

	glGenFramebuffers(2, fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo[READ]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo[DRAW]);

	for (int by = 0; by < blkNum.height; by++){
		for (int bx = 0; bx < blkNum.width; bx++){
			Rect rectSrc(0, 0, sizeSrc.width, sizeSrc.height);
			Rect rectDst(+bx* sizeSrc.width, +by* sizeSrc.height, sizeSrc.width, sizeSrc.height);

			glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, src[by][bx].texid(), 0);
			glBindTexture(GL_TEXTURE_2D, dst.texid());

			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, rectDst.x, rectDst.y, rectSrc.x, rectSrc.y, rectSrc.width, rectSrc.height);
			GL_CHECK_ERROR();

		}
	}

	glDeleteFramebuffers(sizeof(fbo) / sizeof(fbo[0]), fbo);

#endif

}

}//namespace gls



