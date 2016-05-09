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

//! �X�J���Ɣz��� �v�f���̘a�����߂܂��D(dst = scalar + src1)
void add(const vec4& scalar, const GlsMat& src, GlsMat& dst);

//! 2 �̔z�񓯎m�� �v�f���̘a�����߂܂�(dst = src0 + src1)
void add(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

//! �X�J���Ɣz��� �v�f���̍������߂܂��D(dst = scalar - src1)
void subtract(const vec4& scalar, const GlsMat& src, GlsMat& dst);

//! 2 �̔z�񓯎m�� �v�f���̍������߂܂�(dst = src0 - src1)
void subtract(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

//! �X�J���Ɣz��� �v�f���̐ς����߂܂��D(dst = scalar * src1)
void multiply(const vec4& scalar, const GlsMat& src, GlsMat& dst);

//! 2 �̔z�񓯎m�� �v�f���̐ς����߂܂�(dst = src0 * src1)
void multiply(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

//! �X�J���Ɣz��� �v�f���̏������߂܂��D(dst = scalar / src1)
void divide(const vec4& scalar, const GlsMat& src, GlsMat& dst);

//! 2 �̔z�񓯎m��  �v�f���̏������߂܂��D(dst = src0 / src1)
void divide(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

//! �X�J���Ɣz��� �v�f���̍ŏ��l�����߂܂��D(dst = min(scalar, src))
void min(const vec4& scalar, const GlsMat& src, GlsMat& dst);

//! 2 �̔z�񓯎m��  �v�f���̍ŏ��l�����߂܂��D(dst = min(src0, src1))
void min(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

//! �X�J���Ɣz��� �v�f���̍ő�l�����߂܂��D(dst = max(scalar, src))
void max(const vec4& scalar, const GlsMat& src, GlsMat& dst);

//! 2 �̔z�񓯎m��  �v�f���̍ő�l�����߂܂��D(dst = max(src0, src1))
void max(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

//! 2 �̃t�[���G�X�y�N�g�����m�̗v�f���̏�Z���s���܂��D
//! conj = ture �̏ꍇ�A2 �Ԗڂ̔z��𕡑f�����ɕύX�ł��܂��D
void mulSpectrums(const GlsMat& src0, const GlsMat& src1, GlsMat& dst, bool conj = false);

//! 2 �̃t�[���G�X�y�N�g�����m��  �v�f���̈ʑ����葊�ւ����߂܂��D
void mulSpectrumsPhaseOnly(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

//! ���f�s��v�f�̑傫�������߂܂��D(dst = sqrt(src.re**2 + src.im**2 )
void magSpectrums(const GlsMat& src, GlsMat& dst);

//! ���f�s��v�f�̐�Βl��2���offset���Z���A��Βl�̎��R�ΐ������߂܂��D
//! (dst = log( sqrt(src.re**2 + src.im**2) + offset )
void logMagSpectrums(const GlsMat& src, GlsMat& dst,float offset);

//! �e�z��v�f�̐�Βl�̎��R�ΐ������߂܂��D(dst = log(src))
void log(const GlsMat& src, GlsMat& dst);

//! �e�z��v�f���w���Ƃ��āC���R�ΐ��̒�i�l�C�s�A���je �ׂ̂�������߂܂��D(dst = exp(src))
void exp(const GlsMat& src, GlsMat& dst);

//! �e�z��v�f��ݏ悵�܂��D (dst = src**power)
void pow(const GlsMat& src, const float& power, GlsMat& dst);


	

}//namespace gls

#endif