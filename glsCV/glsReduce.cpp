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


#include "glsReduce.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderReduce
class glsShaderReduce : public glsShaderBase
{
public:
	glsShaderReduce(void);

};

#if 0
//-----------------------------------------------------------------------------
// glsShaderReduceU unsigned
class glsShaderReduceU : public glsShaderBase
{
public:
	glsShaderReduceU(void);
};

//-----------------------------------------------------------------------------
// glsShaderReduceS unsigned
class glsShaderReduceS : public glsShaderBase
{
public:
	glsShaderReduceS(void);
};
#endif

//-----------------------------------------------------------------------------
//global 
glsShaderReduce* shaderReduce = 0;
//glsShaderReduceU* shaderReduceU = 0;
//glsShaderReduceS* shaderReduceS = 0;

void ShaderReduceInit(void){
	shaderReduce = new glsShaderReduce();
//	shaderReduceU = new glsShaderReduceU();
//	shaderReduceS = new glsShaderReduceS();
}

void ShaderReduceTerminate(void){
	delete shaderReduce;
//	delete shaderReduceU;
//	delete shaderReduceS;
}


static const char vertexShaderCode[] =
"#version 330 core\n"
"layout (location = 0)in  vec2 position;\n"
"void main(void)\n"
"{\n"
"   gl_Position  = vec4(position,0.0,1.0);\n"
"}\n"
;


//#define CV_REDUCE_SUM 0
//#define CV_REDUCE_AVG 1
//#define CV_REDUCE_MAX 2
//#define CV_REDUCE_MIN 3


//-----------------------------------------------------------------------------
//glsShaderReduce
glsShaderReduce::glsShaderReduce(void)
	:glsShaderBase()
{
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform sampler2D	texSrc;\n"
"uniform int dim;\n"
"uniform int reduceOp;\n"
"layout (location = 0) out vec4 dst;\n"
"#define FLT_MAX  3.402823e+38\n"
"void main(void)\n"
"{\n"
"	ivec2 texSize = textureSize(texSrc,0);\n"
"	vec4 data;\n"
"	if (dim == 1){\n"
"		switch (reduceOp){\n"
"		case(0) : {  //CV_REDUCE_SUM\n"
"			vec4 sum = vec4(0.0);\n"
"			for (int x = 0; x < texSize.x; x++){\n"
"				data = texelFetch(texSrc, ivec2(x, gl_FragCoord.y), 0);\n"
"				sum = sum + data;\n"
"			}\n"
"			dst = sum;\n"
"		}break;\n"
"		case(1) : { //CV_REDUCE_AVE\n"
"			vec4 sum = vec4(0.0);\n"
"			for (int x = 0; x < texSize.x; x++){\n"
"				data = texelFetch(texSrc, ivec2(x, gl_FragCoord.y), 0);\n"
"				sum = sum + data;\n"
"			}\n"
"			dst = sum / vec4(texSize.x);\n"
"		}break;\n"
"		case(2) : { //CV_REDUCE_MAX\n"
"			vec4 maxv = vec4(-FLT_MAX);\n"
"			for (int x = 0; x < texSize.x; x++){\n"
"				data = texelFetch(texSrc, ivec2(x, gl_FragCoord.y), 0);\n"
"				maxv = max(data,maxv);\n"
"			}\n"
"			dst = maxv;\n"
"		}break;\n"
"		case(3) : { //CV_REDUCE_MIN\n"
"			vec4 minv = vec4(FLT_MAX);\n"
"			for (int x = 0; x < texSize.x; x++){\n"
"				data = texelFetch(texSrc, ivec2(x, gl_FragCoord.y), 0);\n"
"				minv = min(data,minv);\n"
"			}\n"
"			dst = minv;\n"
"		}break;\n"
"		}\n"
"	}\n"
"	else {	//dim==0\n"
"		switch (reduceOp){\n"
"		case(0) : {  //CV_REDUCE_SUM\n"
"			vec4 sum = vec4(0.0);\n"
"			for (int y = 0; y < texSize.y; y++){\n"
"				data = texelFetch(texSrc, ivec2(gl_FragCoord.x, y), 0);\n"
"				sum = sum + data;\n"
"			}\n"
"			dst = sum;\n"
"		}break;\n"
"		case(1) : { //CV_REDUCE_AVE\n"
"			vec4 sum = vec4(0.0);\n"
"			for (int y = 0; y < texSize.y; y++){\n"
"				data = texelFetch(texSrc, ivec2(gl_FragCoord.x, y), 0);\n"
"				sum = sum + data;\n"
"			}\n"
"			dst = sum / vec4(texSize.y);\n"
"		}break;\n"
"		case(2) : { //CV_REDUCE_MAX\n"
"			vec4 maxv = vec4(-FLT_MAX);\n"
"			for (int y = 0; y < texSize.y; y++){\n"
"				data = texelFetch(texSrc, ivec2(gl_FragCoord.x, y), 0);\n"
"				maxv = max(data,maxv);\n"
"			}\n"
"			dst = maxv;\n"
"		}break;\n"
"		case(3) : { //CV_REDUCE_MIN\n"
"			vec4 minv = vec4(FLT_MAX);\n"
"			for (int y = 0; y < texSize.y; y++){\n"
"				data = texelFetch(texSrc, ivec2(gl_FragCoord.x, y), 0);\n"
"				minv = min(data,minv);\n"
"			}\n"
"			dst = minv;\n"
"		}break;\n"
"		}\n"
"	}\n"
"}\n"
;

	// Create and compile our GLSL program from the shaders
	LoadShadersCode(vertexShaderCode, fragmentShaderCode);
}



//dim – 行列が縮小される際に従う次元インデックス．0 は行列が1行 に，1 は行列が1列に縮小されることをそれぞれ意味します．
//---------------------------------------------------------------------------
static void glsReduceProcess(
	const glsShaderBase* shader,	//progmra ID
	const GLuint& texSrc,			//src texture IDs
	const Size& texSize,			//texture size
	const int dim,					//reduce dir
	const int reduceOp				//reduceOP
	)
{

	//program
	{
		glUseProgram(shader->program);
	}

	//uniform
	{
		glUniform1i(glGetUniformLocation(shader->program, "dim"), dim);
		glUniform1i(glGetUniformLocation(shader->program, "reduceOp"), reduceOp);
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
	if (dim == 0){
		glViewport(0, 0, texSize.width, 1);
	}
	else{
		glViewport(0, 0, 1, texSize.height);
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
	case(CV_32F) : shader = shaderReduce; break;
	//case(CV_8U) :
	//case(CV_16U) : shader = shaderReduceU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = shaderReduceS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}

//Reduce texture
void reduce(const GlsMat& src, GlsMat& dst, int dim, int reduceOp){
	GLS_Assert(src.depth() == CV_32F);


	GlsMat _src = src;


	GlsMat _dst;
	if (dim == 0){
		_dst = GlsMat(Size(_src.cols, 1), _src.type());
	}
	else{
		_dst = GlsMat(Size(1, _src.rows), _src.type());
	}


	glsShaderBase* shader = selectShader(_src.type());

	{
		glsFBO fbo(1);

		//dst texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _dst.texid(), 0);
		GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		glsReduceProcess(shader, _src.texid(), _src.size(), dim, reduceOp);


	}



	dst = _dst;
}


void minMaxLoc(const GlsMat& src, double* minVal, double* maxVal, Point* minLoc, Point* maxLoc, const GlsMat& mask){
	GLS_Assert(src.depth() == CV_32F);
	GLS_Assert(maxLoc == 0);	// not implement yet
	GLS_Assert(minLoc == 0);	// not implement yet
	GLS_Assert(mask.empty());	// not implement yet

	GlsMat _src = src;


	GlsMat tmp;

	if (minVal){
		gls::reduce(_src, tmp, 0, CV_REDUCE_MIN);
		gls::reduce(tmp, tmp, 1, CV_REDUCE_MIN);

		Mat val;
		tmp.CopyTo(val);
		GLS_Assert(val.rows == 1);
		GLS_Assert(val.cols == 1);
		*minVal = val.at<float>(0, 0);
	}

	if (maxVal){
		gls::reduce(_src, tmp, 0, CV_REDUCE_MAX);
		gls::reduce(tmp, tmp, 1, CV_REDUCE_MAX);

		Mat val;
		tmp.CopyTo(val);
		GLS_Assert(val.rows == 1);
		GLS_Assert(val.cols == 1);
		*maxVal = val.at<float>(0, 0);
	}
}

}//namespace gls





