#ifndef _GLS_CV_H_
#define _GLS_CV_H_


#include "glsMat.h"
#include "glsShader.h"
#include "glsCopy.h"
#include "glsConvert.h"
#include "glsDraw.h"
#include "glsMerge.h"
#include "glsFft.h"
#include "glsBasicOperation.h"

const char* glsErrorString(GLenum err);

//macro 
#define GLS_Assert(exp)	 CV_Assert(exp)  
//#define GLS_Assert(exp)	 assert(exp)  

#ifdef _DEBUG
#define GL_CHECK_ERROR()  { int err = glGetError(); if(err!= GL_NO_ERROR){cerr << "[ERR]" << glsErrorString(err) <<" at " <<__FILE__ <<":" << __LINE__ << endl;}; GLS_Assert(err == GL_NO_ERROR); }
#else
#define GL_CHECK_ERROR()  GLS_Assert(glGetError() == GL_NO_ERROR)
#endif




GLFWwindow* glsCvInit(const int _width = 0, const int _height = 0);
void glsCvTerminate(void);


#endif