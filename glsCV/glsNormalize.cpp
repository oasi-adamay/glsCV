#include "stdafx.h"


#include "glsNormalize.h"

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

void glsNormalizeInit(void){
//	shaderNormalize = new glsShaderNormalize();
//	shaderNormalizeU = new glsShaderNormalizeU();
//	shaderNormalizeS = new glsShaderNormalizeS();
}

void glsNormalizeTerminate(void){
//	delete shaderNormalize;
//	delete shaderNormalizeU;
//	delete shaderNormalizeS;
}


//enum { NORM_INF = 1, NORM_L1 = 2, NORM_L2 = 4, NORM_L2SQR = 5, NORM_HAMMING = 6, NORM_HAMMING2 = 7, NORM_TYPE_MASK = 7, NORM_RELATIVE = 8, NORM_MINMAX = 32 };

void glsNormalize(const glsMat& src, glsMat& dst, double alpha, double beta, int normType){
	GLS_Assert(src.type() == CV_32FC1);
	GLS_Assert(normType == NORM_MINMAX);

	glsMat _dst;

	double minVal;
	double maxVal;

	glsMinMaxLoc(src, &minVal, &maxVal);
	float scl = (float)(beta - alpha) / (float)(maxVal - minVal);
	glsMultiply(vec4(scl), src, _dst);
	glsAdd(vec4((float)(alpha - scl*minVal)), _dst, _dst);
	dst = _dst;
}




