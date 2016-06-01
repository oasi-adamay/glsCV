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

#include "glsRemap.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderRemap
class glsShaderRemapBase : public glsShaderBase
{
protected:
	list<string> UniformNameList(void){
		list<string> lst;
		lst.push_back("texSrc");
		lst.push_back("texMap");
		lst.push_back("flag");
		return lst;
	}
public:
	glsShaderRemapBase(const string& _name) :glsShaderBase(_name){}
};

//-----------------------------------------------------------------------------
// glsShaderRemap
class glsShaderRemap : public glsShaderRemapBase
{
protected:
	string FragmentShaderCode(void);
public:
	glsShaderRemap(void) :glsShaderRemapBase(__FUNCTION__){}
};

//-----------------------------------------------------------------------------
//glsShaderRemap
string glsShaderRemap::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float; \n
uniform sampler2D	texSrc; \n
uniform sampler2D	texMap; \n
uniform int flag; \n
layout(location = 0) out vec4 dst; \n
void main(void)\n
{ \n
	ivec2 texSize = textureSize(texSrc, 0); \n
	vec2 coord = texelFetch(texMap, ivec2(gl_FragCoord.xy), 0).rg; \n
    coord = vec2((coord.x+0.5) / float(texSize.x), (coord.y+0.5) / float(texSize.y)); \n
	dst = texture(texSrc, coord); \n
}\n
);
	return fragmentShaderCode;
}


//-----------------------------------------------------------------------------
//global 
glsShaderRemap ShaderRemap;



static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = &ShaderRemap; break;
	//case(CV_8U) :
	//case(CV_16U) : shader = &ShaderRemapU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = &ShaderRemapI; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}


void remap(const GlsMat& src, GlsMat& dst, const GlsMat& map1, const GlsMat& map2, int interpolation){
	GLS_Assert(src.depth() == CV_32F);
	GLS_Assert(interpolation == INTER_NEAREST || interpolation == INTER_LINEAR);
	GlsMat _dst = GlsMat(map1.size(), src.type());
	int _flag;	// internal flag 
	switch (interpolation){
	case(INTER_NEAREST) : {
		src.setInterpolation(GL_NEAREST);
		_flag = 0;
	}break;
	case(INTER_LINEAR) : {
		src.setInterpolation(GL_LINEAR);
		_flag = 0;
	}break;
	case(INTER_CUBIC) : {
		src.setInterpolation(GL_LINEAR);
		_flag = 1;
	}break;
	default: GLS_Assert(0);
	}

	src.setWrapMode(GL_CLAMP_TO_BORDER);

	glsShaderBase* shader = selectShader(src.type());
	shader->Execute(src, map1, _flag, _dst);
	dst = _dst;


}



}//namespace gls





