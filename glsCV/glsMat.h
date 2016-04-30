#ifndef _GLS_MAT_H_
#define _GLS_MAT_H_

class glsMat
{
private:
	glsMat& operator=(const glsMat&);   ///  Uncopyable
	void createTexture(const int _width, const int _height, GLenum _internalFormat, const int _blkX = 1, const int _blkY = 1);

	int blkX;
	int blkY;

public:
	GLenum internalFormat;
	GLenum format;
	GLenum type;
	int width;
	int height;

	vector<GLuint> texArray;

	glsMat(const int _width, const int _height, GLenum _internalFormat , const int _blkX = 1 , const int _blkY = 1);
	glsMat(const Mat & cvmat,bool upload = true);
	glsMat(const glsMat& src, bool copy = false);

	~glsMat(void);

	GLuint at(const int y, const int x);
	void CopyFrom(const Mat&src);
	void CopyTo(Mat&dst);

	int texWidth(void) const { return width / blkX;}
	int texHeight(void) const { return height / blkY; }

};



GLenum convFmtCV2GL(int ocvtype);
int convFmtGL2CV(GLenum  format);




#endif