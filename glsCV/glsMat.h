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
	shared_ptr<GLuint> _texid;
	int flag;		// opencv compatible

	void createTexture(const int _width, const int _height, int _type);
	void deleteTexture(void);

public:
	int rows;
	int cols;

	glsMat(const Mat & cvmat);
	glsMat(const Size size, const int type);
	glsMat(void);
	~glsMat(void);

	glsMat& operator=(const glsMat& rhs);

	GLuint texid(void)const { return *_texid; }

	Size size(void) const { return Size(cols, rows); }
	int type(void) const { return CV_MAT_TYPE(flag); }
	int channels(void) const { return CV_MAT_CN(flag); }
	int depth(void) const { return CV_MAT_DEPTH(flag); }

	GLenum glSizedFormat(void) const{ return convCVtype2GLsizedFormat(CV_MAT_TYPE(flag)); }
	GLenum glFormat(void) const{ return convCVtype2GLformat(CV_MAT_TYPE(flag)); }
	GLenum glType(void) const{ return convCVtype2GLtype(CV_MAT_TYPE(flag)); }
	bool empty(void) const { return _texid.use_count() == 0; }
	bool isContinuous(void) const { return true; }


	void CopyFrom(const Mat&src);
	void CopyTo(Mat&dst) const;

};







#endif