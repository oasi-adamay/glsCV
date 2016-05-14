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

namespace gls
{

GLenum convCVtype2GLsizedFormat(int type);
GLenum convCVtype2GLformat(int type);
GLenum convCVtype2GLtype(int type);
int convFmtGL2CV(GLenum  format);

//! �Q�ƃJ�E���^�����COpenGL texture�p�̊��X�g���[�W�N���X
class GlsMat
{
private:
	shared_ptr<GLuint> _texid;	//!< �e�N�X�`��id��ۑ�����Ashared_ptr
	int flag;					//!< �t�H�[�}�b�g����ۑ�����t���O

	//! �e�N�X�`������
	void createTexture(const int _width, const int _height, int _type);
	//! �e�N�X�`���폜
	void deleteTexture(void);

public:
	
	int rows;	//!< �s��(height)
	int cols;	//!< ��(width)


	//! cv::mat�Ɠ����T�C�Y�A����type�̃e�N�X�`���[�𐶐����A�f�[�^��upload���܂��B
	GlsMat(const Mat & cvmat);
	//! size, type�̃e�N�X�`���[�𐶐����܂��B
	GlsMat(const Size size, const int type);
	//! ��̍s��ł��B�e�N�X�`���[�͐�������܂���B
	GlsMat(void);

	//! �f�X�g���N�^�@�Q�ƃJ�E���g���Ō�̃G���g���������Ƃ��ɁA�e�N�X�`���[���폜���܂��B
	~GlsMat(void);

	//! ����@�w�b�_�[���݂̂̃R�s�[�ŁA�f�[�^���̂̃R�s�[�͔������܂���B
	GlsMat& operator=(const GlsMat& rhs);

	//! �e�N�X�`���[��ID�̎擾
	GLuint texid(void)const { return _texid.use_count() == 0 ? 0 :* _texid; }

	//! �s��T�C�Y(width,height)
	Size size(void) const { return Size(cols, rows); }
	//! �s��v�f�̌^��Ԃ��܂��DOpenCV��Mat.type()�Ɠ����ł��B
	int type(void) const { return CV_MAT_TYPE(flag); }
	//! �`�����l�����@(1-4)
	int channels(void) const { return CV_MAT_CN(flag); }
	//! �s��v�f�̃r�b�g�[�x��Ԃ��܂��DOpenCV��Mat.depth()�Ɠ����ł��B
	int depth(void) const { return CV_MAT_DEPTH(flag); }

	//! OpenGL internal format (SizedFormat���g�p���܂��BGL_RGB32F�Ȃ�)
	GLenum glSizedFormat(void) const{ return convCVtype2GLsizedFormat(CV_MAT_TYPE(flag)); }
	//! OpenGL format (GL_RED,GL_RG,GL_RGB,GL_RED_INTEGER��)
	GLenum glFormat(void) const{ return convCVtype2GLformat(CV_MAT_TYPE(flag)); }
	//! OpenGL type (GL_FLOAT,GL_UNSIGED_BYTET��)
	GLenum glType(void) const{ return convCVtype2GLtype(CV_MAT_TYPE(flag)); }

	//! �z�񂪗v�f�������Ȃ��i�e�N�X�`���[����������Ă��Ȃ��j�ꍇ�� true ��Ԃ��܂��D
	bool empty(void) const { return _texid.use_count() == 0; }
	bool isContinuous(void) const { return true; }


	//! CPU->GPU�ւ�upload
	void upload(const Mat&src);

	//! GPU->CPU�ւ�download
	void download(Mat&dst) const;
	operator Mat() const;


};

}//namespace gls

using gls::GlsMat;

#endif