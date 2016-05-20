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


#include "glsMinMaxLoc.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsShaderMinMaxLoc
class glsShaderMinMaxLoc : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);
	list<string> UniformNameList(void);

public:
	glsShaderMinMaxLoc(void) :glsShaderBase(__FUNCTION__){}

};


//-----------------------------------------------------------------------------
//global 
glsShaderMinMaxLoc ShaderMinMaxLoc;


//-----------------------------------------------------------------------------
//glsShaderMinMaxLoc
string glsShaderMinMaxLoc::FragmentShaderCode(void){
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform sampler2D	texSrc0;\n"
"uniform sampler2D	texSrc1;\n"
"uniform isampler2D	texSrc2;\n"
"uniform isampler2D	texSrc3;\n"
"uniform int		path;\n"
"layout (location = 0) out float minval;\n"
"layout (location = 1) out float maxval;\n"
"layout (location = 2) out ivec2 minloc;\n"
"layout (location = 3) out ivec2 maxloc;\n"
"#define FLT_MAX  3.402823e+38\n"
"void main(void)\n"
"{\n"
"	ivec2 texSize = textureSize(texSrc0,0);\n"
"	if(path==0){\n"
"		float minv = float (FLT_MAX);\n"
"		float maxv = float(-FLT_MAX);\n"
"		int minl = 0;\n"
"		int maxl = 0;\n"
"		float data;\n"
"		for (int x = 0; x < texSize.x; x++){\n"
"			data = texelFetch(texSrc0, ivec2(x, gl_FragCoord.y), 0).r;\n"
"			if(data<minv) {minv = data; minl = x;}\n"
"			if(data>maxv) {maxv = data; maxl = x;}\n"
"		}\n"
"		minval = minv;\n"
"		maxval = maxv;\n"
"		minloc = ivec2(minl,0);\n"
"		maxloc = ivec2(maxl,0);\n"
"	}\n"
"	else{\n"
"		float minv = float (FLT_MAX);\n"
"		float maxv = float(-FLT_MAX);\n"
"		int minl = 0;\n"
"		int maxl = 0;\n"
"		float dmin;\n"
"		float dmax;\n"
"		for (int y = 0; y < texSize.y; y++){\n"
"			dmin = texelFetch(texSrc0, ivec2(0,y), 0).r;\n"
"			dmax = texelFetch(texSrc1, ivec2(0,y), 0).r;\n"
"			if(dmin<minv) {minv = dmin; minl = y;}\n"
"			if(dmax>maxv) {maxv = dmax; maxl = y;}\n"
"		}\n"
"		minval = minv;\n"
"		maxval = maxv;\n"
"		minloc = ivec2(texelFetch(texSrc2, ivec2(0,minl), 0).r,minl);\n"
"		maxloc = ivec2(texelFetch(texSrc3, ivec2(0,maxl), 0).r,maxl);\n"
"	}\n"
"}\n"
;
	return fragmentShaderCode;
}

list<string> glsShaderMinMaxLoc::UniformNameList(void){
	list<string> lst;
	lst.push_back("path");
	lst.push_back("texSrc0");
	lst.push_back("texSrc1");
	lst.push_back("texSrc2");
	lst.push_back("texSrc3");
	return lst;
}


static 
glsShaderBase* selectShader(int type){
	glsShaderBase* shader = 0;
	switch (CV_MAT_DEPTH(type)){
	case(CV_32F) : shader = &ShaderMinMaxLoc; break;
	//case(CV_8U) :
	//case(CV_16U) : shader = &ShaderMinMaxLocU; break;
	//case(CV_8S) :
	//case(CV_16S) :
	//case(CV_32S) : shader = &ShaderMinMaxLocS; break;
	default: GLS_Assert(0);		//not implement
	}
	return shader;
}



void minMaxLoc(const GlsMat& src, double* minVal, double* maxVal, Point* minLoc, Point* maxLoc, const GlsMat& mask){
	GLS_Assert(src.depth() == CV_32F);
	GLS_Assert(mask.empty());	// not implement yet

	GlsMat _minvec;
	GlsMat _maxvec;
	GlsMat _minloc;
	GlsMat _maxloc;

	if (minVal) _minvec = GlsMat(Size(1, src.rows), CV_MAKE_TYPE(src.depth(), 1));
	if (maxVal) _maxvec = GlsMat(Size(1, src.rows), CV_MAKE_TYPE(src.depth(), 1));
	if (minLoc) _minloc = GlsMat(Size(1, src.rows), CV_32SC1);
	if (maxLoc) _maxloc = GlsMat(Size(1, src.rows), CV_32SC1);

	glsShaderBase* shader = selectShader(src.type());

	{		//path:0
		shader->Execute(0, src, 0, 0, 0, _minvec, _maxvec, _minloc, _maxloc);
	}
	{		//path:1
		GlsMat _minvec1;
		GlsMat _maxvec1;
		GlsMat _minloc1;
		GlsMat _maxloc1;

		if (minVal) _minvec1 = GlsMat(Size(1, 1), CV_MAKE_TYPE(src.depth(), 1));
		if (maxVal) _maxvec1 = GlsMat(Size(1, 1), CV_MAKE_TYPE(src.depth(), 1));
		if (minLoc) _minloc1 = GlsMat(Size(1, 1), CV_32SC2);
		if (maxLoc) _maxloc1 = GlsMat(Size(1, 1), CV_32SC2);

		shader->Execute(1, _minvec, _maxvec, _minloc, _maxloc, _minvec1, _maxvec1, _minloc1, _maxloc1);

		Mat minval;
		Mat maxval;
		Mat minloc;
		Mat maxloc;
		if (minVal) _minvec1.download(minval);
		if (maxVal) _maxvec1.download(maxval);
		if (minLoc) _minloc1.download(minloc);
		if (maxLoc) _maxloc1.download(maxloc);

		if (minVal) *minVal = minval.at<float>(0, 0);
		if (maxVal) *maxVal = maxval.at<float>(0, 0);
		if (minLoc) *minLoc = minloc.at<Vec2i>(0, 0);
		if (maxLoc) *maxLoc = maxloc.at<Vec2i>(0, 0);
	}




}

}//namespace gls





