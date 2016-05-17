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


#include "glsNorm.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderNorm
class glsShaderNorm : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderNorm(void) :glsShaderBase(__FUNCTION__){}

};

//-----------------------------------------------------------------------------
//global 
glsShaderNorm ShaderNorm;

//NORM_INF = 1, NORM_L1 = 2, NORM_L2 = 4

//-----------------------------------------------------------------------------
//glsShaderNorm
string glsShaderNorm::FragmentShaderCode(void){
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform sampler2D	texSrc;\n"
"uniform int cn;\n"
"uniform int normType;\n"
"layout (location = 0) out float dst;\n"
"#define FLT_MAX  3.402823e+38\n"
"#define NORM_INF 1\n"
"#define NORM_L1 2\n"
"#define NORM_L2 4\n"
"void main(void)\n"
"{\n"
"	ivec2 texSize = textureSize(texSrc,0);\n"
"	vec4 data;\n"
"	switch (normType){\n"
"	case(NORM_INF) : {\n"
"		vec4 maxv = vec4(-FLT_MAX);\n"
"		for (int x = 0; x < texSize.x; x++){\n"
"			data = texelFetch(texSrc, ivec2(x, gl_FragCoord.y), 0);\n"
"			data = abs(data);\n"
"			maxv = max(data,maxv);\n"
"		}\n"
"		switch (cn){\n"
"		case(1) : dst = maxv.r ; break;\n"
"		case(2) : dst = max(maxv.r,maxv.g); break;\n"
"		case(3) : dst = max(max(maxv.r,maxv.g),maxv.b); break;\n"
"		case(4) : dst = max(max(maxv.r,maxv.g),max(maxv.b,maxv.a)); break;\n"
"		}\n"
"	}break;\n"
"	case(NORM_L1) : {\n"
"		vec4 sumv = vec4(0.0);\n"
"		for (int x = 0; x < texSize.x; x++){\n"
"			data = texelFetch(texSrc, ivec2(x, gl_FragCoord.y), 0);\n"
"			data = abs(data);\n"
"			sumv = sumv + data;\n"
"		}\n"
"		switch (cn){\n"
"		case(1) : dst = sumv.r ; break;\n"
"		case(2) : dst = sumv.r+sumv.g; break;\n"
"		case(3) : dst = sumv.r+sumv.g+sumv.b; break;\n"
"		case(4) : dst = sumv.r+sumv.g+sumv.b+sumv.a; break;\n"
"		}\n"
"	}break;\n"
"	case(NORM_L2) : {\n"
"		vec4 sumv = vec4(0.0);\n"
"		for (int x = 0; x < texSize.x; x++){\n"
"			data = texelFetch(texSrc, ivec2(x, gl_FragCoord.y), 0);\n"
"			data = data * data;\n"
"			sumv = sumv + data;\n"
"		}\n"
"		switch (cn){\n"
"		case(1) : dst = sumv.r ; break;\n"
"		case(2) : dst = sumv.r+sumv.g; break;\n"
"		case(3) : dst = sumv.r+sumv.g+sumv.b; break;\n"
"		case(4) : dst = sumv.r+sumv.g+sumv.b+sumv.a; break;\n"
"		}\n"
"	}break;\n"
"	}\n"
"}\n"
;
	return fragmentShaderCode;
}



//---------------------------------------------------------------------------
static void glsNormProcess(
	const glsShaderBase* shader,	//progmra ID
	const GLuint& texSrc,			//src texture IDs
	const Size& texSize,			//texture size
	const int cn,					//channels
	const int normType				//normType
	)
{

	//program
	{
		glUseProgram(shader->program());
	}

	//uniform
	{
		glUniform1i(glGetUniformLocation(shader->program(), "cn"), cn);
		glUniform1i(glGetUniformLocation(shader->program(), "normType"), normType);
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
	glViewport(0, 0, texSize.width, texSize.height);

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
	case(CV_32F) : shader = &ShaderNorm; break;
	//case(CV_8U) :
	//case(CV_16U) : shader = &ShaderNormU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = &ShaderNormS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}


double norm(const GlsMat& src1, int normType){
	GLS_Assert(src1.depth() == CV_32F);
	GLS_Assert(normType == NORM_INF || normType == NORM_L1 || normType == NORM_L2);


	GlsMat _tmp;
	_tmp = GlsMat(Size(1, src1.rows), CV_MAKE_TYPE(src1.depth(),1));


	glsShaderBase* shader = selectShader(src1.type());

	{
		glsFBO fbo(1);

		//dst texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tmp.texid(), 0);
		GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glsNormProcess(shader, src1.texid(), _tmp.size(), src1.channels(),normType);

	}

	{
		Mat tmp = (Mat)_tmp;
		cout << tmp << endl;
	}

	GlsMat _val;
	if (normType == NORM_INF){
		gls::reduce(_tmp, _val, 0, CV_REDUCE_MAX);
	}
	else{
		gls::reduce(_tmp, _val, 0, CV_REDUCE_SUM);
	}
	float val = ((Mat)_val).at<float>(0, 0);
	if (normType == NORM_L2) val = sqrt(val);
	return val;

}

}//namespace gls





