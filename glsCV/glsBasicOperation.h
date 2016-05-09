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
#include "glsMat.h"






void glsBasicOperationInit(void);
void glsBasicOperationTerminate(void);

//glsMat I/F
void glsAdd(const vec4& scalar, const glsMat& src, glsMat& dst);
void glsAdd(const glsMat& src0, const glsMat& src1, glsMat& dst);

void glsSubtract(const vec4& scalar, const glsMat& src, glsMat& dst);
void glsSubtract(const glsMat& src0, const glsMat& src1, glsMat& dst);

void glsMultiply(const vec4& scalar, const glsMat& src, glsMat& dst);
void glsMultiply(const glsMat& src0, const glsMat& src1, glsMat& dst);

void glsDivide(const vec4& scalar, const glsMat& src, glsMat& dst);
void glsDivide(const glsMat& src0, const glsMat& src1, glsMat& dst);

void glsMin(const vec4& scalar, const glsMat& src, glsMat& dst);
void glsMin(const glsMat& src0, const glsMat& src1, glsMat& dst);

void glsMax(const vec4& scalar, const glsMat& src, glsMat& dst);
void glsMax(const glsMat& src0, const glsMat& src1, glsMat& dst);

void glsMulSpectrums(const glsMat& src0, const glsMat& src1, glsMat& dst, bool conj = false);
void glsMulSpectrumsPhaseOnly(const glsMat& src0, const glsMat& src1, glsMat& dst);
void glsMagSpectrums(const glsMat& src, glsMat& dst);
void glsLogMagSpectrums(const glsMat& src, glsMat& dst,float offset);

void glsLog(const glsMat& src, glsMat& dst);
void glsExp(const glsMat& src, glsMat& dst);
void glsPow(const glsMat& src, const float& power, glsMat& dst);


#if 0
//OpenCV I/F
void glsAdd(const Vec4f& scalar, const Mat& src, Mat& dst);
void glsAdd(const Mat& src0, const Mat& src1, Mat& dst);

void glsSubtract(const Vec4f& scalar, const Mat& src, Mat& dst);
void glsSubtract(const Mat& src0, const Mat& src1, Mat& dst);

void glsMultiply(const Vec4f& scalar, const Mat& src, Mat& dst);
void glsMultiply(const Mat& src0, const Mat& src1, Mat& dst);

void glsDivide(const Vec4f& scalar, const Mat& src, Mat& dst);
void glsDivide(const Mat& src0, const Mat& src1, Mat& dst);

void glsMin(const Vec4f& scalar, const Mat& src, Mat& dst);
void glsMin(const Mat& src0, const Mat& src1, Mat& dst);

void glsMax(const Vec4f& scalar, const Mat& src, Mat& dst);
void glsMax(const Mat& src0, const Mat& src1, Mat& dst);

void glsMulSpectrums(const Mat& src0, const Mat& src1, Mat& dst, bool conj = false);
void glsMulSpectrumsPhaseOnly(const Mat& src0, const Mat& src1, Mat& dst);
void glsMagSpectrums(const Mat& src, Mat& dst);

void glsLog(const Mat& src, Mat& dst);
void glsExp(const Mat& src, Mat& dst);
void glsPow(const Mat& src, const float& power, Mat& dst);
#endif
	


#endif