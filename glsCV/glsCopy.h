#ifndef _GLS_COPY_H_
#define _GLS_COPY_H_

#include "glsCV.h"
#include "glsMat.h"
#include "glsShader.h"



void glsCopyInit(void);
void glsCopyTerminate(void);

void glsCopy(const glsMat& src, glsMat& dst);
void glsCopyTiled(const glsMat& src, glsMat& dst, const Size& blkNum);
void glsCopyUntiled(const glsMat& src, glsMat& dst);
void glsCopyRect(const glsMat& src, glsMat& dst, const Rect& rect, const Size& blkNum = Size(1, 1));



#endif