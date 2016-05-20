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

#ifndef _GLS_REDUCE_H_
#define _GLS_REDUCE_H_

namespace gls
{


/*!
行列をベクトルに縮小します．

@param dim 行列が縮小される際に従う次元インデックス．0 は行列が1行 に，1 は行列が1列に縮小されることをそれぞれ意味します．
@param reduceOp 縮小処理，以下のうちの1つ：
 * CV_REDUCE_SUM 出力は，行列の行（または列）の合計．
 * CV_REDUCE_AVG 出力は，行列の行（または列）の平均．
 * CV_REDUCE_MAX 出力は，行列の行（または列）の最大値．
 * CV_REDUCE_MIN 出力は，行列の行（または列）の最小値．
*/
void reduce(const GlsMat& src, GlsMat& dst, int dim, int reduceOp);

}//namespace gls

#endif