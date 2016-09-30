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

#include "glsConvert.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsDraw shader
class glsShaderConvertBase : public glsShaderBase
{
protected:
	list<string> UniformNameList(void){
		list<string> lst;
		lst.push_back("texSrc");
		lst.push_back("scl");
		lst.push_back("flag");
		return lst;
	}
public:
	glsShaderConvertBase(const string& _name) :glsShaderBase(_name){}

};


//-----------------------------------------------------------------------------
// glsShaderConvert
class glsShaderConvert : public glsShaderConvertBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderConvert(void) :glsShaderConvertBase(__FUNCTION__) {}

};

//-----------------------------------------------------------------------------
// glsShaderConvertU unsigned to float
class glsShaderConvertU : public glsShaderConvertBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderConvertU(void) :glsShaderConvertBase(__FUNCTION__) {}

};

//-----------------------------------------------------------------------------
// glsShaderConvertS signed to float
class glsShaderConvertS : public glsShaderConvertBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderConvertS(void) :glsShaderConvertBase(__FUNCTION__) {}

};


//-----------------------------------------------------------------------------
//global 
glsShaderConvert ShaderConvert;
glsShaderConvertU ShaderConvertU;
glsShaderConvertS ShaderConvertS;



/* Constants for color conversion */
//	CV_BGR2BGRA = 0,
//	CV_RGB2RGBA = CV_BGR2BGRA,
//
//	CV_BGRA2BGR = 1,
//	CV_RGBA2RGB = CV_BGRA2BGR,
//
//	CV_BGR2RGBA = 2,
//	CV_RGB2BGRA = CV_BGR2RGBA,
//
//	CV_RGBA2BGR = 3,
//	CV_BGRA2RGB = CV_RGBA2BGR,
//
//	CV_BGR2RGB = 4,
//	CV_RGB2BGR = CV_BGR2RGB,
//
//	CV_BGRA2RGBA = 5,
//	CV_RGBA2BGRA = CV_BGRA2RGBA,
//
//	CV_BGR2GRAY = 6,
//	CV_RGB2GRAY = 7,
//	CV_GRAY2BGR = 8,
//	CV_GRAY2RGB = CV_GRAY2BGR,
//	CV_GRAY2BGRA = 9,
//	CV_GRAY2RGBA = CV_GRAY2BGRA,
//	CV_BGRA2GRAY = 10,
//	CV_RGBA2GRAY = 11,


//-----------------------------------------------------------------------------
//glsShaderConvert
string glsShaderConvert::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;\n
uniform sampler2D	texSrc;\n
uniform float	scl;\n
uniform int	flag;\n
layout (location = 0) out vec4 dst;\n
void main(void)\n
{\n
	vec4 data = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0);\n
	vec4 color = vec4(data.r*scl,data.g*scl,data.b*scl,data.a*scl);\n
	float gray;\n
	switch(flag){\n
	case(0)://CV_BGR2BGRA\n
	case(1)://CV_BGRA2BGR\n
		color = vec4(color.r,color.g,color.b,1.0);break;\n
	case(2)://CV_BGR2RGBA\n
	case(3)://CV_RGBA2BGR\n
	case(4)://CV_BGR2RGB\n
	case(5)://CV_BGRA2RGBA\n
		color = vec4(color.b,color.g,color.r,1.0);break;\n
	case(6)://CV_BGR2GRAY\n
		gray = color.b * 0.299 + color.g * 0.587 + color.r * 0.114;\n
		color = vec4(gray,gray,gray,1.0);break;\n
	case(7)://CV_RGB2GRAY\n
		gray = color.r * 0.299 + color.g * 0.587 + color.b * 0.114;\n
		color = vec4(gray,gray,gray,1.0);break;\n
	case(8)://CV_GRAY2BGR\n
	case(9)://CV_GRAY2BGRA\n
	case(10)://CV_BGRA2GRAY\n
	case(11)://CV_RGBA2GRAY\n
		gray = color.r;\n
		color = vec4(gray,gray,gray,1.0);break;\n
	}\n
	dst = color;\n
\n
}\n
);
	return fragmentShaderCode;
}




//-----------------------------------------------------------------------------
//glsShaderConvertU
string glsShaderConvertU::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;\n
uniform usampler2D	texSrc;\n
uniform float	scl;\n
uniform int	flag;\n
layout (location = 0) out vec4 dst;\n
void main(void)\n
{\n
	vec4 data = vec4(texelFetch(texSrc, ivec2(gl_FragCoord.xy),0));\n
	vec4 color = vec4(data.r*scl,data.g*scl,data.b*scl,data.a*scl);\n
	float gray;\n
	switch(flag){\n
	case(0)://CV_BGR2BGRA\n
	case(1)://CV_BGRA2BGR\n
		color = vec4(color.r,color.g,color.b,1.0);break;\n
	case(2)://CV_BGR2RGBA\n
	case(3)://CV_RGBA2BGR\n
	case(4)://CV_BGR2RGB\n
	case(5)://CV_BGRA2RGBA\n
		color = vec4(color.b,color.g,color.r,1.0);break;\n
	case(6)://CV_BGR2GRAY\n
		gray = color.b * 0.299 + color.g * 0.587 + color.r * 0.114;\n
		color = vec4(gray,gray,gray,1.0);break;\n
	case(7)://CV_RGB2GRAY\n
		gray = color.r * 0.299 + color.g * 0.587 + color.b * 0.114;\n
		color = vec4(gray,gray,gray,1.0);break;\n
	case(8)://CV_GRAY2BGR\n
	case(9)://CV_GRAY2BGRA\n
	case(10)://CV_BGRA2GRAY\n
	case(11)://CV_RGBA2GRAY\n
		gray = color.r;\n
		color = vec4(gray,gray,gray,1.0);break;\n
	}\n
	dst = color;\n
}\n
);
	return fragmentShaderCode;
}



//-----------------------------------------------------------------------------
//glsShaderConvertS
string glsShaderConvertS::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;\n
uniform isampler2D	texSrc;\n
uniform float	scl;\n
uniform int	flag;\n
layout (location = 0) out vec4 dst;\n
void main(void)\n
{\n
	vec4 data = vec4(texelFetch(texSrc, ivec2(gl_FragCoord.xy),0));\n
	vec4 color = vec4(data.r*scl,data.g*scl,data.b*scl,data.a*scl);\n
	float gray;\n
	switch(flag){\n
	case(0)://CV_BGR2BGRA\n
	case(1)://CV_BGRA2BGR\n
		color = vec4(color.r,color.g,color.b,1.0);break;\n
	case(2)://CV_BGR2RGBA\n
	case(3)://CV_RGBA2BGR\n
	case(4)://CV_BGR2RGB\n
	case(5)://CV_BGRA2RGBA\n
		color = vec4(color.b,color.g,color.r,1.0);break;\n
	case(6)://CV_BGR2GRAY\n
		gray = color.b * 0.299 + color.g * 0.587 + color.r * 0.114;\n
		color = vec4(gray,gray,gray,1.0);break;\n
	case(7)://CV_RGB2GRAY\n
		gray = color.r * 0.299 + color.g * 0.587 + color.b * 0.114;\n
		color = vec4(gray,gray,gray,1.0);break;\n
	case(8)://CV_GRAY2BGR\n
	case(9)://CV_GRAY2BGRA\n
	case(10)://CV_BGRA2GRAY\n
	case(11)://CV_RGBA2GRAY\n
		gray = color.r;\n
		color = vec4(gray,gray,gray,1.0);break;\n
	}\n
	dst = color;\n
}\n
);
	return fragmentShaderCode;
}



void convert(const GlsMat& src, GlsMat& dst, int rtype,const double scl){
	GLS_Assert(CV_MAT_DEPTH(rtype) == CV_32F || rtype < 0);
	if (rtype < 0) rtype = src.depth();

	GlsMat _dst = GlsMat(src.size(), CV_MAKETYPE(rtype, src.channels()));

	glsShaderConvertBase* shader = 0;

	switch (CV_MAT_DEPTH(src.type())){
	case(CV_32F) : shader = &ShaderConvert; break;
	case(CV_8U) :
	case(CV_16U) : shader = &ShaderConvertU; break;
	case(CV_8S) :
	case(CV_16S) :
	case(CV_32S) : shader = &ShaderConvertS; break;
	default: GLS_Assert(0);		//not implement
	}
	shader->Execute(src, (float)scl, -1, _dst);
	dst = _dst;

}

void cvtColor(const GlsMat& src, GlsMat& dst, const int code){
	GLS_Assert(src.glType() == GL_FLOAT);

	int ch=1;
	switch (code){
	case(CV_BGR2BGRA) :
//	case(CV_RGB2RGBA) :
	case(CV_BGR2RGBA) :
//	case(CV_RGB2BGRA) :
	case(CV_BGRA2RGBA) :
//	case(CV_RGBA2BGRA) :
	case(CV_GRAY2BGRA) :
//	case(CV_GRAY2RGBA) :
		ch = 4; break;
	case(CV_BGRA2BGR) :
//	case(CV_RGBA2RGB) :
	case(CV_RGBA2BGR) :
//	case(CV_BGRA2RGB) :
	case(CV_BGR2RGB) :
//	case(CV_RGB2BGR) :
	case(CV_GRAY2BGR) :
//	case(CV_GRAY2RGB) :
		ch = 3; break;
	case(CV_BGR2GRAY) :
	case(CV_RGB2GRAY) :
	case(CV_BGRA2GRAY) :
	case(CV_RGBA2GRAY) :
		ch = 1; break;
	default:
		GLS_Assert(0);
	}

	GlsMat _dst = GlsMat(src.size(), CV_MAKETYPE(CV_MAT_DEPTH(src.type()), ch));

	glsShaderConvertBase* shader = 0;

	switch (CV_MAT_DEPTH(src.type())){
	case(CV_32F) : shader = &ShaderConvert; break;
	case(CV_8U) :
	case(CV_16U) : shader = &ShaderConvertU; break;
	case(CV_8S) :
	case(CV_16S) :
	case(CV_32S) : shader = &ShaderConvertS; break;
	default: GLS_Assert(0);		//not implement
	}

	double scl = 1.0;
	shader->Execute(src, scl, code, _dst);
	dst = _dst;

}


}//namespace gls



