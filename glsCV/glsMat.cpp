#include "stdafx.h"


#include "glsMat.h"
#include "Timer.h"

#ifdef DEBUG
#define _TMR_(...)  Timer tmr(__VA_ARGS__)
#else
#define _TMR_(...)
#endif


#define _USE_PBO_UP
#define _USE_PBO_DOWN

glsMat::glsMat(void){
	flag = 0;
	width = 0;
	height = 0;
	blkX = 0;
	blkY = 0;
	texArray.clear();
}

glsMat::glsMat(const Size size, const int ocvtype, const Size blkNum){

	createTexture(size.width, size.height, ocvtype, blkNum.width, blkNum.height);
}


glsMat::glsMat(const int _width, const int _height, int _type, const int _blkX, const int _blkY){
	createTexture(_width, _height, _type, _blkX, _blkY);
}


glsMat::glsMat(const Mat & cvmat, bool upload){
	createTexture(cvmat.cols, cvmat.rows, cvmat.type(), 1, 1);
	if (upload)	CopyFrom(cvmat);
}

glsMat& glsMat::operator=(const glsMat& rhs){
	if (refcount.use_count() == 1){
		glDeleteTextures((GLsizei)texArray.size(), &texArray[0]);
		texArray.clear();
//		refcount.reset();
	}
	
	refcount = rhs.refcount;
	flag = rhs.flag;
	width = rhs.width;
	height = rhs.height;
	blkX = rhs.blkX;
	blkY = rhs.blkY;
	texArray = rhs.texArray;

	return *this;
}


//glsMat::glsMat(const glsMat& src, bool copy){
//	createTexture(src.width, src.height, src.internalFormat, src.blkX, src.blkY);
//	assert(copy == false);	///@TODO Copy tex
//}




glsMat::~glsMat(void){
	if (refcount.use_count()==1){
		glDeleteTextures((GLsizei)texArray.size(), &texArray[0]);
		texArray.clear();
	}
}

void glsMat::createTexture(
	const int _width,				//image width
	const int _height,				//image height
	const int _type,				//type(OpenCV)
	const int _blkX,				// block num X
	const int _blkY					// block num Y
	){
	assert((_width%_blkX) == 0);
	assert((_height%_blkY) == 0);

	flag = _type;

	width = _width;
	height = _height;
	blkX = _blkX;
	blkY = _blkY;
	int texWidth = width / blkX;
	int texHeight = height / blkY;

	texArray.resize(blkY*blkX);

	glGenTextures((GLsizei)texArray.size(), &texArray[0]); // create (reference to) a new texture

	for (int i = 0; i < (int)texArray.size(); i++){
		glBindTexture(GL_TEXTURE_RECTANGLE, texArray[i]);
		// (set texture parameters here)
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		//create the texture
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, glSizedFormat(), texWidth, texHeight, 0, glFormat(), glType(), 0);

		glBindTexture(GL_TEXTURE_RECTANGLE, 0);
	}

	refcount = make_shared<int>(1);
}


GLuint glsMat::at(const int y, const int x){
	assert(y*blkX + x < texArray.size());
	return texArray[y*blkX + x];
}

//-----------------------------------------------------------------------------
//Upload texture from cv::Mat to GL texture
void glsMat::CopyFrom(const Mat&src){
	_TMR_("-upload :\t");

	CV_Assert(src.type() == ocvtype());
	CV_Assert(src.cols == width);
	CV_Assert(src.rows == height);

	// texture
	const int texWidth = src.cols / blkX;
	const int texHeight = src.rows / blkY;

	{
#ifdef _USE_PBO_UP
		int size = texWidth*texHeight * (int)src.elemSize();
		vector<GLuint> pbo(texArray.size());
		glGenBuffers((GLsizei)pbo.size(), &pbo[0]);
		for (int i = 0; i < pbo.size(); i++){
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[i]);
			glBufferData(GL_PIXEL_UNPACK_BUFFER, size, 0, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}


		for (int by = 0; by < blkY; by++){
			for (int bx = 0; bx < blkX; bx++){
				int i = by*blkX + bx;
				int x = (bx)* texWidth;
				int y = (by)* texHeight;
				Rect rect(x, y, texWidth, texHeight);
				Mat roi = Mat(src, rect);	// 1/2  1/2 rect

				//bind current pbo for app->pbo transfer
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[i]); //bind pbo
				GLubyte* ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, size,
					GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
				assert(ptr != 0);

				int lSize = roi.cols * (int)roi.elemSize();	// line size in byte
#ifdef _OPENMP
#pragma omp parallel for
#endif
				for (int y = 0; y < roi.rows; y++){
					uchar* pSrc = roi.ptr<uchar>(y);
					uchar* pDst = ptr + lSize * y;
					memcpy(pDst, pSrc, lSize);
				}
				glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

				//Copy pixels from pbo to texture object
				glBindTexture(GL_TEXTURE_RECTANGLE, texArray[i]);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[i]); //bind pbo
				glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, texWidth, texHeight, glFormat(), glType(), 0);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
				glBindTexture(GL_TEXTURE_RECTANGLE, 0);
			}
		}

		glDeleteBuffers((GLsizei)pbo.size(), &pbo[0]);
#else
		for (int by = 0; by < blkY; by++){
			for (int bx = 0; bx < blkX; bx++){
				int i = by*blkX + bx;
				int x = (bx)* texWidth;
				int y = (by)* texHeight;
				Rect rect(x, y, texWidth, texHeight);
				Mat roi = Mat(src, rect).clone();	// 1/2  1/2 rect
				CV_Assert(roi.isContinuous());
				void* data = roi.data;

				glBindTexture(GL_TEXTURE_RECTANGLE, texArray[i]);
				glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, texWidth, texHeight, format, type, data);
				glBindTexture(GL_TEXTURE_RECTANGLE, 0);
			}
		}
#endif
	}

}

void glsMat::CopyTo(Mat&dst){
	_TMR_("-download:\t");

	dst = Mat(Size(width, height), ocvtype());

	// texture
	const int texWidth = width / blkX;
	const int texHeight = height / blkY;

	{	//download from texture
#ifdef _USE_PBO_DOWN
		int size = texWidth*texHeight * (int)dst.elemSize();
		vector<GLuint> pbo(texArray.size());
		glGenBuffers((GLsizei)pbo.size(), &pbo[0]);
		for (int i = 0; i < pbo.size(); i++){
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo[i]);
			glBufferData(GL_PIXEL_UNPACK_BUFFER, size, 0, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}
		for (int by = 0; by < blkY; by++){
			for (int bx = 0; bx < blkX; bx++){
				int i = by*blkX + bx;
				int x = (bx)* texWidth;
				int y = (by)* texHeight;

				Rect rect(x, y, texWidth, texHeight);
				Mat roi = Mat(dst, rect);	// 1/2  1/2 rect

				//Copy pixels from texture object to pbo_bank
				glBindTexture(GL_TEXTURE_RECTANGLE, texArray[i]);
				glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[i]); //bind pbo
				glGetTexImage(GL_TEXTURE_RECTANGLE, 0, glFormat(), glType(), 0);
				glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
				glBindTexture(GL_TEXTURE_RECTANGLE, 0);

				//bind current pbo for app->pbo transfer
				glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[i]); //bind pbo
				GLubyte* ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, size,
					GL_MAP_READ_BIT);
				assert(ptr != 0);

				int lSize = roi.cols * (int)roi.elemSize();	// line size in byte
#ifdef _OPENMP
#pragma omp parallel for
#endif
				for (int y = 0; y < roi.rows; y++){
					uchar* pSrc = ptr + lSize * y;
					uchar* pDst = roi.ptr<uchar>(y);
					memcpy(pDst, pSrc, lSize);
				}
				glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
			}
		}
		glDeleteBuffers((GLsizei)pbo.size(), &pbo[0]);
#else
		Mat tmp = Mat(Size(texWidth, texHeight), dst.type());
		for (int by = 0; by < blkY; by++){
			for (int bx = 0; bx < blkX; bx++){
				int i = by*blkX + bx;
				int x = (bx)* texWidth;
				int y = (by)* texHeight;
				void* data = tmp.data;

				glBindTexture(GL_TEXTURE_RECTANGLE, texArray[i]);
				glGetTexImage(GL_TEXTURE_RECTANGLE, 0, glFormat(), glType(), data);
				glBindTexture(GL_TEXTURE_RECTANGLE, 0);

				Rect rect(x, y, texWidth, texHeight);
				Mat roi = Mat(dst, rect);	// 1/2  1/2 rect
				tmp.copyTo(roi);
			}
		}
#endif
	}
}




GLenum convCVtype2GLsizedFormat(int ocvtype){
	GLenum format;
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
	int ocvtype;
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
	GLenum format;
	int ch = CV_MAT_CN(ocvtype);
	int depth = CV_MAT_DEPTH(ocvtype);
	bool isFloat = depth == (CV_32F);
	if (isFloat){
		switch (ch){
			case(1) : format = GL_RED; break;
			case(2) : format = GL_RG; break;
			case(3) : format = GL_RGB; break;
			case(4) : format = GL_RGBA; break;
			default: assert(0);
		}
	}
	else{
		switch (ch){
			case(1) : format = GL_RED_INTEGER; break;
			case(2) : format = GL_RG_INTEGER; break;
			case(3) : format = GL_RGB_INTEGER; break;
			case(4) : format = GL_RGBA_INTEGER; break;
			default: assert(0);
		}
	}
	return format;
}


GLenum convCVtype2GLtype(int ocvtype){
	GLenum type;
	int depth = CV_MAT_DEPTH(ocvtype);
	switch (depth){
	case(CV_8U) : type = GL_UNSIGNED_BYTE; break;
	case(CV_8S) : type = GL_BYTE; break;
	case(CV_16U) : type = GL_UNSIGNED_SHORT; break;
	case(CV_16S) : type = GL_SHORT; break;
//	case(CV_32U) : type = GL_UNSIGNED_INT; break;
	case(CV_32S) : type = GL_INT; break;
	case(CV_32F) : type = GL_FLOAT; break;
	default: assert(0);
	}
	return type;
}






