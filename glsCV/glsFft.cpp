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

// use radix 4
//#define _USE_RADIX4

// disable openmp
// unittestで稀に例外が発生するため。
#ifdef _OPENMP
#undef _OPENMP
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
#version 330 core	\n
precision highp float;	\n
#define FLAG_DIR	 (1<<0)	\n

uniform sampler2D	texSrc0;
uniform sampler2D	texSrc1;
uniform sampler2D	texW;
uniform ivec4 ctrl;	// [0]:flag(bit0:(0:holizontal 1:vertical) [1]:N  [2]:p [3]:q
uniform vec4 scale;	// [0,1]:xscl [2,3]:yscl

layout (location = 0) out vec2 dst0;
layout (location = 1) out vec2 dst1;

int insertZeroBits(const int src,const int idx,const int num){
	int ret = src << num;
	ret &= ~((1 << (idx + num)) - 1);
	ret |= src & ((1 << idx) - 1);
	return ret;
}
vec2 mul(vec2 a, vec2 b){
	vec2 ans;
	ans[0] = a[0] * b[0] - a[1] * b[1];
	ans[1] = a[0] * b[1] + a[1] * b[0];
	return ans;
}
void main(void){
	int p = ctrl[2];
	int q = ctrl[3];
	int N = ctrl[1];
	int dir = ((ctrl[0] & FLAG_DIR) == 0) ? 0 : 1;
	vec2 xscl = scale.rg;
	vec2 yscl = scale.ba;

	int n = int(gl_FragCoord[dir]);
	int iw = (n >> q) << q;
	int ix0 = insertZeroBits(n, q, 1);
	int ix1 = ix0 + (1 << q);

	vec2 x0, x1;
	vec2 w;
	w = texelFetch(texW, ivec2(iw, 0), 0).rg;

	int tx0 = ix0 / (N / 2); ix0 = ix0 % (N / 2);
	int tx1 = ix1 / (N / 2); ix1 = ix1 % (N / 2);
	ivec2 coord0, coord1, coord2, coord3;
	if (dir == 0){
		coord0 = ivec2(ix0, gl_FragCoord.y);
		coord1 = ivec2(ix1, gl_FragCoord.y);
	}
	else{
		coord0 = ivec2(gl_FragCoord.x, ix0);
		coord1 = ivec2(gl_FragCoord.x, ix1);
	}

	switch (tx0){
	case(0) : x0 = texelFetch(texSrc0, coord0, 0).rg; break;
	case(1) : x0 = texelFetch(texSrc1, coord0, 0).rg; break;
	};
	switch (tx1){
	case(0) : x1 = texelFetch(texSrc0, coord1, 0).rg; break;
	case(1) : x1 = texelFetch(texSrc1, coord1, 0).rg; break;
	};

	// input scaler 
	x0 = x0*xscl;
	x1 = x1*xscl;

	// twidle
	vec2 tmp = mul(x1, w);
	vec2 y0 = x0 + tmp;
	vec2 y1 = x0 - tmp;

	// output scaler 
	dst0 = y0*yscl;
	dst1 = y1*yscl;

}
);
	return fragmentShaderCode;
}

list<string> glsShaderFftRadix2::UniformNameList(void){
	list<string> lst;
	lst.push_back("texSrc0");
	lst.push_back("texSrc1");
	lst.push_back("texW");
	lst.push_back("ctrl");
	lst.push_back("scale");
	return lst;
}




//-----------------------------------------------------------------------------
// glsFft shader
class glsShaderFftRadix4 : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);
	list<string> UniformNameList(void);

public:
	glsShaderFftRadix4(void) :glsShaderBase(__FUNCTION__){}
};
glsShaderFftRadix4 ShaderFftRadix4;


//-----------------------------------------------------------------------------
//glsShaderFftRadix4
// Stockham,DIT Radix4
string glsShaderFftRadix4::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core\n
precision highp float;	\n
#define FLAG_DIR	 (1<<0)	\n
#define FLAG_RADIX4	 (1<<1)	\n
uniform sampler2D	texSrc0;
uniform sampler2D	texSrc1;
uniform sampler2D	texSrc2;
uniform sampler2D	texSrc3;
uniform sampler2D	texW;
uniform ivec4 ctrl;	// [0]:flag(bit0:(0:holizontal 1:vertical) bit1:(0:radix2 1:radix4)) [1]:N  [2]:p [3]:q
uniform vec4 scale;	// [0,1]:xscl [2,3]:yscl
layout(location = 0) out vec2 dst0;
layout(location = 1) out vec2 dst1;
layout(location = 2) out vec2 dst2;
layout(location = 3) out vec2 dst3;
int insertZeroBits(const int src,const int idx,const int num){
	int ret = src << num;
	ret &= ~((1 << (idx + num)) - 1);
	ret |= src & ((1 << idx) - 1);
	return ret;
}
vec2 mul(vec2 a, vec2 b){
	vec2 ans;
	ans[0] = a[0] * b[0] - a[1] * b[1];
	ans[1] = a[0] * b[1] + a[1] * b[0];
	return ans;
}
void main(void)
{
	int p = ctrl[2];
	int q = ctrl[3];
	int N = ctrl[1];
	int dir = ((ctrl[0] & FLAG_DIR) == 0) ? 0 : 1;
	int radix4 = ((ctrl[0] & FLAG_RADIX4) == 0) ? 0 : 1;
	vec2 xscl = scale.rg;
	vec2 yscl = scale.ba;
	int n = int(gl_FragCoord[dir]);

	vec2 x0,x1,x2,x3;
	vec2 y0,y1,y2,y3;
	int ix0,ix2,ix1,ix3;

	if (radix4==0){ //radix2
		ix0 = n;
		ix1 = ix0 + (N / 4) * 1;
		ix2 = ix0 + (N / 4) * 2;
		ix3 = ix0 + (N / 4) * 3;
	}
	else{	//radix4
		ix0 = insertZeroBits(n, q, 2);
		ix1 = ix0 + (1 << q)*1;
		ix2 = ix0 + (1 << q)*2;
		ix3 = ix0 + (1 << q)*3;
	}

	int tx0 = ix0 / (N / 4); ix0 = ix0 % (N / 4);
	int tx1 = ix1 / (N / 4); ix1 = ix1 % (N / 4);
	int tx2 = ix2 / (N / 4); ix2 = ix2 % (N / 4);
	int tx3 = ix3 / (N / 4); ix3 = ix3 % (N / 4);

	ivec2 coord0, coord1, coord2, coord3;
	if (dir == 0){
		coord0 = ivec2(ix0, gl_FragCoord.y);
		coord1 = ivec2(ix1, gl_FragCoord.y);
		coord2 = ivec2(ix2, gl_FragCoord.y);
		coord3 = ivec2(ix3, gl_FragCoord.y);
	}
	else{
		coord0 = ivec2(gl_FragCoord.x,ix0);
		coord1 = ivec2(gl_FragCoord.x,ix1);
		coord2 = ivec2(gl_FragCoord.x,ix2);
		coord3 = ivec2(gl_FragCoord.x,ix3);
	}

	switch (tx0){
	case(0) : x0 = texelFetch(texSrc0, coord0, 0).rg; break;
	case(1) : x0 = texelFetch(texSrc1, coord0, 0).rg; break;
	case(2) : x0 = texelFetch(texSrc2, coord0, 0).rg; break;
	case(3) : x0 = texelFetch(texSrc3, coord0, 0).rg; break;
	};
	switch (tx1){
	case(0) : x1 = texelFetch(texSrc0, coord1, 0).rg; break;
	case(1) : x1 = texelFetch(texSrc1, coord1, 0).rg; break;
	case(2) : x1 = texelFetch(texSrc2, coord1, 0).rg; break;
	case(3) : x1 = texelFetch(texSrc3, coord1, 0).rg; break;
	};
	switch (tx2){
	case(0) : x2 = texelFetch(texSrc0, coord2, 0).rg; break;
	case(1) : x2 = texelFetch(texSrc1, coord2, 0).rg; break;
	case(2) : x2 = texelFetch(texSrc2, coord2, 0).rg; break;
	case(3) : x2 = texelFetch(texSrc3, coord2, 0).rg; break;
	};
	switch (tx3){
	case(0) : x3 = texelFetch(texSrc0, coord3, 0).rg; break;
	case(1) : x3 = texelFetch(texSrc1, coord3, 0).rg; break;
	case(2) : x3 = texelFetch(texSrc2, coord3, 0).rg; break;
	case(3) : x3 = texelFetch(texSrc3, coord3, 0).rg; break;
	};

	// input scaler 
	x0 = x0*xscl;
	x1 = x1*xscl;
	x2 = x2*xscl;
	x3 = x3*xscl;

	if (radix4==0){ //radix2
		y0 = x0 + x2;
		y1 = x1 + x3;
		y2 = x0 - x2;
		y3 = x1 - x3;
	}
	else{	//radix4
		int iw = (n >> q) << q;
		vec2 w1 = texelFetch(texW, ivec2(iw, 0), 0).rg;
		vec2 w2 = mul(w1, w1);
		vec2 w3 = mul(w1, w2);
		vec2 a = x0;
		vec2 b = mul(x1, w1);
		vec2 c = mul(x2, w2);
		vec2 d = mul(x3, w3);
		vec2 apc = a + c;
		vec2 amc = a - c;
		vec2 bpd = b + d;
		vec2 jbmd = mul(vec2(0.0,1.0), (b - d));

		y0 = apc + bpd;
		y1 = amc - jbmd;
		y2 = apc - bpd;
		y3 = amc + jbmd;
	}
	// output scaler 
	dst0 = y0*yscl;
	dst1 = y1*yscl;
	dst2 = y2*yscl;
	dst3 = y3*yscl;
}
	);
	return fragmentShaderCode;
}

list<string> glsShaderFftRadix4::UniformNameList(void){
	list<string> lst;
	lst.push_back("texSrc0");
	lst.push_back("texSrc1");
	lst.push_back("texSrc2");
	lst.push_back("texSrc3");
	lst.push_back("texW");
	lst.push_back("ctrl");
	lst.push_back("scale");
	return lst;
}



//---------------------------------------------------------------------------
//value is power of 2
static bool IsPow2(unsigned int x){
	return (((x)&(x - 1)) == 0);
}


//-----------------------------------------------------------------------------
static void fft_redix2(const GlsMat& src, GlsMat& dst, int flag){
	GLS_Assert(src.channels() == 2);
	GLS_Assert(src.depth() == CV_32F);


	int N = src.cols;
	GLS_Assert(IsPow2(N));


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
				int _flag = 0; //radix2,horizontal
				Vec4i ctrl(_flag, N, p, q);
				Vec2f xscl(1.0f, 1.0f);
				Vec2f yscl(1.0f, 1.0f);
				if ((flag & GLS_FFT_INVERSE) && (p == 0)) xscl[1] *= -1;
				if ((flag & GLS_FFT_SCALE) && (q == 0)) yscl /= (float)N;
				Vec4f scale(xscl[0], xscl[1], yscl[0], yscl[1]);
				ShaderFftRadix2.Execute(texSrc[0], texSrc[1], texW, ctrl, scale, texDst[0], texDst[1]);

			}
		}
		// --- FFT cols ----
		for (int p = 0, q = Q - 1; q >= 0; p++, q--, bank = bank ^ 1) {
			for (int j = 0; j < 2; j++){
				for (int i = 0; i < 2; i++){
					texSrc[i] = (*texbuf[bank])[i][j];
					texDst[i] = (*texbuf[bank ^ 1])[i][j];
				}
				int _flag = (1 << 0); //radix2,vertical
				Vec4i ctrl(_flag, N, p, q);
				Vec2f xscl(1.0f, 1.0f);
				Vec2f yscl(1.0f, 1.0f);
				if ((flag & GLS_FFT_INVERSE) && (q == 0)) yscl[1] *= -1;
				if ((flag & GLS_FFT_SCALE) && (q == 0)) yscl /= (float)N;
				Vec4f scale(xscl[0], xscl[1], yscl[0], yscl[1]);
				ShaderFftRadix2.Execute(texSrc[0], texSrc[1], texW, ctrl, scale, texDst[0], texDst[1]);
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


//-----------------------------------------------------------------------------
static void fft_redix4(const GlsMat& src, GlsMat& dst, int flag){
	GLS_Assert(src.channels() == 2);
	GLS_Assert(src.depth() == CV_32F);


	int N = src.cols;
	GLS_Assert(IsPow2(N));


	Size blkNum(4, 4);
	vector<vector<GlsMat>> _dst0 = vector<vector<GlsMat>>(blkNum.height, vector<GlsMat>(blkNum.width));
	vector<vector<GlsMat>> _dst1 = vector<vector<GlsMat>>(blkNum.height, vector<GlsMat>(blkNum.width));

	gls::tiled(src, _dst0, blkNum);
	for (int by = 0; by < blkNum.height; by++){
		for (int bx = 0; bx < blkNum.width; bx++){
			_dst1[by][bx] = GlsMat(Size(src.cols / blkNum.width, src.rows / blkNum.height), src.type());
		}
	}


	GlsMat texW(Size(N / 4, 1), src.type());

	//---------------------------------
	// upload twidle texture
	{
		_TMR_("-twidle:  \t");

		Mat w(texW.size(), CV_32FC2);
#ifdef _OPENMP
		//#pragma omp parallel for
#pragma omp parallel for if(N>=256)
#endif
		for (int n = 0; n < w.cols; n++){
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

		vector<GlsMat> texSrc(4);
		vector<GlsMat> texDst(4);

		// --- FFT rows ----
		//cout << "---FFT rows---" << endl;
		while ((1 << Q) < N){ Q++; }
		if (Q % 2){	//radix2
			//cout << "---radix2(x2)---" << endl;
			for (int i = 0; i < 4; i++){
				for (int j = 0; j < 4; j++){
					texSrc[j] = (*texbuf[bank])[i][j];
					texDst[j] = (*texbuf[bank ^ 1])[i][j];
				}
				int _flag = 0; //radix2,horizontal
				Vec4i ctrl(_flag,N,0,0);
				Vec2f xscl(1.0f, 1.0f);
				Vec2f yscl(1.0f, 1.0f);
				Vec4f scale(xscl[0], xscl[1], yscl[0], yscl[1]);
				ShaderFftRadix4.Execute(texSrc[0], texSrc[1], texSrc[2], texSrc[3], texW, ctrl, scale, texDst[0], texDst[1], texDst[2], texDst[3]);
			}
			Q -= 1;
			bank ^= 1;
		}

		for (int p = 0, q = Q - 2; q >= 0; p += 2, q -= 2, bank = bank ^ 1) {
			//cout << "---radix4---" << endl;
			//cout << "p:" << p << "\t";
			//cout << "q:" << q << "\t";
			//cout << endl;

			for (int i = 0; i < 4; i++){
				for (int j = 0; j < 4; j++){
					texSrc[j] = (*texbuf[bank])[i][j];
					texDst[j] = (*texbuf[bank ^ 1])[i][j];
				}
				int _flag = 1 << 1; //radix4,horizontal
				Vec4i ctrl(_flag, N, p, q);
				Vec2f xscl(1.0f, 1.0f);
				Vec2f yscl(1.0f, 1.0f);
				if ((flag & GLS_FFT_INVERSE) && (p == 0)) xscl[1] *= -1;
				if ((flag & GLS_FFT_SCALE) && (q == 0)) yscl /= (float)N;
				Vec4f scale(xscl[0], xscl[1], yscl[0], yscl[1]);
				ShaderFftRadix4.Execute(texSrc[0], texSrc[1], texSrc[2], texSrc[3], texW, ctrl, scale, texDst[0], texDst[1], texDst[2], texDst[3]);
			}
		}
		// --- FFT cols ----
		//cout << "---FFT cols---" << endl;
		Q = 0;
		while ((1 << Q) < N){ Q++; }
		if (Q % 2){	//radix2
			//cout << "---radix2(x2)---" << endl;
			for (int j = 0; j < 4; j++){
				for (int i = 0; i < 4; i++){
					texSrc[i] = (*texbuf[bank])[i][j];
					texDst[i] = (*texbuf[bank ^ 1])[i][j];
				}
				int _flag = 1 << 0; //radix2,vertical
				Vec4i ctrl(_flag, N, 0, 0);
				Vec2f xscl(1.0f, 1.0f);
				Vec2f yscl(1.0f, 1.0f);
				Vec4f scale(xscl[0], xscl[1], yscl[0], yscl[1]);
				ShaderFftRadix4.Execute(texSrc[0], texSrc[1], texSrc[2], texSrc[3], texW, ctrl, scale, texDst[0], texDst[1], texDst[2], texDst[3]);
			}
			Q -= 1;
			bank ^= 1;
		}

		for (int p = 0, q = Q - 2; q >= 0; p += 2, q -= 2, bank = bank ^ 1) {
			//cout << "---radix4---" << endl;
			//cout << "p:" << p << "\t";
			//cout << "q:" << q << "\t";
			//cout << endl;

			for (int j = 0; j < 4; j++){
				for (int i = 0; i < 4; i++){
					texSrc[i] = (*texbuf[bank])[i][j];
					texDst[i] = (*texbuf[bank ^ 1])[i][j];
				}
				int _flag = (1<<1) + (1<<0); //radix4,vertical
				Vec4i ctrl(_flag, N, p, q);
				Vec2f xscl(1.0f, 1.0f);
				Vec2f yscl(1.0f, 1.0f);
				if ((flag & GLS_FFT_INVERSE) && (q == 0)) yscl[1] *= -1;
				if ((flag & GLS_FFT_SCALE) && (q == 0)) yscl /= (float)N;
				Vec4f scale(xscl[0], xscl[1], yscl[0], yscl[1]);
				ShaderFftRadix4.Execute(texSrc[0], texSrc[1], texSrc[2], texSrc[3], texW, ctrl, scale, texDst[0], texDst[1], texDst[2], texDst[3]);
			}
		}
	}



	if (flag & GLS_FFT_SHIFT){
		const int _tbl[4] = { 2, 3, 0, 1 };
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				(*texbuf[bank ^ 1])[_tbl[i]][_tbl[j]] = (*texbuf[bank])[i][j];
			}
		}
		bank = bank ^ 1;
	}

	gls::untiled(*texbuf[bank], dst);
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

#ifdef  _USE_RADIX4
	GLint max_color_attachement;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachement);
	//cout << "GL_MAX_COLOR_ATTACHMENTS:" << max_color_attachement << endl;

	if (max_color_attachement < 4){
		fft_redix2(src, dst, flag);
	}
	else{
		fft_redix4(src, dst, flag);
	}
#else
	fft_redix2(src, dst, flag);
#endif
}



}//namespace gls

