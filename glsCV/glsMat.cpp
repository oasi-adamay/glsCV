/*
Copyright (c) 2016, oasi-adamay
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of glsCV nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "stdafx.h"

/*-----------------------------------------------------------------------------
include
*/
#include "glsMacro.h"
#include "GlsMat.h"


//#ifdef _DEBUG
#if 0
#include "Timer.h"
#define _TMR_(...)  Timer tmr(__VA_ARGS__)
#else
#define _TMR_(...)
#endif


namespace gls
{



//#define _USE_PBO_UP
//#define _USE_PBO_DOWN

//#ifdef _DEBUG
#if 0
#define _DBG_GLS_MAT
#endif

#ifdef _DBG_GLS_MAT
static int gTextureNum = 0;
#endif

GlsMat::GlsMat(void){
	flag = 0;
	rows = 0;
	cols = 0;
}

GlsMat::GlsMat(const Size size, const int ocvtype){

	createTexture(size.width, size.height, ocvtype);
}



GlsMat::GlsMat(const Mat & cvmat){
	createTexture(cvmat.cols, cvmat.rows, cvmat.type());
	upload(cvmat);
}



GlsMat& GlsMat::operator=(const GlsMat& rhs){
	if (_texid.use_count() == 1){
		deleteTexture();
	}

	_texid = rhs._texid;
	flag = rhs.flag;
	cols = rhs.cols;
	rows = rhs.rows;

	return *this;
}


//GlsMat::GlsMat(const GlsMat& src, bool copy){
//	createTexture(src.width, src.height, src.internalFormat, src.blkX, src.blkY);
//	GLS_Assert(copy == false);	///@TODO Copy tex
//}




GlsMat::~GlsMat(void){
	if (_texid.use_count() == 1){
		deleteTexture();
	}
}

void GlsMat::createTexture(
	const int _width,				//image width
	const int _height,				//image height
	const int _type					//type(OpenCV)
	){
	_TMR_("createTexture:");

	flag = _type;

	cols = _width;
	rows = _height;

	GLuint tex;

	glGenTextures(1, &tex); // create (reference to) a new texture

	glBindTexture(GL_TEXTURE_2D, tex);
	// (set texture parameters here)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	//create the texture
	glTexImage2D(GL_TEXTURE_2D, 0, glSizedFormat(), cols, rows, 0, glFormat(), glType(), 0);
	GL_CHECK_ERROR();

	glBindTexture(GL_TEXTURE_2D, 0);


	_texid = make_shared<GLuint>(tex);
#ifdef _DBG_GLS_MAT
	{
		std::cout << "create:" << texid() << endl;
		gTextureNum += 1;
		std::cout << "texnum:" << gTextureNum << endl;
	}
#endif
}

void GlsMat::deleteTexture(void){
#ifdef _DBG_GLS_MAT
	{
		std::cout << "delete:" << texid() << endl;
		gTextureNum -= 1;
		std::cout << "texnum:" << gTextureNum << endl;
	}
#endif

	GLuint tex = texid();
	glDeleteTextures(1, &tex);
	flag = 0;
	rows = 0;
	cols = 0;

}



//GLuint GlsMat::at(const int y, const int x)  const{
//	GLS_Assert(y*blkX + x < texArray.size());
//	return texArray[y*blkX + x];
//}

//-----------------------------------------------------------------------------
//OpenCV mat to OpenGL  pbo
static void mat2pbo(const Mat&src, const GLuint pbo){
	int size = (int)src.total() * (int)src.elemSize();

	//bind current pbo for app->pbo transfer
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo); //bind pbo
	GLubyte* ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, size,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	GLS_Assert(ptr != 0);

	int lSize = src.cols * (int)src.elemSize();	// line size in byte
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int y = 0; y < src.rows; y++){
		const uchar* pSrc = src.ptr<uchar>(y);
		uchar* pDst = ptr + lSize * y;
		std::memcpy(pDst, pSrc, lSize);
	}
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
}

//-----------------------------------------------------------------------------
//Upload texture from cv::Mat to GL texture
void GlsMat::upload(const Mat&src){
	_TMR_("-upload :\t");

	CV_Assert(src.type() == type());
	CV_Assert(src.cols == cols);
	CV_Assert(src.rows == rows);


	{
#ifdef _USE_PBO_UP
		int size = cols*rows * (int)src.elemSize();
		GLuint pbo[2];
		glGenBuffers(2, &pbo[0]);
		for (int i = 0; i < 2; i++){
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[i]);
			glBufferData(GL_PIXEL_UNPACK_BUFFER, size, 0, GL_DYNAMIC_READ);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}

		int bank = 0;
		int i = 0;
		{
			int by = i / blkX;
			int bx = i % blkX;
			int x = (bx)* cols;
			int y = (by)* rows;
			Rect rect(x, y, cols, rows);
			Mat roi = Mat(src, rect);	// 1/2  1/2 rect
			mat2pbo(roi, pbo[bank]);
		}

		while(1){
			//bind current pbo for app->pbo transfer
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[bank]); //bind pbo

			//Copy pixels from pbo to texture object
			glBindTexture(GL_TEXTURE_2D, texArray[i]);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[bank]); //bind pbo
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, cols, rows, glFormat(), glType(), 0);
			GL_CHECK_ERROR();

			i++;
			bank = bank ^ 1;
			if (i >= texArray.size()) break;
			{
				int by = i / blkX;
				int bx = i % blkX;
				int x = (bx)* cols;
				int y = (by)* rows;
				Rect rect(x, y, cols, rows);
				Mat roi = Mat(src, rect);	// 1/2  1/2 rect
				mat2pbo(roi, pbo[bank]);
			}

		}
		glDeleteBuffers(2, &pbo[0]);
#else
		{
			Mat _src;
			if (src.isContinuous()){
				_src = src;
			}
			else{
				_src = src.clone();
			}
			CV_Assert(_src.isContinuous());
			void* data = _src.data;

			glBindTexture(GL_TEXTURE_2D, texid());
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, cols, rows, glFormat(), glType(), data);
			GL_CHECK_ERROR();
			glBindTexture(GL_TEXTURE_2D, 0);
		}
#endif
	}

}

//-----------------------------------------------------------------------------
//OpenGL  pbo to OpenCV mat
static void pbo2mat(const GLuint pbo, Mat&dst){
	int size = (int)dst.total() * (int)dst.elemSize();

	//bind current pbo for app->pbo transfer
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo); //bind pbo
	GLubyte* ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, size,
		GL_MAP_READ_BIT);
	GLS_Assert(ptr != 0);

	int lSize = dst.cols * (int)dst.elemSize();	// line size in byte
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int y = 0; y < dst.rows; y++){
		uchar* pSrc = ptr + lSize * y;
		uchar* pDst = dst.ptr<uchar>(y);
		memcpy(pDst, pSrc, lSize);
	}
	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

}


void GlsMat::download(Mat&dst) const{
//	cout << "download:start" << endl;

	_TMR_("-download:\t");

//	dst = Mat(size(), type());

	// texture

	{	//download from texture
#ifdef _USE_PBO_DOWN

		int size = cols*rows * (int)dst.elemSize();
		GLuint pbo[2];
		glGenBuffers(2, &pbo[0]);
		for (int i = 0; i < 2; i++){
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[i]);
			glBufferData(GL_PIXEL_UNPACK_BUFFER, size, 0, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}
		int bank = 0;
		int i = 0;

		{
			//Copy pixels from texture object to pbo_bank
			glBindTexture(GL_TEXTURE_2D, texArray[i]);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[bank]); //bind pbo
			glGetTexImage(GL_TEXTURE_2D, 0, glFormat(), glType(), 0);
		}

		while (1){
			if (i + 1 < texArray.size()){
				//Copy pixels from texture object to pbo_bank
				glBindTexture(GL_TEXTURE_2D, texArray[i + 1]);
				glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[bank ^ 1]); //bind pbo
				glGetTexImage(GL_TEXTURE_2D, 0, glFormat(), glType(), 0);
			}

			int by = i / blkX;
			int bx = i % blkX;
			int x = (bx)* cols;
			int y = (by)* rows;
			Rect rect(x, y, cols, rows);
			Mat roi = Mat(dst, rect);	// 1/2  1/2 rect
			pbo2mat(pbo[bank], roi);
			i++;
			bank = bank ^ 1;

			if (i >= texArray.size()) break;
		}
		glDeleteBuffers(2, &pbo[0]);
#else
		Mat _dst;
		bool replace = dst.isContinuous() && (dst.size() == size()) && (dst.type() == type());
		if (replace){
			_dst = dst;
		}
		else{
			_dst = Mat(size(), type());
		}
		{
			void* data = _dst.data;
			glBindTexture(GL_TEXTURE_2D, texid());
			glGetTexImage(GL_TEXTURE_2D, 0, glFormat(), glType(), data);
			GL_CHECK_ERROR();
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		if (replace){
			dst = _dst;
		}
		else{
			_dst.copyTo(dst);
		}

#endif
	}

//	cout << "download:end" << endl;
}


GlsMat::operator Mat() const{
	Mat mat;
	(*this).download(mat);
	return mat;
}

void GlsMat::setInterpolation(GLint interpolation) const{
	glBindTexture(GL_TEXTURE_2D, texid());
	// (set texture parameters here)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

}





GLenum convCVtype2GLsizedFormat(int ocvtype){
	GLenum format = 0;
	switch (ocvtype){
	case(CV_32FC1) : format = GL_R32F; break;
	case(CV_32FC2) : format = GL_RG32F; break;
	case(CV_32FC3) : format = GL_RGB32F; break;
	case(CV_32FC4) : format = GL_RGBA32F; break;

	case(CV_8UC1) : format = GL_R8UI; break;
	case(CV_8UC2) : format = GL_RG8UI; break;
	case(CV_8UC3) : format = GL_RGB8UI; break;
	case(CV_8UC4) : format = GL_RGBA8UI; break;

	case(CV_8SC1) : format = GL_R8I; break;
	case(CV_8SC2) : format = GL_RG8I; break;
	case(CV_8SC3) : format = GL_RGB8I; break;
	case(CV_8SC4) : format = GL_RGBA8I; break;

	case(CV_16UC1) : format = GL_R16UI; break;
	case(CV_16UC2) : format = GL_RG16UI; break;
	case(CV_16UC3) : format = GL_RGB16UI; break;
	case(CV_16UC4) : format = GL_RGBA16UI; break;

	case(CV_16SC1) : format = GL_R16I; break;
	case(CV_16SC2) : format = GL_RG16I; break;
	case(CV_16SC3) : format = GL_RGB16I; break;
	case(CV_16SC4) : format = GL_RGBA16I; break;

	case(CV_32SC1) : format = GL_R32I; break;
	case(CV_32SC2) : format = GL_RG32I; break;
	case(CV_32SC3) : format = GL_RGB32I; break;
	case(CV_32SC4) : format = GL_RGBA32I; break;

	}

	return format;
}


int convFmtGL2CV(GLenum  format){
	int ocvtype = 0;
	switch (format){
	case(GL_R16F) : ocvtype = CV_32FC1; break;
	case(GL_R32F) : ocvtype = CV_32FC1; break;
	case(GL_RG16F) :ocvtype = CV_32FC2; break;
	case(GL_RG32F) : ocvtype = CV_32FC2; break;
	case(GL_RGB16F) : ocvtype = CV_32FC3; break;
	case(GL_RGB32F) : ocvtype = CV_32FC3; break;
	case(GL_RGBA16F) : ocvtype = CV_32FC4; break;
	case(GL_RGBA32F) : ocvtype = CV_32FC4; break;
	case(GL_R8UI) : ocvtype = CV_8UC1; break;
	case(GL_RG8UI) : ocvtype = CV_8UC2; break;
	case(GL_RGB8UI) : ocvtype = CV_8UC3; break;
	case(GL_RGBA8UI) : ocvtype = CV_8UC4; break;

	case(GL_R8I) : ocvtype = CV_8SC1; break;
	case(GL_RG8I) : ocvtype = CV_8UC2; break;
	case(GL_RGB8I) : ocvtype = CV_8UC3; break;
	case(GL_RGBA8I) : ocvtype = CV_8UC4; break;

	case(GL_R16UI) : ocvtype = CV_16UC1; break;
	case(GL_RG16UI) : ocvtype = CV_16UC2; break;
	case(GL_RGB16UI) : ocvtype = CV_16UC3; break;
	case(GL_RGBA16UI) : ocvtype = CV_16UC4; break;

	case(GL_R16I) : ocvtype = CV_16SC1; break;
	case(GL_RG16I) : ocvtype = CV_16SC2; break;
	case(GL_RGB16I) : ocvtype = CV_16SC3; break;
	case(GL_RGBA16I) : ocvtype = CV_16SC4; break;

	case(GL_R32UI) : ocvtype = CV_32SC1; break;
	case(GL_RG32UI) : ocvtype = CV_32SC2; break;
	case(GL_RGB32UI) : ocvtype = CV_32SC3; break;
	case(GL_RGBA32UI) : ocvtype = CV_32SC4; break;

	case(GL_R32I) : ocvtype = CV_32SC1; break;
	case(GL_RG32I) : ocvtype = CV_32SC2; break;
	case(GL_RGB32I) : ocvtype = CV_32SC3; break;
	case(GL_RGBA32I) : ocvtype = CV_32SC4; break;

	}
	return ocvtype;
}



GLenum convCVtype2GLformat(int ocvtype){
	GLenum format = 0;
	int ch = CV_MAT_CN(ocvtype);
	int depth = CV_MAT_DEPTH(ocvtype);
	bool isFloat = depth == (CV_32F);
	if (isFloat){
		switch (ch){
			case(1) : format = GL_RED; break;
			case(2) : format = GL_RG; break;
			case(3) : format = GL_RGB; break;
			case(4) : format = GL_RGBA; break;
			default: GLS_Assert(0);
		}
	}
	else{
		switch (ch){
			case(1) : format = GL_RED_INTEGER; break;
			case(2) : format = GL_RG_INTEGER; break;
			case(3) : format = GL_RGB_INTEGER; break;
			case(4) : format = GL_RGBA_INTEGER; break;
			default: GLS_Assert(0);
		}
	}
	return format;
}


GLenum convCVtype2GLtype(int ocvtype){
	GLenum type = 0;
	int depth = CV_MAT_DEPTH(ocvtype);
	switch (depth){
	case(CV_8U) : type = GL_UNSIGNED_BYTE; break;
	case(CV_8S) : type = GL_BYTE; break;
	case(CV_16U) : type = GL_UNSIGNED_SHORT; break;
	case(CV_16S) : type = GL_SHORT; break;
//	case(CV_32U) : type = GL_UNSIGNED_INT; break;
	case(CV_32S) : type = GL_INT; break;
	case(CV_32F) : type = GL_FLOAT; break;
	default: GLS_Assert(0);
	}
	return type;
}

}//namespace gls






