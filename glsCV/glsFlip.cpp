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

#include "glsFlip.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderFlip
class glsShaderFlipBase : public glsShaderBase
{
protected:
	list<string> UniformNameList(void){
		list<string> lst;
		lst.push_back("texSrc");
		lst.push_back("flipCode");
		return lst;
	}
public:
	glsShaderFlipBase(const string& _name) :glsShaderBase(_name){}
};

//-----------------------------------------------------------------------------
// glsShaderFlip
class glsShaderFlip : public glsShaderFlipBase
{
protected:
	string FragmentShaderCode(void);
public:
	glsShaderFlip(void) :glsShaderFlipBase(__FUNCTION__){}
};
//-----------------------------------------------------------------------------
// glsShaderFlipU
class glsShaderFlipU : public glsShaderFlipBase
{
protected:
	string FragmentShaderCode(void);
public:
	glsShaderFlipU(void) :glsShaderFlipBase(__FUNCTION__){}
};
//-----------------------------------------------------------------------------
// glsShaderFlipI
class glsShaderFlipI : public glsShaderFlipBase
{
protected:
	string FragmentShaderCode(void);
public:
	glsShaderFlipI(void) :glsShaderFlipBase(__FUNCTION__){}
};


//-----------------------------------------------------------------------------
//glsShaderFlip
string glsShaderFlip::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float; \n
uniform sampler2D	texSrc; \n
uniform int flipCode; \n
layout(location = 0) out vec4 dst; \n
void main(void)\n
{ \n
	ivec2 texSize = textureSize(texSrc, 0); \n
	ivec2 coord = ivec2(gl_FragCoord.xy); \n
	if (flipCode == 0){\n
		coord.y = texSize.y - coord.y - 1; \n
	}\n
	else if (flipCode > 0){\n
		coord.x = texSize.x - coord.x - 1; \n
	}\n
	else {\n
		coord.y = texSize.y - coord.y - 1;\n
		coord.x = texSize.x - coord.x - 1; \n
	}\n
	dst = texelFetch(texSrc, coord, 0); \n
}\n
);
	return fragmentShaderCode;
}

//-----------------------------------------------------------------------------
//glsShaderFlip
string glsShaderFlipU::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float; \n
uniform usampler2D	texSrc; \n
uniform int flipCode; \n
layout(location = 0) out uvec4 dst; \n
void main(void)\n
{ \n
	ivec2 texSize = textureSize(texSrc, 0); \n
	ivec2 coord = ivec2(gl_FragCoord.xy); \n
	if (flipCode == 0){\n
		coord.y = texSize.y - coord.y - 1; \n
	}\n
	else if (flipCode > 0){\n
		coord.x = texSize.x - coord.x - 1; \n
	}\n
	else {\n
		coord.y = texSize.y - coord.y - 1;\n
		coord.x = texSize.x - coord.x - 1; \n
	}\n
	dst = texelFetch(texSrc, coord, 0); \n
}\n
);
	return fragmentShaderCode;
}

//-----------------------------------------------------------------------------
//glsShaderFlip
string glsShaderFlipI::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float; \n
uniform isampler2D	texSrc; \n
uniform int flipCode; \n
layout(location = 0) out ivec4 dst; \n
void main(void)\n
{ \n
	ivec2 texSize = textureSize(texSrc, 0); \n
	ivec2 coord = ivec2(gl_FragCoord.xy); \n
	if (flipCode == 0){\n
		coord.y = texSize.y - coord.y - 1; \n
	}\n
	else if (flipCode > 0){\n
		coord.x = texSize.x - coord.x - 1; \n
	}\n
	else {\n
		coord.y = texSize.y - coord.y - 1;\n
		coord.x = texSize.x - coord.x - 1; \n
	}\n
	dst = texelFetch(texSrc, coord, 0); \n
}\n
);
	return fragmentShaderCode;
}


//-----------------------------------------------------------------------------
//global 
glsShaderFlip ShaderFlip;
glsShaderFlipU ShaderFlipU;
glsShaderFlipI ShaderFlipI;





static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = &ShaderFlip; break;
	case(CV_8U) :
	case(CV_16U) : shader = &ShaderFlipU; break;
	case(CV_8S) :
	case(CV_16S) :
	case(CV_32S) : shader = &ShaderFlipI; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}


void flip(const GlsMat& src, GlsMat& dst, int flipCode){
	GlsMat _dst = getDstMat(src, dst);
	glsShaderBase* shader = selectShader(src.type());
	shader->Execute(src, flipCode, _dst);
	dst = _dst;
}


}//namespace gls





