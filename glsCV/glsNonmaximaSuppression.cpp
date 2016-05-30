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

#include "glsNonmaximaSuppression.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderNonmaximaSuppression
class glsShaderNonmaximaSuppressionBase : public glsShaderBase
{
protected:
	list<string> UniformNameList(void){
		list<string> lst;
		lst.push_back("texMag");
		lst.push_back("texAngle");
		lst.push_back("threshold");
		return lst;
	}
public:
	glsShaderNonmaximaSuppressionBase(const string& _name) :glsShaderBase(_name){}


};

//-----------------------------------------------------------------------------
// glsShaderNonmaximaSuppression
class glsShaderNonmaximaSuppression : public glsShaderNonmaximaSuppressionBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderNonmaximaSuppression(void) : glsShaderNonmaximaSuppressionBase(__FUNCTION__){}

};



//-----------------------------------------------------------------------------
//global 
glsShaderNonmaximaSuppression ShaderNonmaximaSuppression;





//-----------------------------------------------------------------------------
//glsShaderNonmaximaSuppression
string glsShaderNonmaximaSuppression::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;			\n
uniform sampler2D	texMag;		\n
uniform sampler2D	texAngle;	\n
uniform float	threshold;		\n
layout(location = 0) out float dst;	\n
#define M_PI 3.1415926535897932384626433832795 \n
void main(void)\n
{ \n
	float rad_pi_8 = M_PI / 8.0; \n
	float rad_1pi_4 = M_PI / 4.0; \n
	float rad_2pi_4 = M_PI * 2.0 / 4.0; \n
	float rad_3pi_4 = M_PI * 3.0 / 4.0; \n
	float rad = texelFetch(texAngle, ivec2(gl_FragCoord.xy), 0).r; \n
	float a = texelFetch(texMag, ivec2(gl_FragCoord.xy), 0).r; \n
	float b = 0.0;\n
	float c = 0.0;\n
	ivec2 xy = ivec2(gl_FragCoord.xy);\n
	int x = xy.x;\n
	int y = xy.y;\n
	int y1 = 0; \n
	int y2 = 0; \n
	int x1 = 0; \n
	int x2 = 0; \n
	ivec2 texSize = textureSize(texMag, 0); \n
	int width = texSize.x; \n
	int height = texSize.x; \n
	if (a <= threshold) {
		\n
	}\n
	else{\n
		rad += rad_pi_8;\n
\n
		if (rad > M_PI) rad -= M_PI;\n
		if (rad > M_PI) rad -= M_PI;\n
\n
		if (0 < rad && rad < rad_1pi_4){		// E-W\n
			y1 = y2 = y; x1 = x + 1; x2 = x - 1; \n
		}\n
		else if (rad_1pi_4 <= rad && rad <= rad_2pi_4){	// NE-SW\n
			y1 = y + 1; y2 = y - 1; x1 = x + 1; x2 = x - 1;\n
		}\n
		else if (rad_3pi_4 <= rad || rad <= 0){	// NW-SE\n
			y1 = y - 1; y2 = y + 1; x1 = x + 1; x2 = x - 1;\n
		}\n
		else{\n
			x1 = x2 = x; y1 = y + 1; y2 = y - 1; \n
		}\n
		\n
		if (uint(y1) < uint(height) && uint(x1) < uint(width)){\n
			b = texelFetch(texMag, ivec2(x1, y1), 0).r; \n
			b = abs(b); \n
		}\n
		if (uint(y2) < uint(height) && uint(x2) < uint(width)){\n
			c = texelFetch(texMag, ivec2(x2, y2), 0).r; \n
			c = abs(c); \n
		}\n
		if ((a > b || (a == b && ((x1 == x + 1 && y1 == y) || (x1 == x && y1 == y + 1)))) && a > c){\n
		}\n
		else{\n
			a = 0.0;\n
		}\n
	}\n
	dst = a;\n
}\n
);
	return fragmentShaderCode;
}



static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = &ShaderNonmaximaSuppression; break;
	//case(CV_8U) :
	//case(CV_16U) : shader = &ShaderNonmaximaSuppressionU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = &ShaderNonmaximaSuppressionS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}


void nonmaximaSuppression(const GlsMat&mag, const GlsMat&angle, GlsMat& dst, float threshold){
	GLS_Assert(mag.type() == CV_32FC1);
	GLS_Assert(angle.type() == CV_32FC1);

	GlsMat _dst = getDstMat(mag,dst);

	glsShaderBase* shader = selectShader(mag.type());
	shader->Execute(mag, angle, threshold,_dst);

	dst = _dst;
}





}//namespace gls





