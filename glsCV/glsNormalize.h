#ifndef _GLS_NORMALIZE_H_
#define _GLS_NORMALIZE_H_

#include "glsCV.h"
#include "glsMat.h"
#include "glsShader.h"



void glsNormalizeInit(void);
void glsNormalizeTerminate(void);



void glsNormalize(const glsMat& src, glsMat& dst, double alpha = 0, double beta = 1, int normType = NORM_MINMAX);




#endif