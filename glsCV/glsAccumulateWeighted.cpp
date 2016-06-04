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

#include "glsAccumulateWeighted.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderAccumulateWeighted
class glsShaderAccumulateWeightedBase : public glsShaderBase
{
protected:
	list<string> UniformNameList(void){
		list<string> lst;
		lst.push_back("texSrc");
		lst.push_back("texDst");
		lst.push_back("alpha");
		return lst;
	}
public:
	glsShaderAccumulateWeightedBase(const string& _name) :glsShaderBase(_name){}


};

//-----------------------------------------------------------------------------
// glsShaderAccumulateWeighted
class glsShaderAccumulateWeighted : public glsShaderAccumulateWeightedBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderAccumulateWeighted(void) : glsShaderAccumulateWeightedBase(__FUNCTION__){}

};

//-----------------------------------------------------------------------------
// glsShaderAccumulateWeighted
class glsShaderAccumulateWeightedU : public glsShaderAccumulateWeightedBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderAccumulateWeightedU(void) : glsShaderAccumulateWeightedBase(__FUNCTION__){}

};



//-----------------------------------------------------------------------------
//global 
glsShaderAccumulateWeighted ShaderAccumulateWeighted;
glsShaderAccumulateWeightedU ShaderAccumulateWeightedU;

//-----------------------------------------------------------------------------
//glsShaderAccumulateWeighted
string glsShaderAccumulateWeighted::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float; \n
uniform sampler2D	texSrc; \n
uniform sampler2D	texDst; \n
uniform float	alpha; \n
layout(location = 0) out vec4 dst; \n
void main(void)\n
{ \n
	vec4 src = texelFetch(texSrc, ivec2(gl_FragCoord.xy), 0); \n
	vec4 _dst = texelFetch(texDst, ivec2(gl_FragCoord.xy), 0); \n
	dst = vec4(1.0 - alpha)*_dst + vec4(alpha)*src; \n
}\n
);
	return fragmentShaderCode;
}

//-----------------------------------------------------------------------------
//glsShaderAccumulateWeightedU
string glsShaderAccumulateWeightedU::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float; \n
uniform usampler2D	texSrc; \n
uniform sampler2D	texDst; \n
uniform float	alpha; \n
layout(location = 0) out vec4 dst; \n
void main(void)\n
	{ \n
	vec4 src = vec4(texelFetch(texSrc, ivec2(gl_FragCoord.xy), 0)); \n
	vec4 _dst = texelFetch(texDst, ivec2(gl_FragCoord.xy), 0); \n
	dst = vec4(1.0 - alpha)*_dst + vec4(alpha)*src; \n
	}\n
);
	return fragmentShaderCode;
}



static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = &ShaderAccumulateWeighted; break;
	case(CV_8U) :
	case(CV_16U) : shader = &ShaderAccumulateWeightedU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = &ShaderAccumulateWeightedS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}

void accumulateWeighted(const GlsMat& src, GlsMat& dst, double alpha){
	GLS_Assert(src.depth() == CV_32F);
	GLS_Assert(dst.size() == src.size());

	GlsMat _dst = GlsMat(dst.size(), dst.type());
	glsShaderBase* shader = selectShader(src.type());
	shader->Execute(src, dst,(float)alpha, _dst);

	dst = _dst;
}






}//namespace gls





