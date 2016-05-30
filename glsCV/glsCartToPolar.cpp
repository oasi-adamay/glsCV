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

#include "glsCartToPolar.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderCartToPolar
class glsShaderCartToPolarBase : public glsShaderBase
{
protected:
	list<string> UniformNameList(void){
		list<string> lst;
		lst.push_back("texSrcX");
		lst.push_back("texSrcY");
		lst.push_back("angleInDegrees");
		return lst;
	}
public:
	glsShaderCartToPolarBase(const string& _name) :glsShaderBase(_name){}


};

//-----------------------------------------------------------------------------
// glsShaderCartToPolar
class glsShaderCartToPolar : public glsShaderCartToPolarBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderCartToPolar(void) : glsShaderCartToPolarBase(__FUNCTION__){}

};



//-----------------------------------------------------------------------------
//global 
glsShaderCartToPolar ShaderCartToPolar;





//-----------------------------------------------------------------------------
//glsShaderCartToPolar
string glsShaderCartToPolar::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float; \n
uniform sampler2D	texSrcX; \n
uniform sampler2D	texSrcY; \n
uniform int	angleInDegrees; \n
layout(location = 0) out float magnitude; \n
layout(location = 1) out float angle; \n
#define M_PI 3.1415926535897932384626433832795 \n
void main(void)\n
{ \n
	float x = texelFetch(texSrcX, ivec2(gl_FragCoord.xy), 0).r; \n
	float y = texelFetch(texSrcY, ivec2(gl_FragCoord.xy), 0).r; \n
	magnitude = sqrt(x*x + y*y); \n
	if (x == 0.0) angle = 0.0;\n
	else angle = atan(y, x);\n
	if (angle<0)angle = angle + 2.0 * M_PI;\n
	if (angleInDegrees!=0) angle = 360.0 * angle / (2.0*M_PI);\n
}\n
);
	return fragmentShaderCode;
}



static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = &ShaderCartToPolar; break;
	//case(CV_8U) :
	//case(CV_16U) : shader = &ShaderCartToPolarU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = &ShaderCartToPolarS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}


void cartToPolar(const GlsMat& x, const GlsMat& y, GlsMat& magnitude, GlsMat& angle, bool angleInDegrees){
	GLS_Assert(x.type() == CV_32FC1);
	GLS_Assert(y.type() == CV_32FC1);

	magnitude = GlsMat(x.size(), x.type());
	angle = GlsMat(x.size(), x.type());

	glsShaderBase* shader = selectShader(x.type());
	shader->Execute(x, y, (int)angleInDegrees, magnitude, angle);
}





}//namespace gls





