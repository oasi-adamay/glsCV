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

#include "glsFft.h"
#include "glsCopy.h"	//tiled / untiled
#include "glsMerge.h"


#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _DEBUG
//#if 1
#include "Timer.h"
#define _TMR_(...)  Timer tmr(__VA_ARGS__)
#else
#define _TMR_(...)
#endif

namespace gls
{

// glsFft shader
class glsShaderFftRadix2 : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);
	list<string> UniformNameList(void);

public:
	glsShaderFftRadix2(void) :glsShaderBase(__FUNCTION__){}

};


//-----------------------------------------------------------------------------
//global 
glsShaderFftRadix2 ShaderFftRadix2;



//-----------------------------------------------------------------------------
//glsShaderFftRadix2
// Stockham,DIT radix2
string glsShaderFftRadix2::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;\n
uniform sampler2D	texSrc0;\n
uniform sampler2D	texSrc1;\n
uniform sampler2D	texW;\n
uniform  int i_flag;	//bit0:(0:holizontal 1:vertical)\n
uniform  int i_N;\n
uniform  int i_p;\n
uniform  int i_q;\n
uniform  float f_xscl;\n
uniform  float f_yscl;\n
uniform  float f_xconj;\n
uniform  float f_yconj;\n
\n
layout (location = 0) out vec2 dst0;\n
layout (location = 1) out vec2 dst1;\n
\n
#define FLAG_DIR	 (1<<0)\n
\n
\n
int insertZeroBits(\n
	const int src,\n
	const int idx,\n
	const int num\n
	)\n
{\n
	int ret = src << num;\n
	ret &= ~((1 << (idx + num)) - 1);\n
	ret |= src & ((1 << idx) - 1);\n
	return ret;\n
}\n
\n
void main(void)\n
{\n
	int p = i_p;\n
	int q = i_q;\n
	int N = i_N;\n
	int dir = ((i_flag & FLAG_DIR)==0) ?0:1;\n
	float xscl = f_xscl;\n
	float yscl = f_yscl;\n
	float xconj = f_xconj;\n
	float yconj = f_yconj;\n
\n
	int n;\n
	vec2 x0;\n
	vec2 x1;\n
	vec2 w;\n
\n
	n= int(gl_FragCoord[dir]);\n
	int iw = (n >> q) << q;\n
	int ix0 = insertZeroBits(n, q, 1);\n
	int ix1 = ix0 + (1 << q);\n
	w = texelFetch(texW,ivec2(iw,0),0).rg;\n
\n
\n
	if(dir ==0){\n
		if(ix0 < N/2) x0 = texelFetch(texSrc0,ivec2(ix0,gl_FragCoord.y),0).rg;\n
		else x0 = texelFetch(texSrc1,ivec2(ix0-N/2,gl_FragCoord.y),0).rg;\n
\n
		if(ix1 < N/2) x1 = texelFetch(texSrc0,ivec2(ix1,gl_FragCoord.y),0).rg;\n
		else x1 = texelFetch(texSrc1,ivec2(ix1-N/2,gl_FragCoord.y),0).rg;\n
	}\n
	else{\n
		if(ix0 < N/2) x0 = texelFetch(texSrc0,ivec2(gl_FragCoord.x,ix0),0).rg;\n
		else x0 = texelFetch(texSrc1,ivec2(gl_FragCoord.x,ix0-N/2),0).rg;\n
\n
		if(ix1 < N/2) x1 = texelFetch(texSrc0,ivec2(gl_FragCoord.x,ix1),0).rg;\n
		else x1 = texelFetch(texSrc1,ivec2(gl_FragCoord.x,ix1-N/2),0).rg;\n
	}\n
\n
//	x0 = x0*xscl;\n
//	x1 = x1*xscl;\n
	x0.g = x0.g*xconj;\n
	x1.g = x1.g*xconj;\n
\n
	vec2 tmp;\n
	tmp.r = x1.r * w.r - x1.g * w.g;\n
	tmp.g = x1.r * w.g + x1.g * w.r;\n
\n
	vec2 y0;\n
	vec2 y1;\n
\n
	y0 = x0 + tmp;\n
	y1 = x0 - tmp;\n
\n
	y0 = y0*yscl;\n
	y1 = y1*yscl;\n
	y0.g = y0.g*yconj;\n
	y1.g = y1.g*yconj;\n
\n
	dst0 = y0;\n
	dst1 = y1;\n
\n
}\n
);
	return fragmentShaderCode;
}

list<string> glsShaderFftRadix2::UniformNameList(void){
	list<string> lst;
	lst.push_back("texSrc0");
	lst.push_back("texSrc1");
	lst.push_back("texW");
	lst.push_back("i_flag");
	lst.push_back("i_N");
	lst.push_back("i_p");
	lst.push_back("i_q");
	lst.push_back("f_xscl");
	lst.push_back("f_yscl");
	lst.push_back("f_xconj");
	lst.push_back("f_yconj");
	return lst;
}

//---------------------------------------------------------------------------
//value is power of 2
static bool IsPow2(unsigned int x){
	return (((x)&(x - 1)) == 0);
}


//-----------------------------------------------------------------------------
void fft(const GlsMat& _src, GlsMat& dst, int flag){
	GLS_Assert(_src.channels() == 2 || _src.channels() == 1);
	GLS_Assert(_src.depth() == CV_32F);


	int N = _src.cols;
	GLS_Assert(IsPow2(N));

	GlsMat src;
	if (_src.channels() == 1){
		// to complex mat
		vector<GlsMat> tmp(2);
		tmp[0] = _src;
		gls::merge(tmp, src);
	}
	else{
		src = _src;
	}


	Size blkNum(2,2);
	vector<vector<GlsMat>> _dst0 = vector<vector<GlsMat>>(blkNum.height, vector<GlsMat>(blkNum.width));
	vector<vector<GlsMat>> _dst1 = vector<vector<GlsMat>>(blkNum.height, vector<GlsMat>(blkNum.width));

	gls::tiled(src, _dst0, blkNum);
	for (int by = 0; by < blkNum.height; by++){
		for (int bx = 0; bx < blkNum.width; bx++){
			_dst1[by][bx] = GlsMat(Size(src.cols / blkNum.width, src.rows / blkNum.height), src.type());
		}
	}


	GlsMat texW(Size(N / 2, 1), src.type());

	//---------------------------------
	// upload twidle texture
	{
		_TMR_("-twidle:  \t");

		Mat w(Size(N / 2, 1), CV_32FC2);
#ifdef _OPENMP
//#pragma omp parallel for
#pragma omp parallel for if(N>=256)
#endif
		for (int n = 0; n < N / 2; n++){
			float jw = (float)(-2 * M_PI * n / N);
			Vec2f val(cos(jw), sin(jw));
			w.at<Vec2f>(0, n) = val;
		}
		texW.upload(w);
	}

	vector<vector<GlsMat>>* texbuf[2] = { &_dst0, &_dst1 };

	//Execute
	int bank = 0;

	{
		_TMR_("-execute:\t");

		int Q = 0;
		while ((1 << Q) < N){ Q++; }

		vector<GlsMat> texSrc(2);
		vector<GlsMat> texDst(2);

		// --- FFT rows ----
		for (int p = 0, q = Q - 1; q >= 0; p++, q--, bank = bank ^ 1) {
			for (int i = 0; i < 2; i++){
				for (int j = 0; j < 2; j++){
					texSrc[j] = (*texbuf[bank])[i][j];
					texDst[j] = (*texbuf[bank ^ 1])[i][j];
				}
				float yscl = ((flag & GLS_FFT_SCALE) && (q == 0)) ? 1.0f / (float)N : 1.0f;
				float xscl = 1.0f;
				float xconj = ((flag & GLS_FFT_INVERSE) && (p == 0)) ? -1.0f : 1.0f;
				float yconj = 1.0f;
				ShaderFftRadix2.Execute(texSrc[0], texSrc[1], texW, 0, N, p, q, xscl, yscl, xconj, yconj, texDst[0], texDst[1]);

			}
		}
		// --- FFT cols ----
		for (int p = 0, q = Q - 1; q >= 0; p++, q--, bank = bank ^ 1) {
			for (int j = 0; j < 2; j++){
				for (int i = 0; i < 2; i++){
					texSrc[i] = (*texbuf[bank])[i][j];
					texDst[i] = (*texbuf[bank ^ 1])[i][j];
				}
				float yscl = ((flag & GLS_FFT_SCALE) && (q == 0)) ? 1.0f / (float)N : 1.0f;
				float xscl = 1.0f;
				float xconj = 1.0f;
				float yconj = ((flag & GLS_FFT_INVERSE) && (q == 0)) ? -1.0f : 1.0f;
				ShaderFftRadix2.Execute(texSrc[0], texSrc[1], texW, 1, N, p, q, xscl, yscl, xconj, yconj, texDst[0], texDst[1]);
			}
		}
	}



	if (flag & GLS_FFT_SHIFT){
		(*texbuf[bank ^ 1])[0][0] = (*texbuf[bank])[1][1];
		(*texbuf[bank ^ 1])[0][1] = (*texbuf[bank])[1][0];
		(*texbuf[bank ^ 1])[1][0] = (*texbuf[bank])[0][1];
		(*texbuf[bank ^ 1])[1][1] = (*texbuf[bank])[0][0];
		bank = bank ^ 1;
	}

	gls::untiled(*texbuf[bank], dst);
}





}//namespace gls

