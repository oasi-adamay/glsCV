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
void add(const vec4& scalar, const GlsMat& src, GlsMat& dst);
void add(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

void subtract(const vec4& scalar, const GlsMat& src, GlsMat& dst);
void subtract(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

void multiply(const vec4& scalar, const GlsMat& src, GlsMat& dst);
void multiply(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

void divide(const vec4& scalar, const GlsMat& src, GlsMat& dst);
void divide(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

void min(const vec4& scalar, const GlsMat& src, GlsMat& dst);
void min(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

void max(const vec4& scalar, const GlsMat& src, GlsMat& dst);
void max(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);

void mulSpectrums(const GlsMat& src0, const GlsMat& src1, GlsMat& dst, bool conj = false);
void mulSpectrumsPhaseOnly(const GlsMat& src0, const GlsMat& src1, GlsMat& dst);
void magSpectrums(const GlsMat& src, GlsMat& dst);
void logMagSpectrums(const GlsMat& src, GlsMat& dst,float offset);

void log(const GlsMat& src, GlsMat& dst);
void exp(const GlsMat& src, GlsMat& dst);
void pow(const GlsMat& src, const float& power, GlsMat& dst);


	

}//namespace gls

#endif