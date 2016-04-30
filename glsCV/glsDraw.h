#ifndef _GLS_DRAW_H_
#define _GLS_DRAW_H_

#include "glsCV.h"
#include "glsMat.h"
#include "glsShader.h"



void glsDrawInit(void);
void glsDrawTerminate(void);

void glsDraw(glsMat& src);



#endif