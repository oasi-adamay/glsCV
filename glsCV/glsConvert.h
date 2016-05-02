#ifndef _GLS_CONVERT_H_
#define _GLS_CONVERT_H_

#include "glsCV.h"
#include "glsMat.h"
#include "glsShader.h"



void glsConvertInit(void);
void glsConvertTerminate(void);

void glsConvert(const glsMat& src, glsMat& dst,const float scl = 1.0);
void glsCvtColor(const glsMat& src, glsMat& dst, const int code);



#endif