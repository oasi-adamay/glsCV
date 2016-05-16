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


#include "glsSplit.h"


namespace gls
{



//-----------------------------------------------------------------------------
// glsShaderSplit
class glsShaderSplit : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderSplit(void) :glsShaderBase(__FUNCTION__){}

};

//-----------------------------------------------------------------------------
// glsShaderSplitU unsigned
class glsShaderSplitU : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderSplitU(void) :glsShaderBase(__FUNCTION__){}
};

//-----------------------------------------------------------------------------
// glsShaderSplitS unsigned
class glsShaderSplitS : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderSplitS(void) :glsShaderBase(__FUNCTION__){}
};


//-----------------------------------------------------------------------------
//global 
glsShaderSplit ShaderSplit;
glsShaderSplitU ShaderSplitU;
glsShaderSplitS ShaderSplitS;


//-----------------------------------------------------------------------------
//glsShaderSplit
string glsShaderSplit::FragmentShaderCode(void){
	const char fragmentShaderCode[] = 
"#version 330 core\n"
"precision highp float;\n"
"uniform sampler2D	texSrc;\n"
"layout (location = 0) out float dst0;\n"
"layout (location = 1) out float dst1;\n"
"layout (location = 2) out float dst2;\n"
"layout (location = 3) out float dst3;\n"
"void main(void)\n"
"{\n"
"	dst0 = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0).r;\n"
"	dst1 = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0).g;\n"
"	dst2 = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0).b;\n"
"	dst3 = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0).a;\n"
"\n"
"}\n"
;
	return fragmentShaderCode;
}



//-----------------------------------------------------------------------------
//glsShaderSplitU
string glsShaderSplitU::FragmentShaderCode(void){
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform usampler2D	texSrc;\n"
"layout (location = 0) out uint dst0;\n"
"layout (location = 1) out uint dst1;\n"
"layout (location = 2) out uint dst2;\n"
"layout (location = 3) out uint dst3;\n"
"void main(void)\n"
"{\n"
"	dst0 = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0).r;\n"
"	dst1 = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0).g;\n"
"	dst2 = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0).b;\n"
"	dst3 = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0).a;\n"
"}\n"
;
	return fragmentShaderCode;
}


//-----------------------------------------------------------------------------
//glsShaderSplitS
string glsShaderSplitS::FragmentShaderCode(void){
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform isampler2D	texSrc;\n"
"layout (location = 0) out int dst0;\n"
"layout (location = 1) out int dst1;\n"
"layout (location = 2) out int dst2;\n"
"layout (location = 3) out int dst3;\n"
"void main(void)\n"
"{\n"
"	dst0 = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0).r;\n"
"	dst1 = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0).g;\n"
"	dst2 = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0).b;\n"
"	dst3 = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0).a;\n"
"}\n"
;
	return fragmentShaderCode;
}



//---------------------------------------------------------------------------
//
static void splitProcess(
	const glsShaderBase* shader,	//progmra ID
	const GLuint texSrc,			//src texture IDs
	const Rect& rectDst 			//dst rectangel
	)
{

	//program
	{
		glUseProgram(shader->program());
	}

	//uniform
	{
	}


	//Bind Texture
	{
		int id = 0;
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, texSrc);
		glUniform1i(glGetUniformLocation(shader->program(), "texSrc"), id);
	}

	glsVAO vao(glGetAttribLocation(shader->program(), "position"));
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
	case(CV_32F) : shader = &ShaderSplit; break;
	case(CV_8U) :
	case(CV_16U) : shader = &ShaderSplitU; break;
	case(CV_8S) :
	case(CV_16S) :
	case(CV_32S) : shader = &ShaderSplitS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}

//Merge texture
void split(const GlsMat& src, vector<GlsMat>& dst){
	int cn = (int)src.channels();
	GLS_Assert(1 <= cn);
	GLS_Assert(cn <= 4);

	dst.resize(cn);
	for (size_t c = 0; c < cn; c++){
		dst[c] = GlsMat(src.size(), CV_MAKE_TYPE(src.depth(), 1));
	}



	glsShaderBase* shader = selectShader(src.type());

	Rect rectDst(0, 0, src.cols, src.rows);

	glsFBO fbo(cn);

	for (int c = 0; c < cn; c++){
		//dst texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+c, GL_TEXTURE_2D, dst[c].texid(), 0);
		GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}


	splitProcess(shader, src.texid(), rectDst);

}


}	//namespace gls
