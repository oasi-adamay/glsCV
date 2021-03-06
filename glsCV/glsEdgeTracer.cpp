﻿/*
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
#include "glsAtomicCounter.h"


#include "glsEdgeTracer.h"

namespace gls
{


//map encording
enum {
	can_not_belong_to_an_edge = 0,
	might_belong_to_an_edge = 128,
	does_belong_to_an_edge = 255,
};

//-----------------------------------------------------------------------------
// glsShaderEdgeTracer
//3値マップから2値化マップへ

class glsShaderEdgeTracerU : public glsShaderBase
{
protected:
	list<string> UniformNameList(void){
		list<string> lst;
		lst.push_back("texSrc");
		lst.push_back("last");
		lst.push_back("ac");
		return lst;
	}
	string FragmentShaderCode(void);

public:
	glsShaderEdgeTracerU(void) : glsShaderBase(__FUNCTION__){}
};



//-----------------------------------------------------------------------------
//global 
glsShaderEdgeTracerU ShaderEdgeTracerU;


//-----------------------------------------------------------------------------
//glsShaderEdgeTracer

//-----------------------------------------------------------------------------
//glsShaderEdgeTracerU
string glsShaderEdgeTracerU::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 420 compatibility \n
precision highp float;\n
uniform usampler2D	texSrc;\n
uniform int last;\n
layout(binding = 0, offset = 0) uniform atomic_uint ac;
layout (location = 0) out uint dst;\n
void main(void)\n
{\n
	ivec2 xy = ivec2(gl_FragCoord.xy); \n
	uint src = texelFetch(texSrc, xy, 0).r;\n
	uint val = src;\n
	if (src == 128u){\
		val |= texelFetchOffset(texSrc, xy, 0, ivec2(-1, -1)).r; \n
		val |= texelFetchOffset(texSrc, xy, 0, ivec2(+0, -1)).r; \n
		val |= texelFetchOffset(texSrc, xy, 0, ivec2(+1, -1)).r; \n
		val |= texelFetchOffset(texSrc, xy, 0, ivec2(-1, +0)).r; \n
		val |= texelFetchOffset(texSrc, xy, 0, ivec2(+1, +0)).r; \n
		val |= texelFetchOffset(texSrc, xy, 0, ivec2(-1, +1)).r; \n
		val |= texelFetchOffset(texSrc, xy, 0, ivec2(+0, +1)).r; \n
		val |= texelFetchOffset(texSrc, xy, 0, ivec2(+1, +1)).r; \n
	}\n
	if (last != 0 && src == 128u) val =0u;\n
	if (val != src) atomicCounterIncrement(ac); \n
	dst = val;
}\n
);
	return fragmentShaderCode;
}


static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
//	case(CV_32F) : shader = &ShaderEdgeTracer; break;
	case(CV_8U) :
	case(CV_16U) : shader = &ShaderEdgeTracerU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = &ShaderEdgeTracerS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}


void edgeTracer(const GlsMat& src, GlsMat& dst)
{
	GLS_Assert(src.channels() == 1);
	GLS_Assert(src.depth() == CV_8UC1);

	glsShaderBase* shader = selectShader(src.type());

	GlsMat buf[2];
	int bank = 0;
	buf[bank] = src;
	buf[bank ^ 1] = GlsMat(src.size(), src.type());

	AtomicCounter ac;
	const int loop_count_max = 256;
	int loop_count = 0;
	while (loop_count++< loop_count_max){
		ac.Set(0);
		shader->Execute(buf[bank], 0, ac, buf[bank ^ 1]);
		bank = bank ^ 1;
		unsigned int atomic_count = ac.Get();
//		cout << "atomic_count:" << atomic_count << endl;
		if (atomic_count == 0) break;
	}
	shader->Execute(buf[bank], 1, ac, buf[bank ^ 1]);
	bank = bank ^ 1;

	dst = buf[bank];

//	cout << "loop_count:" << loop_count << endl;
}





}//namespace gls





