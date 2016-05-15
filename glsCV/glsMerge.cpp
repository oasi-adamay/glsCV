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


#include "glsMerge.h"


namespace gls
{



//-----------------------------------------------------------------------------
// glsShaderMerge
class glsShaderMerge : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderMerge(void) :glsShaderBase(__FUNCTION__){}

};

//-----------------------------------------------------------------------------
// glsShaderMergeU unsigned
class glsShaderMergeU : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderMergeU(void) :glsShaderBase(__FUNCTION__){}
};

//-----------------------------------------------------------------------------
// glsShaderMergeS unsigned
class glsShaderMergeS : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderMergeS(void) :glsShaderBase(__FUNCTION__){}
};


//-----------------------------------------------------------------------------
//global 
glsShaderMerge ShaderMerge;
glsShaderMergeU ShaderMergeU;
glsShaderMergeS ShaderMergeS;


//-----------------------------------------------------------------------------
//glsShaderMerge
string glsShaderMerge::FragmentShaderCode(void){
	const char fragmentShaderCode[] = 
"#version 330 core\n"
"precision highp float;\n"
"uniform ivec2	offset;\n"
"uniform sampler2D	texSrc0;\n"
"uniform sampler2D	texSrc1;\n"
"uniform sampler2D	texSrc2;\n"
"uniform sampler2D	texSrc3;\n"
"layout (location = 0) out vec4 dst;\n"
"void main(void)\n"
"{\n"
"	dst.r = texelFetch(texSrc0, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.g = texelFetch(texSrc1, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.b = texelFetch(texSrc2, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.a = texelFetch(texSrc3, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"\n"
"}\n"
;
	return fragmentShaderCode;
}



//-----------------------------------------------------------------------------
//glsShaderMergeU
string glsShaderMergeU::FragmentShaderCode(void){
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform ivec2	offset;\n"
"uniform usampler2D	texSrc0;\n"
"uniform usampler2D	texSrc1;\n"
"uniform usampler2D	texSrc2;\n"
"uniform usampler2D	texSrc3;\n"
"layout (location = 0) out uvec4 dst;\n"
"void main(void)\n"
"{\n"
"	dst.r = texelFetch(texSrc0, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.g = texelFetch(texSrc1, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.b = texelFetch(texSrc2, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.a = texelFetch(texSrc3, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"}\n"
;
	return fragmentShaderCode;
}


//-----------------------------------------------------------------------------
//glsShaderMergeS
string glsShaderMergeS::FragmentShaderCode(void){
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform ivec2	offset;\n"
"uniform isampler2D	texSrc0;\n"
"uniform isampler2D	texSrc1;\n"
"uniform isampler2D	texSrc2;\n"
"uniform isampler2D	texSrc3;\n"
"layout (location = 0) out ivec4 dst;\n"
"void main(void)\n"
"{\n"
"	dst.r = texelFetch(texSrc0, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.g = texelFetch(texSrc1, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.b = texelFetch(texSrc2, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"	dst.a = texelFetch(texSrc3, ivec2(gl_FragCoord.xy)+offset,0).r;\n"
"}\n"
;
	return fragmentShaderCode;
}



//---------------------------------------------------------------------------
//
static void mergeProcess(
	const glsShaderBase* shader,	//progmra ID
	const vector<GLuint>& texSrc,	//src texture IDs
	const Rect& rectSrc,			// Merge src rectangel
	const Rect& rectDst 			// Merge dst rectangel
	)
{
	const int offset[2] = { rectSrc.x, rectSrc.y };

	//program
	{
		glUseProgram(shader->program());
	}

	//uniform
	{
		glUniform2iv(glGetUniformLocation(shader->program(), "offset"), 1,&offset[0]);
	}


	//Bind Texture
	{
		for (int id = 0; id < texSrc.size(); id++){
			glActiveTexture(GL_TEXTURE0 + id);
			glBindTexture(GL_TEXTURE_2D, texSrc[id]);
			string name = "texSrc" + to_string(id);
			glUniform1i(glGetUniformLocation(shader->program(), name.c_str()), id);
		}
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
	case(CV_32F) : shader = &ShaderMerge; break;
	case(CV_8U) :
	case(CV_16U) : shader = &ShaderMergeU; break;
	case(CV_8S) :
	case(CV_16S) :
	case(CV_32S) : shader = &ShaderMergeS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}

//Merge texture
void merge(const vector<GlsMat>& plnSrc, GlsMat& dst){
	int cn = (int)plnSrc.size();
	GLS_Assert(1 <= cn);
	GLS_Assert(cn <= 4);

//	GlsMat _dst = GlsMat(plnSrc[0].size(), CV_MAKE_TYPE(plnSrc[0].depth(), cn));
	GlsMat _dst = getDstMat(plnSrc[0].size(), CV_MAKE_TYPE(plnSrc[0].depth(), cn),dst);

	glsShaderBase* shader = selectShader(plnSrc[0].type());

	Rect rectSrc(0, 0, plnSrc[0].cols, plnSrc[0].rows);
	Rect rectDst = rectSrc;

	glsFBO fbo(1);


	//dst texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _dst.texid(), 0);
	GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	vector<GLuint> texSrc(cn);
	for (int c = 0; c < cn; c++){
		texSrc[c] = plnSrc[c].texid();
	}

	mergeProcess(shader, texSrc, rectSrc, rectDst);

	dst = _dst;
}


}	//namespace gls
