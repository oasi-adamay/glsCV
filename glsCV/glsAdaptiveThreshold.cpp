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
#include "glsAdaptiveThreshold.h"

#include "glsFilter.h"	//boxfilter


namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderAdaptiveThreshold
class glsShaderAdaptiveThresholdBase : public glsShaderBase
{
protected:
	list<string> UniformNameList(void){
		list<string> lst;
		lst.push_back("texSrc");
		lst.push_back("texTh");
		lst.push_back("c");
		lst.push_back("maxVal");
		lst.push_back("thresholdType");
		return lst;
	}

public:
	glsShaderAdaptiveThresholdBase(const string& _name) :glsShaderBase(_name){}

};


//-----------------------------------------------------------------------------
// glsShaderAdaptiveThreshold
class glsShaderAdaptiveThreshold : public glsShaderAdaptiveThresholdBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderAdaptiveThreshold(void) : glsShaderAdaptiveThresholdBase(__FUNCTION__){}

};

//-----------------------------------------------------------------------------
// glsShaderAdaptiveThresholdU
class glsShaderAdaptiveThresholdU : public glsShaderAdaptiveThresholdBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderAdaptiveThresholdU(void) : glsShaderAdaptiveThresholdBase(__FUNCTION__){}

};



//-----------------------------------------------------------------------------
//global 
glsShaderAdaptiveThreshold ShaderAdaptiveThreshold;
glsShaderAdaptiveThresholdU ShaderAdaptiveThresholdU;




/* Threshold types */
//enum
//{
//	CV_THRESH_BINARY = 0,  /* value = value > threshold ? max_value : 0       */
//	CV_THRESH_BINARY_INV = 1,  /* value = value > threshold ? 0 : max_value       */
//};

//-----------------------------------------------------------------------------
//glsShaderAdaptiveThreshold
string glsShaderAdaptiveThreshold::FragmentShaderCode(void){
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform sampler2D	texSrc;\n"
"uniform sampler2D	texTh;\n"
"uniform float	c;\n"
"uniform float	maxVal;\n"
"uniform int	thresholdType;\n"
"layout (location = 0) out float dst;\n"
"#define CV_THRESH_BINARY  0\n"
"#define CV_THRESH_BINARY_INV 1\n"
"#define CV_THRESH_TRUNC 2\n"
"#define CV_THRESH_TOZERO 3\n"
"#define CV_THRESH_TOZERO_INV 4\n"
"void main(void)\n"
"{\n"
"	float value = texelFetch(texSrc, ivec2(gl_FragCoord.xy), 0).r;\n"
"	float thresh = texelFetch(texTh, ivec2(gl_FragCoord.xy), 0).r - c;\n"
"	switch(thresholdType){\n"
"   case(CV_THRESH_BINARY): dst = value > thresh? maxVal : 0.0; break;\n"
"   case(CV_THRESH_BINARY_INV): dst = value > thresh? 0.0 : maxVal; break;\n"
"   case(CV_THRESH_TRUNC): dst = value > thresh? thresh : value; break;\n"
"   case(CV_THRESH_TOZERO): dst = value > thresh? value : 0.0; break;\n"
"   case(CV_THRESH_TOZERO_INV): dst = value > thresh? 0.0 : value; break;\n"
"	}\n"
"}\n"
;
	return fragmentShaderCode;
}

//-----------------------------------------------------------------------------
//glsShaderAdaptiveThresholdU
string glsShaderAdaptiveThresholdU::FragmentShaderCode(void){
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform usampler2D	texSrc;\n"
"uniform usampler2D	texTh;\n"
"uniform float	c;\n"
"uniform float	maxVal;\n"
"uniform int	thresholdType;\n"
"layout (location = 0) out uint dst;\n"
"#define CV_THRESH_BINARY  0\n"
"#define CV_THRESH_BINARY_INV 1\n"
"#define CV_THRESH_TRUNC 2\n"
"#define CV_THRESH_TOZERO 3\n"
"#define CV_THRESH_TOZERO_INV 4\n"
"void main(void)\n"
"{\n"
"	uint value = texelFetch(texSrc, ivec2(gl_FragCoord.xy), 0).r;\n"
"	uint _thresh = texelFetch(texTh, ivec2(gl_FragCoord.xy), 0).r - uint(c);\n"
"	uint _maxVal = uint(maxVal);\n"
"	switch(thresholdType){\n"
"   case(CV_THRESH_BINARY): dst = value > _thresh? _maxVal : 0u ; break;\n"
"   case(CV_THRESH_BINARY_INV): dst = value > _thresh? 0u : _maxVal; break;\n"
"   case(CV_THRESH_TRUNC): dst = value > _thresh? _thresh : value; break;\n"
"   case(CV_THRESH_TOZERO): dst = value > _thresh? value : 0u; break;\n"
"   case(CV_THRESH_TOZERO_INV): dst = value > _thresh? 0u : value; break;\n"
"	}\n"
"}\n"
;
	return fragmentShaderCode;
}


static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = &ShaderAdaptiveThreshold; break;
	case(CV_8U) :
	case(CV_16U) : shader = &ShaderAdaptiveThresholdU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = &ShaderAdaptiveThresholdS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}


void adaptiveThreshold(const GlsMat& src, GlsMat& dst, double maxVal, int adaptiveMethod, int thresholdType, int blockSize, double C)
{
	GLS_Assert(src.channels() == 1);
	GLS_Assert(src.depth() == CV_32F || src.depth() == CV_8U || src.depth() == CV_16U);

	GlsMat _dst = getDstMat(src.size(), CV_MAKE_TYPE(src.depth(),1), dst);
	GlsMat _lpf;

	if (adaptiveMethod == ADAPTIVE_THRESH_MEAN_C){
		gls::boxFilter(src, _lpf, src.depth(),Size(blockSize, blockSize));

	}
	else if (adaptiveMethod == ADAPTIVE_THRESH_GAUSSIAN_C){
		gls::GaussianBlur(src, _lpf, Size(blockSize, blockSize), 0, 0);
	}


	glsShaderBase* shader = selectShader(src.type());
	shader->Execute(src, _lpf, C, maxVal, thresholdType, _dst);
	dst = _dst;
}





}//namespace gls





