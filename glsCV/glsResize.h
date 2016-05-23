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

#ifndef _GLS_RESIZE_H_
#define _GLS_RESIZE_H_

namespace gls
{

/*!
画像のサイズを変更します．

@param src – 入力画像 (CV_32FC1,CV_32FC2,CV_32FC3,CV_32FC4)
@param dst – 出力画像．サイズは dsize （0でない場合）か，または src.size() , fx , fy から計算される値になります． dst の型は， src と同じになります
@param dsize – 出力画像サイズ．これが0の場合，次のように計算されます：
dsize = Size(round(fx*src.cols),round(fy*src.rows))
必ず dsize が非0，あるいは fx と fy の両方が非0，でなければいけません
@param fx – 水平軸方向のスケールファクタ．これが0の場合，次のように計算されます
(double) dsize.width / src.cols;
@param fy – 垂直軸方向のスケールファクタ．これが0の場合，次のように計算されます：
(double) dsize.height / src.rows;
@param 補間手法：
- INTER_NEAREST 最近傍補間
- INTER_LINEAR バイリニア補間（デフォルト）
(- INTER_AREA ピクセル領域の関係を利用したリサンプリング)
(- INTER_CUBIC 4x4 の近傍領域を利用するバイキュービック補間)
*/
void resize(const GlsMat& src, GlsMat& dst, Size dsize, double fx = 0, double fy = 0, int interpolation = INTER_LINEAR);


}//namespace gls

#endif