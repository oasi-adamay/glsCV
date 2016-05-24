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

#ifndef _GLS_REMAP_H_
#define _GLS_REMAP_H_

namespace gls
{

/*!
画像に対して，汎用的な幾何学変換を適用します．

@param src – 入力画像 (CV_32FC1,CV_32FC2,CV_32FC3,CV_32FC4)
@param dst – 出力画像． map1 と同じサイズ， src と同じタイプ
@param map1 – CV_32FC2 である座標点 (x,y)．
@param map2 – 空のマップ
@param interpolation 補間手法：
- INTER_NEAREST 最近傍補間
- INTER_LINEAR バイリニア補間（デフォルト）
(- INTER_AREA ピクセル領域の関係を利用したリサンプリング)
(- INTER_CUBIC 4x4 の近傍領域を利用するバイキュービック補間)

dst(x,y) = src(mapx(x,y),mapy(x,y))
*/

void remap(const GlsMat& src, GlsMat& dst, const GlsMat& map1, const GlsMat& map2, int interpolation);



}//namespace gls

#endif