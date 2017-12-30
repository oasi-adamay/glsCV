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

#include "stdafx.h"

/*-----------------------------------------------------------------------------
include
*/
#include "glsMacro.h"
#include "GlsMat.h"
#include "glsShader.h"

#include "glsBasicOperation.h"
#include "glsGemm.h"
#include "glsMatOperator.h"


//#ifdef _DEBUG
#if 0
#include "Timer.h"
#define _TMR_(...)  Timer tmr(__VA_ARGS__)
#else
#define _TMR_(...)
#endif


namespace gls
{

//-----------------------------------------------------------------------------
// operator overload
GlsMat operator + (const GlsMat& lhs, const GlsMat& rhs) {
	GlsMat dst;
	gls::add(lhs, rhs, dst);
	return dst;
}

GlsMat operator + (const Scalar& lhs, const GlsMat& rhs) {
	GlsMat dst;
	gls::add(lhs, rhs, dst);
	return dst;
}

GlsMat operator + (const GlsMat& lhs, const Scalar& rhs) {
	GlsMat dst;
	gls::add(lhs, rhs, dst);
	return dst;
}

GlsMat operator - (const Scalar& lhs, const GlsMat& rhs) {
	GlsMat dst;
	gls::subtract(lhs, rhs, dst);
	return dst;
}

GlsMat operator - (const GlsMat& lhs, const Scalar& rhs) {
	GlsMat dst;
	gls::subtract(lhs, rhs, dst);
	return dst;
}

GlsMat operator - (const GlsMat& lhs, const GlsMat& rhs) {
	GlsMat dst;
	gls::subtract(lhs, rhs, dst);
	return dst;
}


GlsMat operator * (const Scalar& lhs, const GlsMat& rhs) {
	GlsMat dst;
	gls::multiply(lhs, rhs, dst);
	return dst;
}

GlsMat operator * (const GlsMat& lhs, const Scalar& rhs) {
	GlsMat dst;
	gls::multiply(lhs, rhs, dst);
	return dst;
}

GlsMat operator * (const GlsMat& lhs, const GlsMat& rhs) {
	GlsMat dst;
	GlsMat null;
	gls::gemm(lhs, rhs, 1.0f,null,0.0f,dst);
	return dst;
}


GlsMat operator / (const Scalar& lhs, const GlsMat& rhs) {
	GlsMat dst;
	gls::divide(lhs, rhs, dst);
	return dst;
}

GlsMat operator / (const GlsMat& lhs, const Scalar& rhs) {
	GlsMat dst;
	gls::divide(lhs, rhs, dst);
	return dst;
}

GlsMat operator / (const GlsMat& lhs, const GlsMat& rhs) {
	GlsMat dst;
	gls::divide(lhs, rhs, dst);
	return dst;
}



}//namespace gls






