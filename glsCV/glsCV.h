#ifndef _GLS_CV_H_
#define _GLS_CV_H_


#include "glsMat.h"
#include "glsShader.h"
#include "glsCopy.h"
#include "glsConvert.h"
#include "glsDraw.h"
#include "glsFft.h"
#include "glsBasicOperation.h"

//macro 
#define GLS_Assert(exp)	 CV_Assert(exp)  

#define GL_CHECK_ERROR()  GLS_Assert(glGetError() == GL_NO_ERROR)



const char* glsErrorString(GLenum err);


GLFWwindow* glsCvInit(const int _width = 0, const int _height = 0);
void glsCvTerminate(void);


#endif