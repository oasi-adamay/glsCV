#ifndef _GLS_BASIC_OPERATION_H_
#define _GLS_BASIC_OPERATION_H_

#include "glsShader.h"
#include "glsMat.h"






void glsBasicOperationInit(void);
void glsBasicOperationTerminate(void);

//glsMat I/F
void glsAdd(const vec4& scalar, const glsMat& src, glsMat& dst);
void glsAdd(const glsMat& src0, const glsMat& src1, glsMat& dst);

void glsSubtract(const vec4& scalar, const glsMat& src, glsMat& dst);
void glsSubtract(const glsMat& src0, const glsMat& src1, glsMat& dst);

void glsMultiply(const vec4& scalar, const glsMat& src, glsMat& dst);
void glsMultiply(const glsMat& src0, const glsMat& src1, glsMat& dst);

void glsDivide(const vec4& scalar, const glsMat& src, glsMat& dst);
void glsDivide(const glsMat& src0, const glsMat& src1, glsMat& dst);

void glsMin(const vec4& scalar, const glsMat& src, glsMat& dst);
void glsMin(const glsMat& src0, const glsMat& src1, glsMat& dst);

void glsMax(const vec4& scalar, const glsMat& src, glsMat& dst);
void glsMax(const glsMat& src0, const glsMat& src1, glsMat& dst);

void glsMulSpectrums(const glsMat& src0, const glsMat& src1, glsMat& dst, bool conj = false);
void glsMulSpectrumsPhaseOnly(const glsMat& src0, const glsMat& src1, glsMat& dst);
void glsMagSpectrums(const glsMat& src, glsMat& dst);

void glsLog(const glsMat& src, glsMat& dst);
void glsExp(const glsMat& src, glsMat& dst);
void glsPow(const glsMat& src, const float& power, glsMat& dst);



//OpenCV I/F
void glsAdd(const Vec4f& scalar, const Mat& src, Mat& dst);
void glsAdd(const Mat& src0, const Mat& src1, Mat& dst);

void glsSubtract(const Vec4f& scalar, const Mat& src, Mat& dst);
void glsSubtract(const Mat& src0, const Mat& src1, Mat& dst);

void glsMultiply(const Vec4f& scalar, const Mat& src, Mat& dst);
void glsMultiply(const Mat& src0, const Mat& src1, Mat& dst);

void glsDivide(const Vec4f& scalar, const Mat& src, Mat& dst);
void glsDivide(const Mat& src0, const Mat& src1, Mat& dst);

void glsMin(const Vec4f& scalar, const Mat& src, Mat& dst);
void glsMin(const Mat& src0, const Mat& src1, Mat& dst);

void glsMax(const Vec4f& scalar, const Mat& src, Mat& dst);
void glsMax(const Mat& src0, const Mat& src1, Mat& dst);

void glsMulSpectrums(const Mat& src0, const Mat& src1, Mat& dst, bool conj = false);
void glsMulSpectrumsPhaseOnly(const Mat& src0, const Mat& src1, Mat& dst);
void glsMagSpectrums(const Mat& src, Mat& dst);

void glsLog(const Mat& src, Mat& dst);
void glsExp(const Mat& src, Mat& dst);
void glsPow(const Mat& src, const float& power, Mat& dst);

	


#endif