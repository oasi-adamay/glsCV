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

#ifndef _GLS_BASIC_OPERATION_H_
#define _GLS_BASIC_OPERATION_H_

#include "glsShader.h"
#include "GlsMat.h"


namespace gls
{


void ShaderBasicOperationInit(void);
void ShaderBasicOperationTerminate(void);

//GlsMat I/F

//! スカラと配列の 要素毎の和を求めます．(dst = scalar + src1)
void add(const vec4& scalar, const GlsMat& src, GlsMat& dst);

//! 2 つの配列同士の 要素毎の和を求めます(dst = src0 + src1)
void add(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

//! スカラと配列の 要素毎の差を求めます．(dst = scalar - src1)
void subtract(const vec4& scalar, const GlsMat& src, GlsMat& dst);

//! 2 つの配列同士の 要素毎の差を求めます(dst = src0 - src1)
void subtract(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

//! スカラと配列の 要素毎の積を求めます．(dst = scalar * src1)
void multiply(const vec4& scalar, const GlsMat& src, GlsMat& dst);

//! 2 つの配列同士の 要素毎の積を求めます(dst = src0 * src1)
void multiply(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

//! スカラと配列の 要素毎の商を求めます．(dst = scalar / src1)
void divide(const vec4& scalar, const GlsMat& src, GlsMat& dst);

//! 2 つの配列同士の  要素毎の商を求めます．(dst = src0 / src1)
void divide(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

//! スカラと配列の 要素毎の最小値を求めます．(dst = min(scalar, src))
void min(const vec4& scalar, const GlsMat& src, GlsMat& dst);

//! 2 つの配列同士の  要素毎の最小値を求めます．(dst = min(src0, src1))
void min(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

//! スカラと配列の 要素毎の最大値を求めます．(dst = max(scalar, src))
void max(const vec4& scalar, const GlsMat& src, GlsMat& dst);

//! 2 つの配列同士の  要素毎の最大値を求めます．(dst = max(src0, src1))
void max(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

//! 2 つのフーリエスペクトル同士の要素毎の乗算を行います．
//! conj = ture の場合、2 番目の配列を複素共役に変更できます．
void mulSpectrums(const GlsMat& src0, const GlsMat& src1, GlsMat& dst, bool conj = false);

//! 2 つのフーリエスペクトル同士の  要素毎の位相限定相関を求めます．
void mulSpectrumsPhaseOnly(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

//! 複素行列要素の大きさを求めます．(dst = sqrt(src.re**2 + src.im**2 )
void magSpectrums(const GlsMat& src, GlsMat& dst);

//! 複素行列要素の絶対値の2乗にoffset加算し、絶対値の自然対数を求めます．
//! (dst = log( sqrt(src.re**2 + src.im**2) + offset )
void logMagSpectrums(const GlsMat& src, GlsMat& dst,float offset);

//! 各配列要素の絶対値の自然対数を求めます．(dst = log(src))
void log(const GlsMat& src, GlsMat& dst);

//! 各配列要素を指数として，自然対数の底（ネイピア数）e のべき乗を求めます．(dst = exp(src))
void exp(const GlsMat& src, GlsMat& dst);

//! 各配列要素を累乗します． (dst = src**power)
void pow(const GlsMat& src, const float& power, GlsMat& dst);


	

}//namespace gls

#endif