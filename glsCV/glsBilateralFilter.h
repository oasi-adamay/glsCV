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

#ifndef _GLS_BILATERAL_FILTER_H_
#define _GLS_BILATERAL_FILTER_H_

namespace gls
{

/*!
画像にバイラテラルフィルタを適用します．

@param src 入力画像．CV_32FC1型の画像がサポートされます．
@param dst 出力画像． src と同じサイズ，同じチャンネル数になります．
@param d – フィルタリングで利用される，各ピクセル近傍領域の直径．これが正値でない場合， sigmaSpace から求められます．
@param sigmaColor – 色空間におけるフィルタシグマ．
@param sigmaSpace – 座標空間におけるフィルタシグマ． d>0 ならば sigmaSpace に関係なく近傍領域のサイズが決まります。
*/
void bilateralFilter(const GlsMat& src, GlsMat& dst, int d, double sigmaColor, double sigmaSpace);



}//namespace gls

#endif