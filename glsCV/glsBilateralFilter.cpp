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

/*----------------------------------------ffine-------------------------------------
include
*/
#include "glsMacro.h"
#include "GlsMat.h"
#include "glsShader.h"

#include "glsBilateralFilter.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderBilateralFilterBase
class glsShaderBilateralFilterBase : public glsShaderBase
{
protected:
	list<string> UniformNameList(void);

public:
	glsShaderBilateralFilterBase(const string& _name) :glsShaderBase(_name){}
};

list<string> glsShaderBilateralFilterBase::UniformNameList(void){
	list<string> lst;
	lst.push_back("texSrc");
	lst.push_back("texKernel");
	lst.push_back("color_coeff");
	return lst;
}


//-----------------------------------------------------------------------------
// glsShaderBilateralFilter
class glsShaderBilateralFilter : public glsShaderBilateralFilterBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderBilateralFilter(void) : glsShaderBilateralFilterBase(__FUNCTION__){}
};

//-----------------------------------------------------------------------------
//glsShaderBilateralFilter
string glsShaderBilateralFilter::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;\n
uniform sampler2D	texSrc;\n
uniform sampler2D	texKernel;\n
uniform float color_coeff; \n
layout (location = 0) out float dst;\n
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
		float data0 = texelFetch(texSrc, ivec2(gl_FragCoord.xy), 0).r; \n
		float sum = 0.0; \n
		float sumw = 0.0; \n
		for (int ky = kym; ky <= kyp; ky++){\n
			for (int kx = kxm; kx <= kxp; kx++){\n
				float data;\n
				float coef; \n
				int x = int(gl_FragCoord.x) + kx;\n
				int y = int(gl_FragCoord.y) + ky;\n
				x = BOUND_PVT(x,0,texSize.x-1);\n
				y = BOUND_PVT(y,0,texSize.y-1);\n
				data = texelFetch(texSrc, ivec2(x, y), 0).r;\n
				coef = texelFetch(texKernel, ivec2(kx+kxp, ky+kyp), 0).r;\n
				float diff2 = (data - data0)*(data - data0); \n
				float w = coef * exp(diff2*color_coeff); \n
				sum += data * w; \n
				sumw += w; \n
			}\n
		}\n
		dst = sum / sumw; \n
	}\n
}\n
);
	return fragmentShaderCode;
}

//-----------------------------------------------------------------------------
// glsShaderBilateralFilter
class glsShaderBilateralFilter32FC3 : public glsShaderBilateralFilterBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderBilateralFilter32FC3(void) : glsShaderBilateralFilterBase(__FUNCTION__){}
};

//-----------------------------------------------------------------------------
//glsShaderBilateralFilter32FC3
string glsShaderBilateralFilter32FC3::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;\n
uniform sampler2D	texSrc;\n
uniform sampler2D	texKernel;\n
uniform float color_coeff; \n
layout (location = 0) out vec3 dst;\n
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
		vec3 data0 = texelFetch(texSrc, ivec2(gl_FragCoord.xy), 0).rgb; \n
		vec3 sum = vec3(0.0,0.0,0.0); \n
		float sumw = 0.0; \n
		for (int ky = kym; ky <= kyp; ky++){\n
			for (int kx = kxm; kx <= kxp; kx++){\n
				vec3 data; \n
				float coef; \n
				int x = int(gl_FragCoord.x) + kx;\n
				int y = int(gl_FragCoord.y) + ky;\n
				x = BOUND_PVT(x,0,texSize.x-1);\n
				y = BOUND_PVT(y,0,texSize.y-1);\n
				data = texelFetch(texSrc, ivec2(x, y), 0).rgb;\n
				coef = texelFetch(texKernel, ivec2(kx+kxp, ky+kyp), 0).r;\n
//				vec3 diff2 = (data - data0)*(data - data0); \n
//				float ssd = diff2.r + diff2.g + diff2.b; \n
//				float w = coef * exp(ssd*color_coeff); \n
				vec3  absdif = abs(data - data0); \n
				float sad = absdif.r + absdif.g + absdif.b; \n
				float w = coef * exp(sad*sad*color_coeff); \n
				sum += data * vec3(w); \n
				sumw += w; \n
			}\n
		}\n
		dst = sum / vec3(sumw); \n
	}\n
}\n
);
	return fragmentShaderCode;
}



//-----------------------------------------------------------------------------
//global 
glsShaderBilateralFilter ShaderBilateralFilter;
glsShaderBilateralFilter32FC3 ShaderBilateralFilter32FC3;


static
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (type){
	case(CV_32FC1) : shader = &ShaderBilateralFilter; break;
	case(CV_32FC3) : shader = &ShaderBilateralFilter32FC3; break;
		//case(CV_8U) :
		//case(CV_16U) : shader = shaderFilterU; break;
		//case(CV_8S) :
		//case(CV_16S) :
		//case(CV_32S) : shader = shaderFilterS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}

void bilateralFilter(const GlsMat& src, GlsMat& dst, int d, double sigmaColor, double sigmaSpace){
	GLS_Assert(src.depth() == CV_32FC1 || src.depth() == CV_32FC3);

	if (sigmaColor <= 0)	sigmaColor = 1;
	if (sigmaSpace <= 0)	sigmaSpace = 1;

	double gauss_color_coeff = -0.5 / (sigmaColor*sigmaColor);
	double gauss_space_coeff = -0.5 / (sigmaSpace*sigmaSpace);

	int radius;
	if (d <= 0)		radius = cvRound(sigmaSpace*1.5);
	else	radius = d / 2;
	radius = MAX(radius, 1);
	d = radius * 2 + 1;
#if 0
	Mat kernel = cv::getGaussianKernel(d, sigmaSpace, CV_32F);
	kernel = kernel*(kernel.t());
#else
	Mat kernel = Mat::zeros(d, d, CV_32F);
	for (int i = -radius, maxk = 0; i <= radius; i++){
		for (int j = -radius; j <= radius; j++)	{
			double r = std::sqrt((double)i*i + (double)j*j);
			if (r > radius)	continue;

			kernel.at<float>(i + radius, j + radius)
			  = (float)std::exp(r*r*gauss_space_coeff);
		}
	}
#endif
//	cout << kernel << endl;

	GlsMat _dst = getDstMat(src, dst);
	glsShaderBase* shader = selectShader(src.type());
	GlsMat _kernel = (GlsMat)kernel;
	shader->Execute(src, _kernel, gauss_color_coeff, _dst);
	dst = _dst;

}



}//namespace gls





