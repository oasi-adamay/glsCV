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

#include "glsThreshold.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderThreshold
class glsShaderThresholdBase : public glsShaderBase
{
protected:
	list<string> UniformNameList(void){
		list<string> lst;
		lst.push_back("texSrc");
		lst.push_back("thresh");
		lst.push_back("maxVal");
		lst.push_back("thresholdType");
		return lst;
	}
public:
	glsShaderThresholdBase(const string& _name) :glsShaderBase(_name){}


};

//-----------------------------------------------------------------------------
// glsShaderThreshold
class glsShaderThreshold : public glsShaderThresholdBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderThreshold(void) : glsShaderThresholdBase(__FUNCTION__){}

};

//-----------------------------------------------------------------------------
// glsShaderThresholdU
class glsShaderThresholdU : public glsShaderThresholdBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderThresholdU(void) : glsShaderThresholdBase(__FUNCTION__){}

};



//-----------------------------------------------------------------------------
//global 
glsShaderThreshold ShaderThreshold;
glsShaderThresholdU ShaderThresholdU;




/* Threshold types */
//enum
//{
//	CV_THRESH_BINARY = 0,  /* value = value > threshold ? max_value : 0       */
//	CV_THRESH_BINARY_INV = 1,  /* value = value > threshold ? 0 : max_value       */
//	CV_THRESH_TRUNC = 2,  /* value = value > threshold ? threshold : value   */
//	CV_THRESH_TOZERO = 3,  /* value = value > threshold ? value : 0           */
//	CV_THRESH_TOZERO_INV = 4,  /* value = value > threshold ? 0 : value           */
//	CV_THRESH_MASK = 7,
//	CV_THRESH_OTSU = 8  /* use Otsu algorithm to choose the optimal threshold value;
//						combine the flag with one of the above CV_THRESH_* values */
//};

//-----------------------------------------------------------------------------
//glsShaderThreshold
string glsShaderThreshold::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;\n
uniform sampler2D	texSrc;\n
uniform float	thresh;\n
uniform float	maxVal;\n
uniform int	thresholdType;\n
layout (location = 0) out float dst;\n
#define CV_THRESH_BINARY  0\n
#define CV_THRESH_BINARY_INV 1\n
#define CV_THRESH_TRUNC 2\n
#define CV_THRESH_TOZERO 3\n
#define CV_THRESH_TOZERO_INV 4\n
void main(void)\n
{\n
	float value = texelFetch(texSrc, ivec2(gl_FragCoord.xy), 0).r;\n
	switch(thresholdType){\n
   case(CV_THRESH_BINARY): dst = value > thresh? maxVal : 0.0; break;\n
   case(CV_THRESH_BINARY_INV): dst = value > thresh? 0.0 : maxVal; break;\n
   case(CV_THRESH_TRUNC): dst = value > thresh? thresh : value; break;\n
   case(CV_THRESH_TOZERO): dst = value > thresh? value : 0.0; break;\n
   case(CV_THRESH_TOZERO_INV): dst = value > thresh? 0.0 : value; break;\n
	}\n
}\n
);
	return fragmentShaderCode;
}

//-----------------------------------------------------------------------------
//glsShaderThresholdU
string glsShaderThresholdU::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;\n
uniform usampler2D	texSrc;\n
uniform float	thresh;\n
uniform float	maxVal;\n
uniform int	thresholdType;\n
layout (location = 0) out uint dst;\n
#define CV_THRESH_BINARY  0\n
#define CV_THRESH_BINARY_INV 1\n
#define CV_THRESH_TRUNC 2\n
#define CV_THRESH_TOZERO 3\n
#define CV_THRESH_TOZERO_INV 4\n
void main(void)\n
{\n
	uint value = texelFetch(texSrc, ivec2(gl_FragCoord.xy), 0).r;\n
	uint _thresh = uint(thresh);\n
	uint _maxVal = uint(maxVal);\n
	switch(thresholdType){\n
   case(CV_THRESH_BINARY): dst = value > _thresh? _maxVal : 0u ; break;\n
   case(CV_THRESH_BINARY_INV): dst = value > _thresh? 0u : _maxVal; break;\n
   case(CV_THRESH_TRUNC): dst = value > _thresh? _thresh : value; break;\n
   case(CV_THRESH_TOZERO): dst = value > _thresh? value : 0u; break;\n
   case(CV_THRESH_TOZERO_INV): dst = value > _thresh? 0u : value; break;\n
	}\n
}\n
);
	return fragmentShaderCode;
}


static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = &ShaderThreshold; break;
	case(CV_8U) :
	case(CV_16U) : shader = &ShaderThresholdU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = &ShaderThresholdS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}



void threshold(const GlsMat& src, GlsMat& dst, double thresh, double maxVal, int thresholdType){
	GLS_Assert(src.channels() == 1);
	GLS_Assert(src.depth() == CV_32F || src.depth() == CV_8U || src.depth() == CV_16U);

	GlsMat _dst = getDstMat(src.size(), CV_MAKE_TYPE(src.depth(),1), dst);

	glsShaderBase* shader = selectShader(src.type());
	shader->Execute(src, thresh, maxVal, thresholdType,_dst);

	dst = _dst;
}





}//namespace gls





