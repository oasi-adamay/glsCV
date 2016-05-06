#ifndef _GLS_REDUCE_H_
#define _GLS_REDUCE_H_

#include "glsCV.h"
#include "glsMat.h"
#include "glsShader.h"



void glsReduceInit(void);
void glsReduceTerminate(void);



void glsReduce(const glsMat& src, glsMat& dst, int dim, int reduceOp);

void glsMinMaxLoc(const glsMat& src, double* minVal, double* maxVal = 0, Point* minLoc = 0, Point* maxLoc = 0, const glsMat& mask = glsMat());



#endif