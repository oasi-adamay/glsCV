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

#include "glsWarpAffine.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderWarpAffine
class glsShaderWarpAffineBase : public glsShaderBase
{
protected:
	list<string> UniformNameList(void){
		list<string> lst;
		lst.push_back("texSrc");
		lst.push_back("M");
		lst.push_back("flag");
		return lst;
	}
public:
	glsShaderWarpAffineBase(const string& _name) :glsShaderBase(_name){}
};

//-----------------------------------------------------------------------------
// glsShaderWarpAffine
class glsShaderWarpAffine : public glsShaderWarpAffineBase
{
protected:
	string FragmentShaderCode(void);
public:
	glsShaderWarpAffine(void) :glsShaderWarpAffineBase(__FUNCTION__){}
};

//-----------------------------------------------------------------------------
//glsShaderWarpAffine
string glsShaderWarpAffine::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float; \n
uniform sampler2D	texSrc; \n
uniform mat3x2 M; \n
uniform int flag; \n
layout(location = 0) out vec4 dst; \n
void main(void)\n
{ \n
	ivec2 texSize = textureSize(texSrc, 0); \n
	vec2 coord = gl_FragCoord.xy; \n
//	vec2 coord = gl_FragCoord.xy + vec2(+0.25,0); \n
	coord = vec2(coord.x*M[0][0] + coord.y*M[1][0] + M[2][0], \n
	             coord.x*M[0][1] + coord.y*M[1][1] + M[2][1]); \n
    coord = vec2(coord.x / float(texSize.x), coord.y / float(texSize.y)); \n
	dst = texture(texSrc, coord); \n
}\n
);
	return fragmentShaderCode;
}


//-----------------------------------------------------------------------------
//global 
glsShaderWarpAffine ShaderWarpAffine;



static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = &ShaderWarpAffine; break;
	//case(CV_8U) :
	//case(CV_16U) : shader = &ShaderWarpAffineU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = &ShaderWarpAffineI; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}


void warpAffine(const GlsMat& src, GlsMat& dst, const Mat& M, Size dsize, int flags){
	GLS_Assert(src.depth() == CV_32F);
	int interpolation = flags & INTER_MAX;
	bool inverseMap = (flags & WARP_INVERSE_MAP);
	GLS_Assert(interpolation == INTER_NEAREST || interpolation == INTER_LINEAR);
	GLS_Assert(M.rows == 2 && M.cols == 3);
	GlsMat _dst = getDstMat(dsize, src.type(), dst);
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

	Mat m;
	M.convertTo(m, CV_32F);			//32fに変更
	if (!inverseMap) cv::invertAffineTransform(m,m);
	/*! NOTE
	shader のAffine行列は、inverse format
	*/
	Matx23f m23((float*)m.ptr());
	glsShaderBase* shader = selectShader(src.type());	shader->Execute(src, m23, _flag, _dst);
	dst = _dst;


}



}//namespace gls





