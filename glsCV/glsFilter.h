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

#ifndef _GLS_FILTER_H_
#define _GLS_FILTER_H_

#include "glsCV.h"
#include "GlsMat.h"
#include "glsShader.h"

namespace gls
{


void ShaderFilterInit(void);
void ShaderFilterTerminate(void);




/*!
分離型2次元線形フィルタを，画像に適用します．

@param src 入力画像．CV_32FC1,CV_32FC2,CV_32FC3,CV_32FC4型の画像がサポートされます．
@param dst 出力画像． src と同じサイズ，同じチャンネル数になります．
@param ddepth 出力画像のビット深度(無視されます)
@param kernelX　row方向フィルタ係数(1ch)　奇数TAPのみサポートします。
@param kernelY　col方向フィルタ係数(1ch)　奇数TAPのみサポートします。
*/
void sepFilter2D(const GlsMat& src, GlsMat& dst, int ddepth, const Mat& kernelX, const Mat& kernelY);

/*!
カーネルを用いて画像の畳み込みを行います．

@param src 入力画像．CV_32FC1,CV_32FC2,CV_32FC3,CV_32FC4型の画像がサポートされます．
@param dst 出力画像． src と同じサイズ，同じチャンネル数になります．
@param ddepth 出力画像のビット深度(無視されます)
@param シングルチャンネルの浮動小数点型行列である畳み込みカーネル
*/
void filter2D(const GlsMat& src, GlsMat& dst, int ddepth, const Mat& kernel);

/*!
Gaussian フィルタを用いて，画像を平滑化します．

@param src 入力画像．CV_32FC1,CV_32FC2,CV_32FC3,CV_32FC4型の画像がサポートされます．
@param dst 出力画像． src と同じサイズ，同じチャンネル数になります．
@param ksize Gaussian カーネルサイズ． ksize.width と ksize.height を別々に指定できますが，共に正の奇数でなければいけません．または，0を指定することも可能で，その場合， sigmaX と sigmaY から計算されます．
@param sigmaX (sigmaY) – X および Y 方向の，Gaussian カーネルの標準偏差． sigmaY が0の場合， sigmaX と同じ値にセットされます．どちらも0だった場合は， ksize.width と ksize.height から，それぞれ計算されます．
*/
void GaussianBlur(const GlsMat& src, GlsMat& dst, Size ksize, double sigmaX, double sigmaY = 0);

/*!
ボックスフィルタを用いて画像を平滑化します．

@param src 入力画像．CV_32FC1,CV_32FC2,CV_32FC3,CV_32FC4型の画像がサポートされます．
@param dst 出力画像． src と同じサイズ，同じチャンネル数になります．
@param ddepth 出力画像のビット深度(無視されます)
@param ksize 平滑化カーネルのサイズ．
*/
void boxFilter(const GlsMat& src, GlsMat& dst, int ddepth, Size ksize);

/*!
拡張 Sobel オペレータを用いて，1次，2次，3次または混合次数の微分画像を求めます．

@param src 入力画像．CV_32FC1,CV_32FC2,CV_32FC3,CV_32FC4型の画像がサポートされます．
@param dst 出力画像． src と同じサイズ，同じチャンネル数になります．
@param ddepth 出力画像のビット深度(無視されます)
@param xorder xに関する微分の次数．
@param yorder yに関する微分の次数．
@param ksize – 拡張Sobelカーネルのサイズ． 1, 3, 5 あるいは 7 のいずれか
*/
void Sobel(const GlsMat& src, GlsMat& dst, int ddepth, int xorder, int yorder, int ksize = 3);

}//namespace gls

#endif