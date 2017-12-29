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

#include "glsGemm.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderGemm
class glsShaderGemm : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);
	list<string> UniformNameList(void);


public:
	glsShaderGemm(void) :glsShaderBase(__FUNCTION__){}

};

//-----------------------------------------------------------------------------
//global 
glsShaderGemm ShaderGemm;



list<string> glsShaderGemm::UniformNameList(void){
	list<string> lst;
	lst.push_back("texSrc1");
	lst.push_back("texSrc2");
	lst.push_back("alpha");
	lst.push_back("texSrc3");
	lst.push_back("beta");
	return lst;
}


//-----------------------------------------------------------------------------
//glsShaderGemm
string glsShaderGemm::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;\n
uniform sampler2D	texSrc1;\n
uniform sampler2D	texSrc2; \n
uniform sampler2D	texSrc3; \n
uniform float alpha; \n
uniform float beta; \n
layout (location = 0) out float dst;\n
void main(void)\n
{\n
	ivec2 texSize = textureSize(texSrc1,0);\n
	float src1;\n
	float src2;\n
	float src3;\n
	float sum = float(0.0);\n
	for (int k = 0; k < texSize.x; k++){\n
		src1 = texelFetch(texSrc1, ivec2(k, gl_FragCoord.y), 0).r;\n
		src2 = texelFetch(texSrc2, ivec2(gl_FragCoord.x, k), 0).r;\n
		sum += src1*src2;\n
	}\n
	src3 = texelFetch(texSrc3, ivec2(gl_FragCoord.x, gl_FragCoord.y), 0).r; \n
	dst = alpha*sum + beta*src3;\n
}\n
);
	return fragmentShaderCode;
}


static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = &ShaderGemm; break;
	//case(CV_8U) :
	//case(CV_16U) : shader = &ShaderGemmU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = &ShaderGemmS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}


void gemm(const GlsMat& src1, const GlsMat& src2, float alpha, const GlsMat& src3, float beta, GlsMat& dst) {
	GLS_Assert(src1.depth() == CV_32F);
	GLS_Assert(src2.depth() == CV_32F);
	GLS_Assert(src1.cols == src2.rows);


	GlsMat _dst;
	_dst = getDstMat(Size(src2.cols, src1.rows), src1.type(), dst);


	glsShaderBase* shader = selectShader(src1.type());
	shader->Execute(src1,src2,alpha,src3,beta, _dst);
	dst = _dst;

}



}//namespace gls





