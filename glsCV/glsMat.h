#ifndef _GLS_MAT_H_
#define _GLS_MAT_H_

#include <memory>

GLenum convCVtype2GLsizedFormat(int type);
GLenum convCVtype2GLformat(int type);
GLenum convCVtype2GLtype(int type);
int convFmtGL2CV(GLenum  format);

class glsMat
{
private:
	shared_ptr<int> refcount;
	int flag;		// opencv compatible

//	glsMat& operator=(const glsMat&);   ///  Uncopyable
//	glsMat(const glsMat& src, bool copy);

	void createTexture(const int _width, const int _height, int _type, const int _blkX = 1, const int _blkY = 1);

	int blkX;
	int blkY;

public:
	int rows;
	int cols;

	vector<GLuint> texArray;

	glsMat(const Mat & cvmat, bool upload = true);
	glsMat(const Size size, const int type, const Size blkNum = Size(1,1));
	glsMat(void);
	~glsMat(void);

	glsMat& operator=(const glsMat& rhs);

	Size size(void) const { return Size(cols, rows); }
	Size blkNum(void) const { return Size(blkX, blkY); }
	int type(void) const { return CV_MAT_TYPE(flag); }
	GLenum glSizedFormat(void) const{ return convCVtype2GLsizedFormat(CV_MAT_TYPE(flag)); }
	GLenum glFormat(void) const{ return convCVtype2GLformat(CV_MAT_TYPE(flag)); }
	GLenum glType(void) const{ return convCVtype2GLtype(CV_MAT_TYPE(flag)); }
	bool empty(void) const { return refcount.use_count() == 0; }
	bool isContinuous(void) const { return texArray.size() == 1; }


	GLuint at(const int y, const int x);
	void CopyFrom(const Mat&src);
	void CopyTo(Mat&dst);

	int texWidth(void) const { return cols / blkX;}
	int texHeight(void) const { return rows / blkY; }

};







#endif