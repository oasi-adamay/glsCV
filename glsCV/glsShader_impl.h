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

#ifndef _GLS_SHADER_IMPL_H_
#define _GLS_SHADER_IMPL_H_

#include "glsMacro.h"

#include "glsVAO.h"
#include "glsFBO.h"

#ifdef _DEBUG
//#define _DEBUG_SHADER
#endif

namespace gls
{

template<class T>
void glsShaderBase::setup(SetupInfo* info, const T& t)
{
	Gls_Assert(0);	//not implement
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "T:" << t << std::endl;
#endif
}

template<>
void glsShaderBase::setup<int>(SetupInfo* info, const int& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniform1i(loc, t);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}

template<>
void glsShaderBase::setup<float>(SetupInfo* info, const float& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniform1f(loc, t);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}

template<>
void glsShaderBase::setup<double>(SetupInfo* info, const double& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	float val = (float)t;
	glUniform1f(loc, val);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}

template<>
void glsShaderBase::setup<Scalar>(SetupInfo* info, const Scalar& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	float val[4] = { (float)t[0], (float)t[1], (float)t[2], (float)t[3] };
	glUniform4fv(loc, 1, val);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}

template<>
void glsShaderBase::setup<Vec2f>(SetupInfo* info, const Vec2f& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniform2fv(loc, 1, &t.val[0]);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}

template<>
void glsShaderBase::setup<Vec3f>(SetupInfo* info, const Vec3f& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniform3fv(loc, 1, &t.val[0]);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}

template<>
void glsShaderBase::setup<Vec4f>(SetupInfo* info, const Vec4f& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniform4fv(loc, 1, &t.val[0]);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}

template<>
void glsShaderBase::setup<Vec2i>(SetupInfo* info, const Vec2i& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniform2iv(loc, 1, &t.val[0]);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}

template<>
void glsShaderBase::setup<Vec3i>(SetupInfo* info, const Vec3i& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniform3iv(loc, 1, &t.val[0]);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}

template<>
void glsShaderBase::setup<Vec4i>(SetupInfo* info, const Vec4i& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniform4iv(loc, 1, &t.val[0]);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}


template<>
void glsShaderBase::setup<Matx22f>(SetupInfo* info, const Matx22f& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniformMatrix2fv(loc, 1, GL_TRUE, &t.val[0]);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}

template<>
void glsShaderBase::setup<Matx33f>(SetupInfo* info, const Matx33f& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniformMatrix3fv(loc, 1, GL_TRUE, &t.val[0]);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}

template<>
void glsShaderBase::setup<Matx44f>(SetupInfo* info, const Matx44f& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniformMatrix4fv(loc, 1, GL_TRUE, &t.val[0]);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}

template<>
void glsShaderBase::setup<Matx23f>(SetupInfo* info, const Matx23f& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniformMatrix3x2fv(loc, 1, GL_TRUE, &t.val[0]);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}


template<>
void glsShaderBase::setup<Matx32f>(SetupInfo* info, const Matx32f& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniformMatrix2x3fv(loc, 1, GL_TRUE, &t.val[0]);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}

template<>
void glsShaderBase::setup<Matx34f>(SetupInfo* info, const Matx34f& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniformMatrix4x3fv(loc, 1, GL_TRUE, &t.val[0]);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}

template<>
void glsShaderBase::setup<Matx43f>(SetupInfo* info, const Matx43f& t)
{
	const int i = info->argnum++;
	GLuint loc = uniformLocArray[i];
	glUniformMatrix3x4fv(loc, 1, GL_TRUE, &t.val[0]);
#ifdef _DEBUG_SHADER
	std::cout << "i:" << i << "\t";
	std::cout << "loc:" << loc << "\t";
	std::cout << "val:" << t << endl;
#endif
}


template<>
void glsShaderBase::setup<GlsMat>(SetupInfo* info, const GlsMat& t)
{
	const int i = info->argnum++;
	if (i<uniformLocArray.size()){	//src texture
		if (!t.empty()){
			GLuint loc = uniformLocArray[i];
			int id = info->texSrcNum++;
			info->texSrcSize = t.size();
			glActiveTexture(GL_TEXTURE0 + id);
			glBindTexture(GL_TEXTURE_2D, t.texid());
			glUniform1i(loc, id);

#ifdef _DEBUG_SHADER
			std::cout << "i:" << i << "\t";
			std::cout << "loc:" << loc << "\t";
			std::cout << "texunit:" << id << "\t";
			std::cout << "texSrc:" << t.texid() << endl;
#endif
		}

	}
	else{	//dst texture
		if (!t.empty()){
			if (info->texDstNum == 0){
				// create FBO (off-screen framebuffer)
				glGenFramebuffers(1,&(info->fbo));
				// bind offscreen framebuffer (that is, skip the window-specific render target)
				glBindFramebuffer(GL_FRAMEBUFFER, info->fbo);
			}
			int id = info->texDstNum++;
			info->texDstSize = t.size();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + id, GL_TEXTURE_2D, t.texid(), 0);
			GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
#ifdef _DEBUG_SHADER
			std::cout << "i:" << i << "\t";
			std::cout << "attach:" << id << "\t";
			std::cout << "texdst:" << t.texid() << endl;
#endif
		}

	}
}



template<class First, class... Rest>
void glsShaderBase::setup(SetupInfo* info, const First& first, const Rest&... rest)
{
	setup(info, first);
	setup(info, rest...);
}


template<class First, class... Rest>
void glsShaderBase::Execute(const First& first, const Rest&... rest)
{
	glsVAO vao(glGetAttribLocation(program(), "position"));
//	glsFBO fbo(1);

	//program
	{
		glUseProgram(program());
	}

	//setup
	SetupInfo info = { 0 };

	{
		setup(&info, first);
		setup(&info, rest...);
	}
#ifdef _DEBUG_SHADER
	cout << "argnum:" << info.argnum << endl;
	cout << "texSrcNum:" << info.texSrcNum << endl;
	cout << "texDstNum:" << info.texDstNum << endl;
	cout << "texDstSize:" << info.texDstSize << endl;

#endif

	if (info.texDstNum){
		DrawBuffers(info.texDstNum);

		//Viewport
		glViewport(0, 0, info.texDstSize.width, info.texDstSize.height);
	}
	else{
		//Viewport
		glViewport(0, 0, info.texSrcSize.width, info.texSrcSize.height);
	}

	//Render!!
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glFlush();
	}

	if (info.fbo){
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &(info.fbo));
	}

	GL_CHECK_ERROR();

}






}//namespace gls


#endif