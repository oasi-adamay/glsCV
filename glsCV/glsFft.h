#ifndef _GLS_FFT_H_
#define _GLS_FFT_H_

#include "glsCV.h"
#include "glsMat.h"
#include "glsShader.h"




#define GLS_FFT_SCALE	   (1<<1)
#define GLS_FFT_INVERSE   (1<<2)

void glsFftInit(void);
void glsFftTerminate(void);

void glsFft(const glsMat& src, glsMat& dst, int flag = 0);
void glsFft(const Mat& src, Mat& dst, int flag = 0);



#endif