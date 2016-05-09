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


#include "glsNormalize.h"

namespace gls
{



#if 0
//-----------------------------------------------------------------------------
// glsShaderNormalize
class glsShaderNormalize : public glsShaderBase
{
public:
	glsShaderNormalize(void){ ; }
};

//-----------------------------------------------------------------------------
// glsShaderNormalizeU unsigned
class glsShaderNormalizeU : public glsShaderBase
{
public:
	glsShaderNormalizeU(void);
};

//-----------------------------------------------------------------------------
// glsShaderNormalizeS unsigned
class glsShaderNormalizeS : public glsShaderBase
{
public:
	glsShaderNormalizeS(void);
};
#endif

//-----------------------------------------------------------------------------
//global 
//glsShaderNormalize* shaderNormalize = 0;
//glsShaderNormalizeU* shaderNormalizeU = 0;
//glsShaderNormalizeS* shaderNormalizeS = 0;

void normalizeInit(void){
//	shaderNormalize = new glsShaderNormalize();
//	shaderNormalizeU = new glsShaderNormalizeU();
//	shaderNormalizeS = new glsShaderNormalizeS();
}

void normalizeTerminate(void){
//	delete shaderNormalize;
//	delete shaderNormalizeU;
//	delete shaderNormalizeS;
}


//enum { NORM_INF = 1, NORM_L1 = 2, NORM_L2 = 4, NORM_L2SQR = 5, NORM_HAMMING = 6, NORM_HAMMING2 = 7, NORM_TYPE_MASK = 7, NORM_RELATIVE = 8, NORM_MINMAX = 32 };

void normalize(const GlsMat& src, GlsMat& dst, double alpha, double beta, int normType){
	GLS_Assert(src.type() == CV_32FC1);
	GLS_Assert(normType == NORM_MINMAX);

	GlsMat _dst;

	double minVal;
	double maxVal;

	gls::minMaxLoc(src, &minVal, &maxVal);
	float scl = (float)(beta - alpha) / (float)(maxVal - minVal);
	gls::multiply(vec4(scl), src, _dst);
	gls::add(vec4((float)(alpha - scl*minVal)), _dst, _dst);
	dst = _dst;
}


}//namespace gls

