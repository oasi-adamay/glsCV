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

#include "glsCanny.h"
#include "glsFilter.h"
#include "glsConvert.h"
#include "glsCartToPolar.h"
#include "glsNonmaximaSuppression.h"

namespace gls
{


//map encording
enum {
	can_not_belong_to_an_edge = 0,
	might_belong_to_an_edge = 128,
	does_belong_to_an_edge = 255,
};


#if 1
//-----------------------------------------------------------------------------
// glsShaderMapFollow
//3値マップから2値化マップへ

class glsShaderMapFollowU : public glsShaderBase
{
protected:
	list<string> UniformNameList(void){
		list<string> lst;
		lst.push_back("texSrc");
		lst.push_back("last");
		return lst;
	}
	string FragmentShaderCode(void);

public:
	glsShaderMapFollowU(void) : glsShaderBase(__FUNCTION__){}
};



//-----------------------------------------------------------------------------
//global 
glsShaderMapFollowU ShaderMapFollowU;


//-----------------------------------------------------------------------------
//glsShaderMapFollow

//-----------------------------------------------------------------------------
//glsShaderMapFollowU
string glsShaderMapFollowU::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;\n
uniform usampler2D	texSrc;\n
uniform int last;\n
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
	dst = val;
}\n
);
	return fragmentShaderCode;
}


static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
//	case(CV_32F) : shader = &ShaderMapFollow; break;
	case(CV_8U) :
	case(CV_16U) : shader = &ShaderMapFollowU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = &ShaderMapFollowS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}

#endif

void Canny(const GlsMat& src, GlsMat& dst, double threshold1, double threshold2, int apertureSize, bool L2gradient)
{
	GLS_Assert(src.channels() == 1);
	GLS_Assert(src.depth() == CV_8UC1 || src.depth() == CV_32FC1);

	float lowThreshold = (float)MIN(threshold1, threshold2);
	float highThreshold = (float)MAX(threshold1, threshold2);

	GlsMat _src;
	if (src.depth() == CV_8UC1 || src.depth() == CV_16UC1){
		gls::convert(src,_src);
	}
	else{
		_src = src;
	}

	GlsMat dx;
	GlsMat dy;

	gls::Sobel(_src, dx, CV_32F, 1, 0, apertureSize, 1, 0 /*, cv::BORDER_REPLICATE*/);
	gls::Sobel(_src, dy, CV_32F, 0, 1, apertureSize, 1, 0 /*, cv::BORDER_REPLICATE*/);


	GlsMat mag;
	GlsMat angle;
	GlsMat map;

	gls::cartToPolar(dx, dy, mag, angle);
	gls::nonmaximaSuppression(mag, angle, map, highThreshold, lowThreshold);

#if 1
	glsShaderBase* shader = selectShader(map.type());

	GlsMat buf[2];
	int bank = 0;
	buf[bank] = map;
	buf[bank ^ 1] = GlsMat(map.size(), map.type());
	int loop_count = 256;
	while (loop_count--){
		shader->Execute(buf[bank], 0, buf[bank ^ 1]);
		bank = bank ^ 1;
	}
	shader->Execute(buf[bank], 1, buf[bank ^ 1]);
	bank = bank ^ 1;

	dst = buf[bank];
#else
	dst = map;
#endif
}





}//namespace gls





