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

#include "glsFilter.h"
#include "glsBasicOperation.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderFilter1D
class glsShaderFilter1D : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);
	list<string> UniformNameList(void);

public:
	glsShaderFilter1D(void) : glsShaderBase(__FUNCTION__){}

};

//-----------------------------------------------------------------------------
// glsShaderFilter1DU
class glsShaderFilter1DU : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);
	list<string> UniformNameList(void);

public:
	glsShaderFilter1DU(void) : glsShaderBase(__FUNCTION__){}

};


//-----------------------------------------------------------------------------
// glsShaderFilter2D
class glsShaderFilter2D : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);
	list<string> UniformNameList(void);

public:
	glsShaderFilter2D(void) : glsShaderBase(__FUNCTION__){}

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
glsShaderFilter1D ShaderFilter1D;
glsShaderFilter1DU ShaderFilter1DU;
glsShaderFilter2D ShaderFilter2D;

//-----------------------------------------------------------------------------
//glsShaderFilter1D
string glsShaderFilter1D::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;\n
uniform sampler2D	texSrc;\n
uniform sampler2D	texKernel;\n
uniform float delta;\n
layout (location = 0) out vec4 dst;\n
#define BOUND_PVT(x,pl,pr) ((x)<(pl)?2*(pl)-(x) :(x)>(pr)? 2*(pr)-(x): (x))\n
void main(void)\n
{\n
	ivec2 texSize = textureSize(texSrc,0);\n
	ivec2 texKernelSize = textureSize(texKernel,0);\n
	if(texKernelSize.y==1){\n
		int ksize = texKernelSize.x ;\n
		int kp = ksize / 2 ;\n
		int km = -kp;\n
		vec4 sum = vec4(0.0,0.0,0.0,0.0);\n
		for (int k = km; k <= kp; k++){\n
			vec4 data;\n
			vec4 coef;\n
			int x = int(gl_FragCoord.x) + k;\n
			x = BOUND_PVT(x,0,texSize.x-1);\n
			data = texelFetch(texSrc, ivec2(x, gl_FragCoord.y), 0);\n
			coef = vec4(texelFetch(texKernel, ivec2(k+kp, 0), 0).r);\n
			sum += data * coef;\n
		}\n
		dst = sum + delta; \n
	}\n
	else{\n
		int ksize = texKernelSize.y ;\n
		int kp = ksize / 2 ;\n
		int km = -kp;\n
		vec4 sum = vec4(0.0,0.0,0.0,0.0);\n
		for (int k = km; k <= kp; k++){\n
			vec4 data;\n
			vec4 coef;\n
			int y = int(gl_FragCoord.y) + k;\n
			y = BOUND_PVT(y,0,texSize.y-1);\n
			data = texelFetch(texSrc, ivec2(gl_FragCoord.x,y), 0);\n
			coef = vec4(texelFetch(texKernel, ivec2(0,k+kp), 0).r);\n
			sum += data * coef;\n
		}\n
		dst = sum + delta; \n
	}\n
}\n
);
	return fragmentShaderCode;
}

list<string> glsShaderFilter1D::UniformNameList(void){
	list<string> lst;
	lst.push_back("texSrc");
	lst.push_back("texKernel");
	lst.push_back("delta");
	return lst;
}


//-----------------------------------------------------------------------------
//glsShaderFilter1DU
string glsShaderFilter1DU::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;\n
uniform usampler2D	texSrc;\n
uniform sampler2D	texKernel;\n
uniform float delta; \n
layout (location = 0) out uvec4 dst;\n
#define BOUND_PVT(x,pl,pr) ((x)<(pl)?2*(pl)-(x) :(x)>(pr)? 2*(pr)-(x): (x))\n
void main(void)\n
{\n
	ivec2 texSize = textureSize(texSrc,0);\n
	ivec2 texKernelSize = textureSize(texKernel,0);\n
	if(texKernelSize.y==1){\n
		int ksize = texKernelSize.x ;\n
		int kp = ksize / 2 ;\n
		int km = -kp;\n
		vec4 sum = vec4(0.0,0.0,0.0,0.0);\n
		for (int k = km; k <= kp; k++){\n
			vec4 data;\n
			vec4 coef;\n
			int x = int(gl_FragCoord.x) + k;\n
			x = BOUND_PVT(x,0,texSize.x-1);\n
			data = vec4(texelFetch(texSrc, ivec2(x, gl_FragCoord.y), 0));\n
			coef = vec4(texelFetch(texKernel, ivec2(k+kp, 0), 0).r);\n
			sum += data * coef;\n
		}\n
		dst = uvec4(roundEven(sum + delta)); \n
	}\n
	else{\n
		int ksize = texKernelSize.y ;\n
		int kp = ksize / 2 ;\n
		int km = -kp;\n
		vec4 sum = vec4(0.0,0.0,0.0,0.0);\n
		for (int k = km; k <= kp; k++){\n
			vec4 data;\n
			vec4 coef;\n
			int y = int(gl_FragCoord.y) + k;\n
			y = BOUND_PVT(y,0,texSize.y-1);\n
			data = vec4(texelFetch(texSrc, ivec2(gl_FragCoord.x,y), 0));\n
			coef = vec4(texelFetch(texKernel, ivec2(0,k+kp), 0).r);\n
			sum += data * coef;\n
		}\n
		dst = uvec4(roundEven(sum + delta)); \n
	}\n
}\n
);
	return fragmentShaderCode;
}

list<string> glsShaderFilter1DU::UniformNameList(void){
	list<string> lst;
	lst.push_back("texSrc");
	lst.push_back("texKernel");
	lst.push_back("delta");
	return lst;
}


//-----------------------------------------------------------------------------
//glsShaderFilter2D
string glsShaderFilter2D::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;\n
uniform sampler2D	texSrc;\n
uniform sampler2D	texKernel;\n
uniform float delta; \n
layout (location = 0) out vec4 dst;\n
#define BOUND_PVT(x,pl,pr) ((x)<(pl)?2*(pl)-(x) :(x)>(pr)? 2*(pr)-(x): (x))\n
void main(void)\n
{\n
	ivec2 texSize = textureSize(texSrc,0);\n
	ivec2 texKernelSize = textureSize(texKernel,0);\n
	{\n
		int kxsize = texKernelSize.x ;\n
		int kysize = texKernelSize.y ;\n
		int kxp = kxsize / 2 ;\n
		int kxm = -kxp;\n
		int kyp = kysize / 2 ;\n
		int kym = -kyp;\n
		vec4 sum = vec4(0.0,0.0,0.0,0.0);\n
		for (int ky = kym; ky <= kyp; ky++){\n
				for (int kx = kxm; kx <= kxp; kx++){\n
				vec4 data;\n
				vec4 coef;\n
				int x = int(gl_FragCoord.x) + kx;\n
				int y = int(gl_FragCoord.y) + ky;\n
				x = BOUND_PVT(x,0,texSize.x-1);\n
				y = BOUND_PVT(y,0,texSize.y-1);\n
				data = texelFetch(texSrc, ivec2(x, y), 0);\n
				coef = vec4(texelFetch(texKernel, ivec2(kx+kxp, ky+kyp), 0).r);\n
				sum += data * coef;\n
			}\n
		}\n
		dst = sum + delta; \n
	}\n
}\n
);
	return fragmentShaderCode;
}

list<string> glsShaderFilter2D::UniformNameList(void){
	list<string> lst;
	lst.push_back("texSrc");
	lst.push_back("texKernel");
	lst.push_back("delta");
	return lst;
}






static 
glsShaderBase* selectShader1D(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = &ShaderFilter1D; break;
	case(CV_8U) :
	case(CV_16U) : shader = &ShaderFilter1DU; break;
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
	case(CV_32F) : shader = &ShaderFilter2D; break;
		//case(CV_8U) :
		//case(CV_16U) : shader = shaderFilterU; break;
		//case(CV_8S) :
		//case(CV_16S) :
		//case(CV_32S) : shader = shaderFilterS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}



void sepFilter2D(const GlsMat& src, GlsMat& dst, int ddepth, const Mat& kernelX, const Mat& kernelY, Point anchor, double delta){
	GLS_Assert(src.depth() == CV_32F || src.depth() == CV_16U || src.depth() == CV_8U);
	GLS_Assert(kernelX.type() == CV_32FC1);
	GLS_Assert(kernelY.type() == CV_32FC1);
	GLS_Assert(kernelX.rows == 1 || kernelX.cols == 1);
	GLS_Assert(kernelY.rows == 1 || kernelY.cols == 1);
	GLS_Assert(anchor == Point(-1,-1));

	GlsMat _kernelX;
	GlsMat _kernelY;
	if (kernelX.rows == 1) _kernelX = (GlsMat)kernelX;
	else  _kernelX = (GlsMat)kernelX.t();

	if (kernelY.cols == 1) _kernelY = (GlsMat)kernelY;
	else _kernelY = (GlsMat)kernelY.t();


	GlsMat _dst = getDstMat(src,dst);

	GlsMat _tmp = GlsMat(src.size(), src.type());

	glsShaderBase* shader = selectShader1D(src.type());
#if 1
	shader->Execute(src, _kernelX, 0.0f,_tmp);	//row filter
	shader->Execute(_tmp, _kernelY, (float)delta,_dst);	//col filter
#else
	shader->Execute(src, _kernelY, _tmp);	//col filter
	shader->Execute(_tmp, _kernelX, _dst);	//row filter
#endif
	dst = _dst;
}

void filter2D(const GlsMat& src, GlsMat& dst, int ddepth, const Mat& kernel, Point anchor, double delta){
	GLS_Assert(src.depth() == CV_32F);
	GLS_Assert(kernel.type() == CV_32FC1);
	GLS_Assert(anchor == Point(-1, -1));

	GlsMat _kernel = (GlsMat)kernel;

	GlsMat _dst = getDstMat(src, dst);

	glsShaderBase* shader = selectShader2D(src.type());
	shader->Execute(src, _kernel, delta, _dst);
	dst = _dst;
}


void GaussianBlur(const GlsMat& src, GlsMat& dst, Size ksize, double sigmaX, double sigmaY){

	Mat kernelX = cv::getGaussianKernel(ksize.width, sigmaX, CV_32F);
	Mat kernelY = cv::getGaussianKernel(ksize.height, sigmaY, CV_32F);

	gls::sepFilter2D(src, dst, src.depth(), kernelX, kernelY);
}

void boxFilter(const GlsMat& src, GlsMat& dst, int ddepth, Size ksize){
	/* TODO
	integral filterによる実装
	*/

	Mat kernelX = Mat::ones(Size(ksize.width, 1), CV_32F) / (float)ksize.width;
	Mat kernelY = Mat::ones(Size(1, ksize.height), CV_32F) / (float)ksize.height;

	gls::sepFilter2D(src, dst, ddepth, kernelX, kernelY);
}

void Sobel(const GlsMat& src, GlsMat& dst, int ddepth, int xorder, int yorder, int ksize, double scale, double delta){

	const Point anchor = Point(-1, -1);

	Mat kernelX;
	Mat kernelY;

	cv::getDerivKernels(kernelX, kernelY, xorder, yorder, ksize, false, CV_32F);

	/* NOTE
	次数の低いkernelにscaleを掛け合わせる。(LPF側)
	(または、両方のkernelにsqrt(scale)を掛け合わせる。)
	*/
	if (xorder < yorder)kernelX *= scale;
	else kernelY *= scale;

	gls::sepFilter2D(src, dst, ddepth, kernelX, kernelY, anchor, delta);

}

void Laplacian(const GlsMat& src, GlsMat& dst, int ddepth, int ksize, double scale, double delta){

	GLS_Assert(ksize == 1);

	if (ksize == 1){
		Mat kernel;
		kernel = (cv::Mat_<float>(3, 3) << 0.0f, 1.0f, 0.0f, 1.0f, -4.0f, 1.0f, 0.0f, 1.0f, 0.0f);
		kernel *= scale;
//		cout << kernel << endl;
		const Point anchor(-1, -1);
		gls::filter2D(src, dst, ddepth, kernel, anchor, delta);

	}
	else{
		/* TODO
		cvと結果が一致しない。
		*/

		Mat kernelX;
		Mat kernelY;

		cv::getDerivKernels(kernelX, kernelY, 2, 2, ksize, false, CV_32F);
//		cout << kernelX << endl;
//		cout << kernelY << endl;
#if 0
		gls::sepFilter2D(src, dst, ddepth, kernelX, kernelY);
#elif 1
		Mat kernel = kernelY*(kernelX.t());
//		cout << kernel << endl;
		gls::filter2D(src, dst, ddepth, kernel);
#else
		GlsMat _kernelX = (GlsMat)kernelX.t();
		GlsMat _kernelY = (GlsMat)kernelY;

		GlsMat _tmpX = GlsMat(src.size(), src.type());
		GlsMat _tmpY = GlsMat(src.size(), src.type());

		glsShaderBase* shader = selectShader1D(src.type());
		shader->Execute(src, _kernelX, _tmpX);	//row filter
		shader->Execute(src, _kernelY, _tmpY);	//col filter

		gls::add(_tmpX, _tmpY,dst);


#endif
	}
}



}//namespace gls





