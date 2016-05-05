#ifndef _GLS_MERGE_H_
#define _GLS_MERGE_H_

#include "glsCV.h"
#include "glsMat.h"
#include "glsShader.h"



void glsMergeInit(void);
void glsMergeTerminate(void);

void glsMerge(const vector<glsMat>& src, glsMat& dst);


#endif