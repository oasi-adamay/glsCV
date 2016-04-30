#ifndef _GLS_CV_H_
#define _GLS_CV_H_

#include "glsMat.h"
#include "glsShader.h"
#include "glsConvert.h"
#include "glsDraw.h"
#include "glsFft.h"
#include "glsBasicOperation.h"


GLFWwindow* glsCvInit(const int _width = 0, const int _height = 0);
void glsCvTerminate(void);


#endif