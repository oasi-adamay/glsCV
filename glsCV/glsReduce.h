#ifndef _GLS_REDUCE_H_
#define _GLS_REDUCE_H_

#include "glsCV.h"
#include "glsMat.h"
#include "glsShader.h"



void glsReduceInit(void);
void glsReduceTerminate(void);



void glsReduce(const glsMat& src, glsMat& dst, int dim, int reduceOp);


#endif