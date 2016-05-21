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

#include "glsResize.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderResize
class glsShaderResizeBase : public glsShaderBase
{
protected:
	list<string> UniformNameList(void){
		list<string> lst;
		lst.push_back("texSrc");
		lst.push_back("fx");
		lst.push_back("fy");
		lst.push_back("flag");
		return lst;
	}
public:
	glsShaderResizeBase(const string& _name) :glsShaderBase(_name){}
};

//-----------------------------------------------------------------------------
// glsShaderResize
class glsShaderResize : public glsShaderResizeBase
{
protected:
	string FragmentShaderCode(void);
public:
	glsShaderResize(void) :glsShaderResizeBase(__FUNCTION__){}
};

//-----------------------------------------------------------------------------
//glsShaderResize
string glsShaderResize::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float; \n
uniform sampler2D	texSrc; \n
uniform float fx; \n
uniform float fy; \n
uniform int flag; \n
layout(location = 0) out vec4 dst; \n
vec4 cubic(float v){ \n
    vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v; \n
	vec4 s = n * n * n; \n
	float x = s.x; \n
	float y = s.y - 4.0 * s.x; \n
	float z = s.z - 4.0 * s.y + 6.0 * s.x; \n
	float w = 6.0 - x - y - z; \n
	return vec4(x, y, z, w) * (1.0 / 6.0); \n
} \n
vec4 textureBicubic(sampler2D sampler, vec2 texCoords){ \n
	vec2 texSize = textureSize(texSrc, 0); \n
    vec2 invTexSize = 1.0 / texSize; \n
    texCoords = texCoords * texSize - 0.5;  \n
	vec2 fxy = fract(texCoords);  \n
	texCoords -= fxy; \n
	vec4 xcubic = cubic(fxy.x);  \n
	vec4 ycubic = cubic(fxy.y); \n
	vec4 c = texCoords.xxyy + vec2(-0.5, +1.5).xyxy;  \n
	vec4 s = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);  \n
	vec4 offset = c + vec4(xcubic.yw, ycubic.yw) / s; \n
	offset *= invTexSize.xxyy; \n
	vec4 sample0 = texture(sampler, offset.xz); \n
	vec4 sample1 = texture(sampler, offset.yz); \n
	vec4 sample2 = texture(sampler, offset.xw); \n
	vec4 sample3 = texture(sampler, offset.yw); \n
	float sx = s.x / (s.x + s.y); \n
	float sy = s.z / (s.z + s.w); \n
	return mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy); \n
} \n
void main(void)\n
{ \n
	ivec2 texSize = textureSize(texSrc, 0); \n
	vec2 coord = vec2(gl_FragCoord.x / (float(texSize.x)*fx), gl_FragCoord.y / (float(texSize.y)*fy)); \n
	if (flag == 0){ \n
		dst = texture(texSrc, coord); \n
	}\n
	else{ \n
		dst = textureBicubic(texSrc, coord); \n
	} \n
}\n
);
	return fragmentShaderCode;
}


//-----------------------------------------------------------------------------
//global 
glsShaderResize ShaderResize;



static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = &ShaderResize; break;
	//case(CV_8U) :
	//case(CV_16U) : shader = &ShaderResizeU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = &ShaderResizeI; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}

void resize(const GlsMat& src, GlsMat& dst, Size dsize, double fx, double fy, int interpolation){
	GLS_Assert(src.depth()==CV_32F);
	GLS_Assert(interpolation == INTER_NEAREST || interpolation == INTER_LINEAR);
	GLS_Assert((fx == 0 && fy == 0 && dsize != Size(0, 0))
		|| (fx > 0 && fy > 0 && dsize == Size(0, 0)));

	if (fx == 0 && fy == 0){
		fx = dsize.width / (double)src.cols;
		fy = dsize.height / (double)src.rows;
	}
	else{
		dsize = Size((int)round(fx*src.cols), (int)round(fy*src.rows));
	}

	int flag;
		
	GlsMat _dst = getDstMat(dsize, src.type(), dst);
	glsShaderBase* shader = selectShader(src.type());
	switch (interpolation){
	case(INTER_NEAREST) : {
		src.setInterpolation(GL_NEAREST);
		flag = 0;
	}break;
	case(INTER_LINEAR) : {
		src.setInterpolation(GL_LINEAR);
		flag = 0;
	}break;
	case(INTER_CUBIC) : {
		src.setInterpolation(GL_LINEAR);
		flag = 1;
	}break;
	default: GLS_Assert(0);
	}

	shader->Execute(src, fx, fy, flag, _dst);
	dst = _dst;
}


}//namespace gls





