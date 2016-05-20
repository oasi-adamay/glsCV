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

#include "glsFBO.h"

namespace gls
{

glsFBO::glsFBO(const int attachment_num){
	GLS_Assert(attachment_num < 16);
	// create FBO (off-screen framebuffer)
	glGenFramebuffers(1, &fbo);

	// bind offscreen framebuffer (that is, skip the window-specific render target)
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GLenum bufs[] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5,
		GL_COLOR_ATTACHMENT6,
		GL_COLOR_ATTACHMENT7,
		GL_COLOR_ATTACHMENT8,
		GL_COLOR_ATTACHMENT9,
		GL_COLOR_ATTACHMENT10,
		GL_COLOR_ATTACHMENT11,
		GL_COLOR_ATTACHMENT12,
		GL_COLOR_ATTACHMENT13,
		GL_COLOR_ATTACHMENT14,
		GL_COLOR_ATTACHMENT15,
	};
	glDrawBuffers(attachment_num, bufs);

}

glsFBO::~glsFBO(void){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
}

}//namespace gls
