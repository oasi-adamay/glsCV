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

#ifndef _GLS_FFT_H_
#define _GLS_FFT_H_

#include "glsCV.h"
#include "GlsMat.h"
#include "glsShader.h"


namespace gls
{

#define GLS_FFT_SCALE	   (1<<1)
#define GLS_FFT_INVERSE   (1<<2)
#define GLS_FFT_SHIFT	   (1<<3)	

void ShaderFftInit(void);
void ShaderFftTerminate(void);

/*!
���������_�^�̍s��ɑ΂��āC2�����̃t�[���G���ϊ��܂��͋t�ϊ����s���܂��D
�C���^���[�u�t�H�[�}�b�g�̕��f�s��i CV32FC2�j���������Ƃ��ł��܂��D

*/
void fft(const GlsMat& src, GlsMat& dst, int flag = 0);



void fft(const Mat& src, Mat& dst, int flag = 0);

}//namespace gls


#endif