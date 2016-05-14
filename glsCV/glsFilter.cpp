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


#include "glsFilter.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderFilter1D
class glsShaderFilter1D : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderFilter1D(void);

};

//-----------------------------------------------------------------------------
// glsShaderFilter2D
class glsShaderFilter2D : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderFilter2D(void);

};

#if 0
//-----------------------------------------------------------------------------
// glsShaderFilterU unsigned
class glsShaderFilterU : public glsShaderBase
{
public:
	glsShaderFilterU(void);
};

//-----------------------------------------------------------------------------
// glsShaderFilterS unsigned
class glsShaderFilterS : public glsShaderBase
{
public:
	glsShaderFilterS(void);
};
#endif

//-----------------------------------------------------------------------------
//global 
glsShaderFilter1D* shaderFilter1D = 0;
//glsShaderFilterU* shaderFilterU = 0;
//glsShaderFilterS* shaderFilterS = 0;

glsShaderFilter2D* shaderFilter2D = 0;
//glsShaderFilterU* shaderFilterU = 0;
//glsShaderFilterS* shaderFilterS = 0;


void ShaderFilterInit(void){
	shaderFilter1D = new glsShaderFilter1D();
	shaderFilter2D = new glsShaderFilter2D();

//	shaderFilterU = new glsShaderFilterU();
//	shaderFilterS = new glsShaderFilterS();
}

void ShaderFilterTerminate(void){
	delete shaderFilter1D;
	delete shaderFilter2D;
//	delete shaderFilterU;
//	delete shaderFilterS;
}




//-----------------------------------------------------------------------------
//glsShaderFilter1D
string glsShaderFilter1D::FragmentShaderCode(void){
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform sampler2D	texSrc;\n"
"uniform sampler2D	texKernel;\n"
"layout (location = 0) out vec4 dst;\n"
"#define BOUND_PVT(x,pl,pr) ((x)<(pl)?2*(pl)-(x) :(x)>(pr)? 2*(pr)-(x): (x))\n"
"void main(void)\n"
"{\n"
"	ivec2 texSize = textureSize(texSrc,0);\n"
"	ivec2 texKernelSize = textureSize(texKernel,0);\n"
"	if(texKernelSize.y==1){\n"
"		int ksize = texKernelSize.x ;\n"
"		int kp = ksize / 2 ;\n"
"		int km = -kp;\n"
"		vec4 sum = vec4(0.0,0.0,0.0,0.0);\n"
"		for (int k = km; k <= kp; k++){\n"
"			vec4 data;\n"
"			vec4 coef;\n"
"			int x = int(gl_FragCoord.x) + k;\n"
"			x = BOUND_PVT(x,0,texSize.x-1);\n"
"			data = texelFetch(texSrc, ivec2(x, gl_FragCoord.y), 0);\n"
"			coef = vec4(texelFetch(texKernel, ivec2(k+kp, 0), 0).r);\n"
"			sum += data * coef;\n"
"		}\n"
"		dst = sum;\n"
"	}\n"
"	else{\n"
"		int ksize = texKernelSize.y ;\n"
"		int kp = ksize / 2 ;\n"
"		int km = -kp;\n"
"		vec4 sum = vec4(0.0,0.0,0.0,0.0);\n"
"		for (int k = km; k <= kp; k++){\n"
"			vec4 data;\n"
"			vec4 coef;\n"
"			int y = int(gl_FragCoord.y) + k;\n"
"			y = BOUND_PVT(y,0,texSize.y-1);\n"
"			data = texelFetch(texSrc, ivec2(gl_FragCoord.x,y), 0);\n"
"			coef = vec4(texelFetch(texKernel, ivec2(0,k+kp), 0).r);\n"
"			sum += data * coef;\n"
"		}\n"
"		dst = sum;\n"
"	}\n"
"}\n"
;
	return fragmentShaderCode;
}

glsShaderFilter1D::glsShaderFilter1D(void)
	:glsShaderBase()
{

	const string bin_filename = shaderBinName(__FUNCTION__);
	if (!LoadShadersBinary(bin_filename))
	{
		LoadShadersCode(VertexShaderCode(), FragmentShaderCode(), bin_filename);
	}
}


//-----------------------------------------------------------------------------
//glsShaderFilter2D
string glsShaderFilter2D::FragmentShaderCode(void){
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform sampler2D	texSrc;\n"
"uniform sampler2D	texKernel;\n"
"layout (location = 0) out vec4 dst;\n"
"#define BOUND_PVT(x,pl,pr) ((x)<(pl)?2*(pl)-(x) :(x)>(pr)? 2*(pr)-(x): (x))\n"
"void main(void)\n"
"{\n"
"	ivec2 texSize = textureSize(texSrc,0);\n"
"	ivec2 texKernelSize = textureSize(texKernel,0);\n"
"	{\n"
"		int kxsize = texKernelSize.x ;\n"
"		int kysize = texKernelSize.y ;\n"
"		int kxp = kxsize / 2 ;\n"
"		int kxm = -kxp;\n"
"		int kyp = kysize / 2 ;\n"
"		int kym = -kyp;\n"
"		vec4 sum = vec4(0.0,0.0,0.0,0.0);\n"
"		for (int ky = kym; ky <= kyp; ky++){\n"
"				for (int kx = kxm; kx <= kxp; kx++){\n"
"				vec4 data;\n"
"				vec4 coef;\n"
"				int x = int(gl_FragCoord.x) + kx;\n"
"				int y = int(gl_FragCoord.y) + ky;\n"
"				x = BOUND_PVT(x,0,texSize.x-1);\n"
"				y = BOUND_PVT(y,0,texSize.y-1);\n"
"				data = texelFetch(texSrc, ivec2(x, y), 0);\n"
"				coef = vec4(texelFetch(texKernel, ivec2(kx+kxp, ky+kyp), 0).r);\n"
"				sum += data * coef;\n"
"			}\n"
"		}\n"
"		dst = sum;\n"
"	}\n"
"}\n"
;
	return fragmentShaderCode;
}

glsShaderFilter2D::glsShaderFilter2D(void)
	:glsShaderBase()
{

	const string bin_filename = shaderBinName(__FUNCTION__);
	if (!LoadShadersBinary(bin_filename))
	{
		LoadShadersCode(VertexShaderCode(), FragmentShaderCode(), bin_filename);
	}
}



//---------------------------------------------------------------------------
/*!
*/
static void glsFilter1DProcess(
	const glsShaderBase* shader,	//progmra ID
	const GLuint& texSrc,			//src   texture ID
	const GLuint& texKernel,		//Kernel texture ID // row ,col vector
	const Size& texSize				//dst texture size
	)
{

	//program
	{
		glUseProgram(shader->program);
	}

	//uniform
	{
	}

	//Bind Texture
	{
		int id = 0;
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, texSrc);
		glUniform1i(glGetUniformLocation(shader->program, "texSrc"), id);
		id++;
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, texKernel);
		glUniform1i(glGetUniformLocation(shader->program, "texKernel"), id);

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

static void glsFilter2DProcess(
	const glsShaderBase* shader,	//progmra ID
	const GLuint& texSrc,			//src   texture ID
	const GLuint& texKernel,		//Kernel texture ID
	const Size& texSize				//dst texture size
	)
{

	//program
	{
		glUseProgram(shader->program);
	}

	//uniform
	{
	}

	//Bind Texture
	{
		int id = 0;
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, texSrc);
		glUniform1i(glGetUniformLocation(shader->program, "texSrc"), id);
		id++;
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, texKernel);
		glUniform1i(glGetUniformLocation(shader->program, "texKernel"), id);

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
glsShaderBase* selectShader1D(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = shaderFilter1D; break;
	//case(CV_8U) :
	//case(CV_16U) : shader = shaderFilterU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = shaderFilterS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}

static
glsShaderBase* selectShader2D(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = shaderFilter2D; break;
		//case(CV_8U) :
		//case(CV_16U) : shader = shaderFilterU; break;
		//case(CV_8S) :
		//case(CV_16S) :
		//case(CV_32S) : shader = shaderFilterS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}



void sepFilter2D(const GlsMat& src, GlsMat& dst, int ddepth, const Mat& kernelX, const Mat& kernelY){
	GLS_Assert(src.depth() == CV_32F);
	GLS_Assert(kernelX.rows == 1 || kernelX.cols == 1);
	GLS_Assert(kernelY.rows == 1 || kernelY.cols == 1);

	GlsMat _kernelX;
	GlsMat _kernelY;
	if (kernelX.rows == 1) _kernelX = (GlsMat)kernelX;
	else  _kernelX = (GlsMat)kernelX.reshape(0, 1);

	if (kernelY.cols == 1) _kernelY = (GlsMat)kernelY;
	else _kernelY = (GlsMat)kernelY.reshape(0, kernelY.cols);


	GlsMat _dst;
	if (&src == &dst || src.size() != dst.size() || src.type() != dst.type()){
		_dst = GlsMat(src.size(), src.type());
	}
	else{
		_dst = dst;
	}

	GlsMat _tmp = GlsMat(src.size(), src.type());

	glsShaderBase* shader = selectShader1D(src.type());

	glsFBO fbo(1);
	{
		//row
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tmp.texid(), 0);
		glsFilter1DProcess(shader, src.texid(), _kernelX.texid(), src.size());	

		//col
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _dst.texid(), 0);
		glsFilter1DProcess(shader, _tmp.texid(), _kernelY.texid(), src.size());	//col

	}

	dst = _dst;
}

void filter2D(const GlsMat& src, GlsMat& dst, int ddepth, const Mat& kernel){
	GLS_Assert(src.depth() == CV_32F);
	GLS_Assert(kernel.type() == CV_32FC1);

	GlsMat _kernel = (GlsMat)kernel;

	GlsMat _dst;
	if (&src == &dst || src.size() != dst.size() || src.type() != dst.type()){
		_dst = GlsMat(src.size(), src.type());
	}
	else{
		_dst = dst;
	}

	glsShaderBase* shader = selectShader2D(src.type());

	glsFBO fbo(1);
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _dst.texid(), 0);
		glsFilter2DProcess(shader, src.texid(), _kernel.texid(), src.size());
	}

	dst = _dst;
}


void GaussianBlur(const GlsMat& src, GlsMat& dst, Size ksize, double sigmaX, double sigmaY){

	Mat kernelX = cv::getGaussianKernel(ksize.width, sigmaX, CV_32F);
	Mat kernelY = cv::getGaussianKernel(ksize.height, sigmaY, CV_32F);

	gls::sepFilter2D(src, dst, src.depth(), kernelX, kernelY);
}

void boxFilter(const GlsMat& src, GlsMat& dst, int ddepth, Size ksize){
	Mat kernelX = Mat::ones(Size(ksize.width, 1), CV_32F) / (float)ksize.width;
	Mat kernelY = Mat::ones(Size(1, ksize.height), CV_32F) / (float)ksize.height;

	gls::sepFilter2D(src, dst, ddepth, kernelX, kernelY);
}

void Sobel(const GlsMat& src, GlsMat& dst, int ddepth, int xorder, int yorder, int ksize){

	Mat kernelX;
	Mat kernelY;

	cv::getDerivKernels(kernelX, kernelY, xorder, yorder, ksize, false, CV_32F);

	gls::sepFilter2D(src, dst, ddepth, kernelX, kernelY);

}



}//namespace gls





