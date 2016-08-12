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
#include "glsCopy.h"


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

GlsMat::GlsMat(void) : size(&rows){
	init();
}

GlsMat::GlsMat(const GlsMat &obj) : size(&rows){
	init();
	*this = obj;
}


GlsMat::GlsMat(const Size size, const int ocvtype) : size(&rows){
	init();
	createTexture(size.width, size.height, ocvtype);
}

GlsMat::GlsMat(const int _dims, const int* sizes, const int ocvtype) : size(&rows){
	GLS_Assert(_dims == 2 || _dims == 3);
	init();
	createTexture(_dims, sizes, ocvtype);
}


GlsMat::GlsMat(const Mat & cvmat) : size(&rows){
	GLS_Assert(cvmat.dims == 2 || cvmat.dims == 3);
	init();
	createTexture(cvmat.dims, cvmat.size, cvmat.type());
	GlsMat::GlsMat(cvmat.dims, cvmat.size, cvmat.type());
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
	plns = rhs.plns;
	dims = rhs.dims;
	if (rhs.size.p == &rhs.rows) size.p = &rows;
	else if (rhs.size.p == &rhs.plns) size.p = &plns;
	else GLS_Assert(0 && "unreachable.");

	return *this;
}



GlsMat::~GlsMat(void){
	if (_texid.use_count() == 1){
		deleteTexture();
	}
}

void GlsMat::createTexture(const int _dims, const int* _sizes, int _type){
	GLS_Assert(_dims == 2 || _dims == 3);
	if (_dims == 2){
		createTexture(_sizes[1], _sizes[0], _type);
	}
	else if (_dims == 3){
		createTexture(_sizes[2], _sizes[1], _sizes[0], _type);
	}
	else{
		GLS_Assert(0 && "not support.");
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
	plns = 1;
	size.p = &rows;
	dims = 2;


	GLuint tex = 0;

	glGenTextures(1, &tex); // create (reference to) a new texture
	GLS_Assert(tex != 0);

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
	init();
}


void GlsMat::createTexture(const int _width, const int _height, const int _depth, int _type){
	_TMR_("createTexture:");

	flag = _type;
	cols = _width;
	rows = _height;
	plns = _depth;
	size.p = &plns;
	dims = 3;

	GLuint tex = 0;

	glGenTextures(1, &tex); // create (reference to) a new texture
	GLS_Assert(tex != 0);

	glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
	// (set texture parameters here)
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	//create the texture
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, glSizedFormat(), _width, _height, _depth, 0, glFormat(), glType(), 0);
	GL_CHECK_ERROR();

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);


	_texid = make_shared<GLuint>(tex);
#ifdef _DBG_GLS_MAT
	{
		std::cout << "create:" << texid() << endl;
		gTextureNum += 1;
		std::cout << "texnum:" << gTextureNum << endl;
	}
#endif

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

#if 0
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < src.size[0]; i++){
		size_t _step = src.step[0];
		uchar* pSrc = src.data + _step * i;
		uchar* pDst = ptr + _step * i;
		std::memcpy(pDst, pSrc, _step);
	}
#else
	uchar* pSrc = src.data;
	uchar* pDst = ptr;
#pragma warning(suppress: 28183)
	std::memcpy(pDst, pSrc, size);

#endif
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
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

#if 1
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < dst.size[0]; i++){
		size_t _step = dst.step[0];
		uchar* pSrc = ptr + _step * i;
		uchar* pDst = dst.data + _step *i;
#pragma warning(suppress: 28183)
		std::memcpy(pDst, pSrc, _step);
	}
#else
	uchar* pSrc = ptr;
	uchar* pDst = dst.data;
	std::memcpy(pDst, pSrc, size);

#endif
	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

}

#ifdef _USE_PBO_UP
//-----------------------------------------------------------------------------
//Upload texture from cv::Mat to GL texture
void GlsMat::upload(const Mat&src){
	_TMR_("-upload :\t");

	CV_Assert(src.type() == type());
	CV_Assert(src.dims == dims);
	if (src.dims == 2){
		CV_Assert(src.size() == size());
	}
	else{
		CV_Assert(src.size[0] == size[0]);
		CV_Assert(src.size[1] == size[1]);
		CV_Assert(src.size[2] == size[2]);
	}

	Mat _src;
	if (src.isContinuous()){
		_src = src;
	}
	else{
		_src = src.clone();
	}
	CV_Assert(_src.isContinuous());

	{
		int _size = (int)total() * (int)_src.elemSize();
		GLuint pbo;

		glGenBuffers(1, &pbo);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, _size, 0, GL_DYNAMIC_READ);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		mat2pbo(_src, pbo);

		//bind current pbo for app->pbo transfer
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo); //bind pbo

		//Copy pixels from pbo to texture object
		if (src.dims == 2){
			glBindTexture(GL_TEXTURE_2D, texid());
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo); //bind pbo
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, cols, rows, glFormat(), glType(), 0);
			GL_CHECK_ERROR();
		}
		else if (src.dims == 3){
			glBindTexture(GL_TEXTURE_2D_ARRAY, texid());
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, size[2], size[1], size[0], glFormat(), glType(), 0);
			GL_CHECK_ERROR();
			glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		}
		else {
			GLS_Assert(0 && "dims must be 2 or 3.");
		}

		glDeleteBuffers(1, &pbo);
	}
}
#else
//-----------------------------------------------------------------------------
//Upload texture from cv::Mat to GL texture
void GlsMat::upload(const Mat&src){
	_TMR_("-upload :\t");

	CV_Assert(src.type() == type());
	CV_Assert(src.dims == dims);
	if (src.dims == 2){
		CV_Assert(src.size() == size());
	}
	else{
		CV_Assert(src.size[0] == size[0]);
		CV_Assert(src.size[1] == size[1]);
		CV_Assert(src.size[2] == size[2]);
	}


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

		if (src.dims == 2){
			glBindTexture(GL_TEXTURE_2D, texid());
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, cols, rows, glFormat(), glType(), data);
			GL_CHECK_ERROR();
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else if (src.dims == 3){
			glBindTexture(GL_TEXTURE_2D_ARRAY, texid());
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, size[2], size[1], size[0], glFormat(), glType(), data);
			GL_CHECK_ERROR();
			glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		}
		else {
			GLS_Assert(0 && "dims must be 2 or 3.");
		}
	}

}


#endif

#ifdef _USE_PBO_DOWN 
void GlsMat::download(Mat&dst) const{
	//	cout << "download:start" << endl;

	_TMR_("-download:\t");

	Mat _dst;
	bool isSameSize = true;
	isSameSize &= (dst.dims == dims);
	if (isSameSize){
		for (int i = 0; i < dims; i++){
			isSameSize &= (dst.size[i] == size[i]);
		}
	}
	bool isSameType = (dst.type() == type());

	bool replace = dst.isContinuous() && isSameSize && isSameType;

	if (replace){
		_dst = dst;
	}
	else{
		_dst = Mat(dims, size, type());
	}


	{	//download from texture
		int _size = (int)_dst.total() * (int)_dst.elemSize();
		GLuint pbo = 0;
		glGenBuffers(1, &pbo);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
		glBufferData(GL_PIXEL_PACK_BUFFER, _size, 0, GL_DYNAMIC_DRAW);
		GLS_Assert(pbo);

		//Copy pixels from texture object to pbok
		if (dims == 2){
			glBindTexture(GL_TEXTURE_2D, texid());
			glGetTexImage(GL_TEXTURE_2D, 0, glFormat(), glType(), 0);
			GL_CHECK_ERROR();
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else if (dims == 3){
			glBindTexture(GL_TEXTURE_2D_ARRAY, texid());
			glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, glFormat(), glType(), 0);
			GL_CHECK_ERROR();
			glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

		}
		else {
			GLS_Assert(0 && "dims must be 2 or 3.");
		}

		pbo2mat(pbo, _dst);
		glDeleteBuffers(1, &pbo);
	}

	if (!dst.isContinuous() && isSameSize && isSameType){
		_dst.copyTo(dst);
	}
	else{
		dst = _dst;
	}

	//	cout << "download:end" << endl;
}

#else




void GlsMat::download(Mat&dst) const{
	//	cout << "download:start" << endl;

	_TMR_("-download:\t");

	//	dst = Mat(size(), type());

	// texture

	{	//download from texture
		Mat _dst;
		bool isSameSize = true;
		isSameSize &= (dst.dims == dims);
		if (isSameSize){
			for (int i = 0; i < dims; i++){
				isSameSize &= (dst.size[i] == size[i]);
			}
		}
		bool isSameType = (dst.type() == type());

		bool replace = dst.isContinuous() && isSameSize && isSameType;

		if (replace){
			_dst = dst;
		}
		else{
			_dst = Mat(dims,size, type());
		}
		if (_dst.dims == 2){
			void* data = _dst.data;
			glBindTexture(GL_TEXTURE_2D, texid());
			glGetTexImage(GL_TEXTURE_2D, 0, glFormat(), glType(), data);
			GL_CHECK_ERROR();
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else if (_dst.dims == 3){
			void* data = _dst.data;
			glBindTexture(GL_TEXTURE_2D_ARRAY, texid());
			glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, glFormat(), glType(), data);
			GL_CHECK_ERROR();
			glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		}
		//if (replace){
		//	dst = _dst;
		//}
		//else{
		//	_dst.copyTo(dst);
		//}
		if (!dst.isContinuous() && isSameSize && isSameType){
			_dst.copyTo(dst);
		}
		else{
			dst = _dst;
		}

	}

	//	cout << "download:end" << endl;
}

#endif

GlsMat::operator Mat() const{
	Mat mat;
	(*this).download(mat);
	return mat;
}

void GlsMat::setInterpolation(GLint interpolation) const{
	if (dims == 3){
		glBindTexture(GL_TEXTURE_2D_ARRAY, texid());
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, interpolation);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, interpolation);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
	else{
		glBindTexture(GL_TEXTURE_2D, texid());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}

void GlsMat::setWrapMode(GLint mode) const{
	if (dims == 3){
		glBindTexture(GL_TEXTURE_2D_ARRAY, texid());
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, mode);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, mode);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
	else{
		glBindTexture(GL_TEXTURE_2D, texid());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}


inline GlsMat::MSize::MSize(int* _p) : p(_p) {}
inline Size GlsMat::MSize::operator()(void) const { return Size(p[1], p[0]); }
inline const int& GlsMat::MSize::operator[](int i) const { return p[i]; }
inline int& GlsMat::MSize::operator[](int i) { return p[i]; }
inline GlsMat::MSize::operator const int*() const { return p; }

//inline bool GlsMat::MSize::operator == (const MSize& sz) const
//{
//	int d = p[-1], dsz = sz.p[-1];
//	if (d != dsz)
//		return false;
//	if (d == 2)
//		return p[0] == sz.p[0] && p[1] == sz.p[1];
//
//	for (int i = 0; i < d; i++)
//		if (p[i] != sz.p[i])
//			return false;
//	return true;
//}
//
//inline bool GlsMat::MSize::operator != (const MSize& sz) const
//{
//	return !(*this == sz);
//}

inline size_t GlsMat::total() const
{
	if (dims <= 2)
		return (size_t)rows*cols;
	size_t p = 1;
	for (int i = 0; i < dims; i++)
		p *= size[i];
	return p;
}


void GlsMat::copyTo(GlsMat& m) const{
	gls::copy(*this, m);
}

GlsMat GlsMat::clone(void) const{
	GlsMat ret(size(), type());
	copyTo(ret);
	return ret;
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






