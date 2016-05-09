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