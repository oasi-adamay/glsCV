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


#include "glsMinMaxLoc.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderMinMaxLoc
class glsShaderMinMaxLoc : public glsShaderBase
{
public:
	glsShaderMinMaxLoc(void);

};

#if 0
//-----------------------------------------------------------------------------
// glsShaderMinMaxLocU unsigned
class glsShaderMinMaxLocU : public glsShaderBase
{
public:
	glsShaderMinMaxLocU(void);
};

//-----------------------------------------------------------------------------
// glsShaderMinMaxLocS unsigned
class glsShaderMinMaxLocS : public glsShaderBase
{
public:
	glsShaderMinMaxLocS(void);
};
#endif

//-----------------------------------------------------------------------------
//global 
glsShaderMinMaxLoc* shaderMinMaxLoc = 0;
//glsShaderMinMaxLocU* shaderMinMaxLocU = 0;
//glsShaderMinMaxLocS* shaderMinMaxLocS = 0;

void ShaderMinMaxLocInit(void){
	shaderMinMaxLoc = new glsShaderMinMaxLoc();
//	shaderMinMaxLocU = new glsShaderMinMaxLocU();
//	shaderMinMaxLocS = new glsShaderMinMaxLocS();
}

void ShaderMinMaxLocTerminate(void){
	delete shaderMinMaxLoc;
//	delete shaderMinMaxLocU;
//	delete shaderMinMaxLocS;
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
//glsShaderMinMaxLoc
glsShaderMinMaxLoc::glsShaderMinMaxLoc(void)
	:glsShaderBase()
{
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform sampler2D	texSrc0;\n"
"uniform sampler2D	texSrc1;\n"
"uniform isampler2D	texSrc2;\n"
"uniform isampler2D	texSrc3;\n"
"uniform int		path;\n"
"layout (location = 0) out float minval;\n"
"layout (location = 1) out float maxval;\n"
"layout (location = 2) out ivec2 minloc;\n"
"layout (location = 3) out ivec2 maxloc;\n"
"#define FLT_MAX  3.402823e+38\n"
"void main(void)\n"
"{\n"
"	ivec2 texSize = textureSize(texSrc0,0);\n"
"	if(path==0){\n"
"		float minv = float (FLT_MAX);\n"
"		float maxv = float(-FLT_MAX);\n"
"		int minl = 0;\n"
"		int maxl = 0;\n"
"		float data;\n"
"		for (int x = 0; x < texSize.x; x++){\n"
"			data = texelFetch(texSrc0, ivec2(x, gl_FragCoord.y), 0).r;\n"
"			if(data<minv) {minv = data; minl = x;}\n"
"			if(data>maxv) {maxv = data; maxl = x;}\n"
"		}\n"
"		minval = minv;\n"
"		maxval = maxv;\n"
"		minloc = ivec2(minl,0);\n"
"		maxloc = ivec2(maxl,0);\n"
"	}\n"
"	else{\n"
"		float minv = float (FLT_MAX);\n"
"		float maxv = float(-FLT_MAX);\n"
"		int minl = 0;\n"
"		int maxl = 0;\n"
"		float dmin;\n"
"		float dmax;\n"
"		for (int y = 0; y < texSize.y; y++){\n"
"			dmin = texelFetch(texSrc0, ivec2(0,y), 0).r;\n"
"			dmax = texelFetch(texSrc1, ivec2(0,y), 0).r;\n"
"			if(dmin<minv) {minv = dmin; minl = y;}\n"
"			if(dmax>maxv) {maxv = dmax; maxl = y;}\n"
"		}\n"
"		minval = minv;\n"
"		maxval = maxv;\n"
"		minloc = ivec2(texelFetch(texSrc2, ivec2(0,minl), 0).r,minl);\n"
"		maxloc = ivec2(texelFetch(texSrc3, ivec2(0,maxl), 0).r,maxl);\n"
"	}\n"
"}\n"
;

	// Create and compile our GLSL program from the shaders
	LoadShadersCode(vertexShaderCode, fragmentShaderCode);
}



//---------------------------------------------------------------------------
/*!
各rowごとのminmaxloc
*/
static void glsMinMaxLocProcess(
	const glsShaderBase* shader,	//progmra ID
	const vector<GLuint>& texSrc,			//src texture IDs
	const Size& texSize,				//dst texture size
	const int path					// path id
	)
{

	//program
	{
		glUseProgram(shader->program);
	}

	//uniform
	{
		glUniform1i(glGetUniformLocation(shader->program, "path"), path);
	}

	//Bind Texture
	for (int id = 0; id < (int)texSrc.size();id++){
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, texSrc[id]);
		string name = "texSrc" + to_string(id);
		glUniform1i(glGetUniformLocation(shader->program, name.c_str()), id);
	}

	glsVAO vao(glGetAttribLocation(shader->program, "position"));

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
	case(CV_32F) : shader = shaderMinMaxLoc; break;
	//case(CV_8U) :
	//case(CV_16U) : shader = shaderMinMaxLocU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = shaderMinMaxLocS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}



void minMaxLoc(const GlsMat& src, double* minVal, double* maxVal, Point* minLoc, Point* maxLoc, const GlsMat& mask){
	GLS_Assert(src.depth() == CV_32F);
	GLS_Assert(mask.empty());	// not implement yet

	GlsMat _minvec;
	GlsMat _maxvec;
	GlsMat _minloc;
	GlsMat _maxloc;

	if (minVal) _minvec = GlsMat(Size(1, src.rows), CV_MAKE_TYPE(src.depth(), 1));
	if (maxVal) _maxvec = GlsMat(Size(1, src.rows), CV_MAKE_TYPE(src.depth(), 1));
	if (minLoc) _minloc = GlsMat(Size(1, src.rows), CV_32SC1);
	if (maxLoc) _maxloc = GlsMat(Size(1, src.rows), CV_32SC1);

	glsShaderBase* shader = selectShader(src.type());

	glsFBO fbo(4);
	{		//path:0
		//src texture
		vector<GLuint> texArray(1);
		texArray[0] = src.texid();

		//dst texture
		if (minVal)glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _minvec.texid(), 0);
		if (maxVal)glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _maxvec.texid(), 0);
		if (minLoc)glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _minloc.texid(), 0);
		if (maxLoc)glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _maxloc.texid(), 0);
		GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		glsMinMaxLocProcess(shader, texArray, Size(1, src.rows), 0);
	}
#if 1
	{		//path:1
		GlsMat _minvec1;
		GlsMat _maxvec1;
		GlsMat _minloc1;
		GlsMat _maxloc1;

		if (minVal) _minvec1 = GlsMat(Size(1, 1), CV_MAKE_TYPE(src.depth(), 1));
		if (maxVal) _maxvec1 = GlsMat(Size(1, 1), CV_MAKE_TYPE(src.depth(), 1));
		if (minLoc) _minloc1 = GlsMat(Size(1, 1), CV_32SC2);
		if (maxLoc) _maxloc1 = GlsMat(Size(1, 1), CV_32SC2);

		//src texture
		vector<GLuint> texArray(4);
		texArray[0] = _minvec.texid();
		texArray[1] = _maxvec.texid();
		texArray[2] = _minloc.texid();
		texArray[3] = _maxloc.texid();

		//dst texture
		if (minVal)glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _minvec1.texid(), 0);
		if (maxVal)glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _maxvec1.texid(), 0);
		if (minLoc)glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _minloc1.texid(), 0);
		if (maxLoc)glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _maxloc1.texid(), 0);
		GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		glsMinMaxLocProcess(shader, texArray, Size(1,1), 1);

		Mat minval;
		Mat maxval;
		Mat minloc;
		Mat maxloc;
		if (minVal) _minvec1.CopyTo(minval);
		if (maxVal) _maxvec1.CopyTo(maxval);
		if (minLoc) _minloc1.CopyTo(minloc);
		if (maxLoc) _maxloc1.CopyTo(maxloc);

		if (minVal) *minVal = minval.at<float>(0, 0);
		if (maxVal) *maxVal = maxval.at<float>(0, 0);
		if (minLoc) *minLoc = minloc.at<Vec2i>(0, 0);
		if (maxLoc) *maxLoc = maxloc.at<Vec2i>(0, 0);
	}

#else
	Mat minvec_row;
	Mat maxvec_row;
	Mat minloc_row;
	Mat maxloc_row;
	if (minVal) _minvec.CopyTo(minvec_row);
	if (maxVal) _maxvec.CopyTo(maxvec_row);
	if (minLoc) _minloc.CopyTo(minloc_row);
	if (maxLoc) _maxloc.CopyTo(maxloc_row);

	//cout << minvec_row << endl;
	//cout << maxvec_row << endl;
	//cout << minloc_row << endl;
	//cout << maxloc_row << endl;


	///! col方向のminMaxはcvの関数で
	cv::minMaxLoc(minvec_row, minVal, 0, minLoc, 0);
	cv::minMaxLoc(maxvec_row, 0, maxVal, 0, maxLoc);
	if (minLoc) minLoc->x = minloc_row.at<int>(minLoc->y);
	if (maxLoc) maxLoc->x = maxloc_row.at<int>(maxLoc->y);

#endif


}

}//namespace gls





