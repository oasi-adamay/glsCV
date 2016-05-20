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

#include "glsReduce.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderReduce
class glsShaderReduce : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);
	list<string> UniformNameList(void);


public:
	glsShaderReduce(void) :glsShaderBase(__FUNCTION__){}

};

//-----------------------------------------------------------------------------
//global 
glsShaderReduce ShaderReduce;


//#define CV_REDUCE_SUM 0
//#define CV_REDUCE_AVG 1
//#define CV_REDUCE_MAX 2
//#define CV_REDUCE_MIN 3

list<string> glsShaderReduce::UniformNameList(void){
	list<string> lst;
	lst.push_back("texSrc");
	lst.push_back("dim");
	lst.push_back("reduceOp");
	return lst;
}


//-----------------------------------------------------------------------------
//glsShaderReduce
string glsShaderReduce::FragmentShaderCode(void){
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform sampler2D	texSrc;\n"
"uniform int dim;\n"
"uniform int reduceOp;\n"
"layout (location = 0) out vec4 dst;\n"
"#define FLT_MAX  3.402823e+38\n"
"void main(void)\n"
"{\n"
"	ivec2 texSize = textureSize(texSrc,0);\n"
"	vec4 data;\n"
"	if (dim == 1){\n"
"		switch (reduceOp){\n"
"		case(0) : {  //CV_REDUCE_SUM\n"
"			vec4 sum = vec4(0.0);\n"
"			for (int x = 0; x < texSize.x; x++){\n"
"				data = texelFetch(texSrc, ivec2(x, gl_FragCoord.y), 0);\n"
"				sum = sum + data;\n"
"			}\n"
"			dst = sum;\n"
"		}break;\n"
"		case(1) : { //CV_REDUCE_AVE\n"
"			vec4 sum = vec4(0.0);\n"
"			for (int x = 0; x < texSize.x; x++){\n"
"				data = texelFetch(texSrc, ivec2(x, gl_FragCoord.y), 0);\n"
"				sum = sum + data;\n"
"			}\n"
"			dst = sum / vec4(texSize.x);\n"
"		}break;\n"
"		case(2) : { //CV_REDUCE_MAX\n"
"			vec4 maxv = vec4(-FLT_MAX);\n"
"			for (int x = 0; x < texSize.x; x++){\n"
"				data = texelFetch(texSrc, ivec2(x, gl_FragCoord.y), 0);\n"
"				maxv = max(data,maxv);\n"
"			}\n"
"			dst = maxv;\n"
"		}break;\n"
"		case(3) : { //CV_REDUCE_MIN\n"
"			vec4 minv = vec4(FLT_MAX);\n"
"			for (int x = 0; x < texSize.x; x++){\n"
"				data = texelFetch(texSrc, ivec2(x, gl_FragCoord.y), 0);\n"
"				minv = min(data,minv);\n"
"			}\n"
"			dst = minv;\n"
"		}break;\n"
"		}\n"
"	}\n"
"	else {	//dim==0\n"
"		switch (reduceOp){\n"
"		case(0) : {  //CV_REDUCE_SUM\n"
"			vec4 sum = vec4(0.0);\n"
"			for (int y = 0; y < texSize.y; y++){\n"
"				data = texelFetch(texSrc, ivec2(gl_FragCoord.x, y), 0);\n"
"				sum = sum + data;\n"
"			}\n"
"			dst = sum;\n"
"		}break;\n"
"		case(1) : { //CV_REDUCE_AVE\n"
"			vec4 sum = vec4(0.0);\n"
"			for (int y = 0; y < texSize.y; y++){\n"
"				data = texelFetch(texSrc, ivec2(gl_FragCoord.x, y), 0);\n"
"				sum = sum + data;\n"
"			}\n"
"			dst = sum / vec4(texSize.y);\n"
"		}break;\n"
"		case(2) : { //CV_REDUCE_MAX\n"
"			vec4 maxv = vec4(-FLT_MAX);\n"
"			for (int y = 0; y < texSize.y; y++){\n"
"				data = texelFetch(texSrc, ivec2(gl_FragCoord.x, y), 0);\n"
"				maxv = max(data,maxv);\n"
"			}\n"
"			dst = maxv;\n"
"		}break;\n"
"		case(3) : { //CV_REDUCE_MIN\n"
"			vec4 minv = vec4(FLT_MAX);\n"
"			for (int y = 0; y < texSize.y; y++){\n"
"				data = texelFetch(texSrc, ivec2(gl_FragCoord.x, y), 0);\n"
"				minv = min(data,minv);\n"
"			}\n"
"			dst = minv;\n"
"		}break;\n"
"		}\n"
"	}\n"
"}\n"
;
	return fragmentShaderCode;
}


static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = &ShaderReduce; break;
	//case(CV_8U) :
	//case(CV_16U) : shader = &ShaderReduceU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = &ShaderReduceS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}


void reduce(const GlsMat& src, GlsMat& dst, int dim, int reduceOp){
	GLS_Assert(src.depth() == CV_32F);


	GlsMat _dst;
	if (dim == 0){
		_dst = getDstMat(Size(src.cols, 1), src.type(),dst);
	}
	else{
		_dst = getDstMat(Size(1, src.rows), src.type(),dst);
	}


	glsShaderBase* shader = selectShader(src.type());
	shader->Execute(src, dim, reduceOp, _dst);
	dst = _dst;
}


}//namespace gls





