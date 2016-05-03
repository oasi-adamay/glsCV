#include "stdafx.h"

#include "glsCV.h"
#include "glsFBO.h"

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
	glDeleteFramebuffers(1, &fbo);
}
