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

#include "glsCV.h"
#include "glsBasicOperation.h"

namespace gls
{


class glsShaderScalarOperation : public glsShaderBase
{
public:
	glsShaderScalarOperation(void);

	//attribute location
	GLuint position;
	//uniform  location
	GLuint texSrc;
	GLuint v4_scalar;
	GLuint i_flags;
};



class glsShaderBinaryOperation : public glsShaderBase
{
public:
	glsShaderBinaryOperation(void);

	//attribute location
	GLuint position;
	//uniform  location
	GLuint texSrc0;
	GLuint texSrc1;
	GLuint i_flags;
};


//-----------------------------------------------------------------------------
//global 
glsShaderScalarOperation* shaderScalarOperation = 0;
glsShaderBinaryOperation* shaderBinaryOperation = 0;

void ShaderBasicOperationInit(void){
	shaderScalarOperation = new glsShaderScalarOperation();
	shaderBinaryOperation = new glsShaderBinaryOperation();
}

void ShaderBasicOperationTerminate(void){
	delete shaderScalarOperation;
	delete shaderBinaryOperation;
}

#define OPCODE_ADD 0
#define OPCODE_SUB 1
#define OPCODE_MUL 2
#define OPCODE_DIV 3
#define OPCODE_MIN 4
#define OPCODE_MAX 5
#define OPCODE_MUL_SPCETRUM		 6
#define OPCODE_MUL_SPCETRUM_CONJ 7
#define OPCODE_MUL_SPCETRUM_POC  8
#define OPCODE_MAG_SPCETRUM		 9
#define OPCODE_LOG 10
#define OPCODE_EXP 11
#define OPCODE_POW 12
#define OPCODE_LOG_MAG_SPCETRUM		 13

static const char vertexShaderCode[] =
"#version 330 core\n"
"layout (location = 0)in  vec2 position;\n"
"void main(void)\n"
"{\n"
"   gl_Position  = vec4(position,0.0,1.0);\n"
"}\n"
;

//-----------------------------------------------------------------------------
glsShaderScalarOperation::glsShaderScalarOperation(void){
	const char fragmentShaderCode[] =
		"#version 330 core\n"
		"precision highp float;\n"
		"uniform sampler2D	texSrc;\n"
		"uniform vec4 scalar;\n"
		"uniform int	flags;\n"
		"layout (location = 0) out vec4 dst;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	vec4 src = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0);\n"
		"	switch(flags){\n"
		"   case(0): dst = scalar + src; break;\n"
		"	case(1): dst = scalar - src; break;\n"
		"	case(2): dst = scalar * src; break;\n"
		"	case(3): dst = scalar / src; break;\n"
		"	case(4)://OPCODE_MIN \n"
		"		dst.r = scalar.r < src.r? scalar.r : src.r;\n"
		"		dst.g = scalar.g < src.g? scalar.g : src.g;\n"
		"		dst.b = scalar.b < src.b? scalar.b : src.b;\n"
		"		dst.a = scalar.a < src.a? scalar.a : src.a;\n"
		"		break;\n"
		"	case(5)://OPCODE_MAX \n"
		"		dst.r = scalar.r > src.r? scalar.r : src.r;\n"
		"		dst.g = scalar.g > src.g? scalar.g : src.g;\n"
		"		dst.b = scalar.b > src.b? scalar.b : src.b;\n"
		"		dst.a = scalar.a > src.a? scalar.a : src.a;\n"
		"		break;\n"
		"	case(9)://OPCODE_MAG_SPCETRUM \n"
		"		dst.r = sqrt(src.r*src.r +  src.g*src.g);\n"
		"		break;\n"
		"	case(10)://OPCODE_LOG \n"
		"		dst = log(abs(src));\n"
		"		break;\n"
		"	case(11)://OPCODE_EXP \n"
		"		dst = exp(src);\n"
		"		break;\n"
		"	case(12)://OPCODE_POW \n"
		"		dst = pow(src,scalar);\n"
		"		break;\n"
		"	case(13)://OPCODE_LOG_MAG_SPCETRUM \n"
		"		dst.r = log(abs(sqrt(src.r*src.r +  src.g*src.g)+scalar.r));\n"
		"		break;\n"
		"	default: dst = src;\n"
		"   }\n"
		"}\n"
		;


	LoadShadersCode(vertexShaderCode, fragmentShaderCode);

	// Attribute & Uniform location
	position = glGetAttribLocation(program, "position");

	texSrc = glGetUniformLocation(program, "texSrc");
	v4_scalar = glGetUniformLocation(program, "scalar");
	i_flags = glGetUniformLocation(program, "flags");

}


//-------------------------------------------------------------
// Scalar operation
static
void glslScalarOperation(
const glsShaderScalarOperation* shader,	//progmra ID
const vec4 scalar,				//scalar
const GLuint texSrc,			//src texture ID
const GLuint texDst,			//dst texture ID
const int flags					//flag (opcode)
)
{
	int width;
	int height;

	glBindTexture(GL_TEXTURE_2D, texSrc);
	glGetTexLevelParameteriv(
		GL_TEXTURE_2D, 0,
		GL_TEXTURE_WIDTH, &width
		);

	glGetTexLevelParameteriv(
		GL_TEXTURE_2D, 0,
		GL_TEXTURE_HEIGHT, &height
		);

	glBindTexture(GL_TEXTURE_2D, 0);

	
	glsFBO fbo;		//FBO 
	glsVAO vao(shaderBinaryOperation->position);	//VAO

	//program
	{
		glUseProgram(shader->program);
	}

	//uniform
	{
		glUniform1i(shader->i_flags, flags);
		glUniform4fv(shader->v4_scalar, 1,&scalar[0]);

	}

	//Bind Texture
	{
		int id = 0;
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, texSrc);
		glUniform1i(shader->texSrc, id);
	}

	//dst texture
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texDst, 0);
		GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

	GLenum bufs[] =
	{
		GL_COLOR_ATTACHMENT0,
	};
	glDrawBuffers(1, bufs);

	//Viewport
	{
		glViewport(0, 0, width, height);
	}

	//Render!!
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glFlush();
	}

	GL_CHECK_ERROR();

}






//-----------------------------------------------------------------------------
glsShaderBinaryOperation::glsShaderBinaryOperation(void){

	const char fragmentShaderCode[] =
		"#version 330 core\n"
		"precision highp float;\n"
		"uniform sampler2D	texSrc0;\n"
		"uniform sampler2D	texSrc1;\n"
		"uniform int			flags;\n"
		"layout (location = 0) out vec4 dst;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	vec4 src0 = texelFetch(texSrc0, ivec2(gl_FragCoord.xy),0);\n"
		"	vec4 src1 = texelFetch(texSrc1, ivec2(gl_FragCoord.xy),0);\n"
		"	switch(flags){\n"
		"   case(0): dst = src0 + src1; break;\n"
		"	case(1): dst = src0 - src1; break;\n"
		"	case(2): dst = src0 * src1; break;\n"
		"	case(3): dst = src0 / src1; break;\n"
		"	case(4)://OPCODE_MIN \n"
		"		dst.r = src0.r < src1.r? src0.r : src1.r;\n"
		"		dst.g = src0.g < src1.g? src0.g : src1.g;\n"
		"		dst.b = src0.b < src1.b? src0.b : src1.b;\n"
		"		dst.a = src0.a < src1.a? src0.a : src1.a;\n"
		"		break;\n"
		"	case(5)://OPCODE_MAX \n"
		"		dst.r = src0.r > src1.r? src0.r : src1.r;\n"
		"		dst.g = src0.g > src1.g? src0.g : src1.g;\n"
		"		dst.b = src0.b > src1.b? src0.b : src1.b;\n"
		"		dst.a = src0.a > src1.a? src0.a : src1.a;\n"
		"		break;\n"
		"	case(6)://OPCODE_MUL_SPCETRUM \n"
		"		dst.r = src0.r*src1.r - src0.g*src1.g;\n"
		"		dst.g = src0.r*src1.g + src0.g*src1.r;\n"
		"		dst.b = src0.b*src1.b - src0.a*src1.a;\n"
		"		dst.a = src0.b*src1.a + src0.a*src1.b;\n"
		"		break;\n"
		"	case(7)://OPCODE_MUL_SPCETRUM_CONJ \n"
		"		dst.r = src0.r*src1.r + src0.g*src1.g;\n"
		"		dst.g = -src0.r*src1.g + src0.g*src1.r;\n"
		"		dst.b = src0.b*src1.b + src0.a*src1.a;\n"
		"		dst.a = -src0.b*src1.a + src0.a*src1.b;\n"
		"		break;\n"
		"	case(8)://OPCODE_MUL_SPCETRUM_POC \n"
		"       vec2 tmp0;\n"
		"       vec2 tmp1;\n"
		"		tmp0.r = src0.r*src1.r + src0.g*src1.g;\n"
		"		tmp0.g = -src0.r*src1.g + src0.g*src1.r;\n"
		"		tmp1.r = src0.b*src1.b + src0.a*src1.a;\n"
		"		tmp1.g = -src0.b*src1.a + src0.a*src1.b;\n"
		"		dst.r = tmp0.r / sqrt(tmp0.r*tmp0.r+tmp0.g*tmp0.g);\n"
		"		dst.g = tmp0.g / sqrt(tmp0.r*tmp0.r+tmp0.g*tmp0.g);\n"
		"		dst.b = tmp1.r / sqrt(tmp1.r*tmp1.r+tmp1.g*tmp1.g);\n"
		"		dst.a = tmp1.g / sqrt(tmp1.r*tmp1.r+tmp1.g*tmp1.g);\n"
		"		break;\n"
		"	default: dst = src0;\n"
		"   }\n"
		"}\n"
		;


	LoadShadersCode(vertexShaderCode, fragmentShaderCode);

	// Attribute & Uniform location
	position = glGetAttribLocation(program, "position");

	texSrc0  = glGetUniformLocation(program, "texSrc0");
	texSrc1 = glGetUniformLocation(program, "texSrc1");
	i_flags = glGetUniformLocation(program, "flags");

}

//-------------------------------------------------------------
// Binary operation
static 
void glslBinaryOperation(
	const glsShaderBinaryOperation* shader,	//progmra ID
	const GLuint texSrc0,	//src texture ID
	const GLuint texSrc1,	//src texture ID
	const GLuint texDst,	//dst texture ID
	const int flags					//flag (opcode)
	)
{
	int width;
	int height;

	glBindTexture(GL_TEXTURE_2D, texSrc0);
	glGetTexLevelParameteriv(
		GL_TEXTURE_2D, 0,
		GL_TEXTURE_WIDTH, &width
		);

	glGetTexLevelParameteriv(
		GL_TEXTURE_2D, 0,
		GL_TEXTURE_HEIGHT, &height
		);

	glBindTexture(GL_TEXTURE_2D, 0);

	glsFBO fbo;		//FBO 
	glsVAO vao(shaderBinaryOperation->position);	//VAO


	//program
	{
		glUseProgram(shader->program);
	}

	//uniform
	{
		glUniform1i(shader->i_flags, flags);
	}

	//Bind Texture
	{
		int id = 0;
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, texSrc0);
		glUniform1i(shader->texSrc0, id);
		id++;
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, texSrc1);
		glUniform1i(shader->texSrc1, id);
	}
	//dst texture
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texDst, 0);
		GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

	GLenum bufs[] =
	{
		GL_COLOR_ATTACHMENT0,
	};
	glDrawBuffers(1, bufs);

	//Viewport
	{
		glViewport(0, 0, width, height);
	}

	//Render!!
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glFlush();
	}

	GL_CHECK_ERROR();

}

void add(const vec4& scalar, const GlsMat& src, GlsMat& dst){
	GLS_Assert(src.glType() == GL_FLOAT);
	GlsMat _dst = GlsMat(src.size(), src.type());
	glslScalarOperation(shaderScalarOperation, scalar, src.texid(), _dst.texid(), OPCODE_ADD);
	dst = _dst;
}

void add(const GlsMat& src0, const GlsMat& src1, GlsMat& dst)
{
	GLS_Assert(src0.glType() == GL_FLOAT);
	GLS_Assert(src1.glType() == GL_FLOAT);
	GlsMat _dst = GlsMat(src0.size(), src0.type());
	glslBinaryOperation(shaderBinaryOperation, src0.texid(), src1.texid(), _dst.texid(), OPCODE_ADD);
	dst = _dst;
}


void subtract(const vec4& scalar, const GlsMat& src, GlsMat& dst){
	GLS_Assert(src.glType() == GL_FLOAT);
	GlsMat _dst = GlsMat(src.size(), src.type());
	glslScalarOperation(shaderScalarOperation, scalar, src.texid(), _dst.texid(), OPCODE_SUB);
	dst = _dst;
}

void subtract(const GlsMat& src0, const GlsMat& src1, GlsMat& dst)
{
	GLS_Assert(src0.glType() == GL_FLOAT);
	GLS_Assert(src1.glType() == GL_FLOAT);
	GlsMat _dst = GlsMat(src0.size(), src0.type());
	glslBinaryOperation(shaderBinaryOperation, src0.texid(), src1.texid(), _dst.texid(), OPCODE_SUB);
	dst = _dst;
}


void multiply(const vec4& scalar, const GlsMat& src, GlsMat& dst){
	GLS_Assert(src.glType() == GL_FLOAT);
	GlsMat _dst = GlsMat(src.size(), src.type());
	glslScalarOperation(shaderScalarOperation, scalar, src.texid(), _dst.texid(), OPCODE_MUL);
	dst = _dst;
}

void multiply(const GlsMat& src0, const GlsMat& src1, GlsMat& dst)
{
	GLS_Assert(src0.glType() == GL_FLOAT);
	GLS_Assert(src1.glType() == GL_FLOAT);
	GlsMat _dst = GlsMat(src0.size(), src0.type());
	glslBinaryOperation(shaderBinaryOperation, src0.texid(), src1.texid(), _dst.texid(), OPCODE_MUL);
	dst = _dst;
}

void divide(const vec4& scalar, const GlsMat& src, GlsMat& dst){
	GLS_Assert(src.glType() == GL_FLOAT);
	GlsMat _dst = GlsMat(src.size(), src.type());
	glslScalarOperation(shaderScalarOperation, scalar, src.texid(), _dst.texid(), OPCODE_DIV);
	dst = _dst;
}


void divide(const GlsMat& src0, const GlsMat& src1, GlsMat& dst){
	GLS_Assert(src0.glType() == GL_FLOAT);
	GLS_Assert(src1.glType() == GL_FLOAT);
	GlsMat _dst = GlsMat(src0.size(), src0.type());
	glslBinaryOperation(shaderBinaryOperation, src0.texid(), src1.texid(), _dst.texid(), OPCODE_DIV);
	dst = _dst;
}

void min(const vec4& scalar, const GlsMat& src, GlsMat& dst){
	GLS_Assert(src.glType() == GL_FLOAT);
	GlsMat _dst = GlsMat(src.size(), src.type());
	glslScalarOperation(shaderScalarOperation, scalar, src.texid(), _dst.texid(), OPCODE_MIN);
	dst = _dst;
}

void min(const GlsMat& src0, const GlsMat& src1, GlsMat& dst){
	GLS_Assert(src0.glType() == GL_FLOAT);
	GLS_Assert(src1.glType() == GL_FLOAT);
	GlsMat _dst = GlsMat(src0.size(), src0.type());
	glslBinaryOperation(shaderBinaryOperation, src0.texid(), src1.texid(), _dst.texid(), OPCODE_MIN);
	dst = _dst;
}

void max(const vec4& scalar, const GlsMat& src, GlsMat& dst){
	GLS_Assert(src.glType() == GL_FLOAT);
	GlsMat _dst = GlsMat(src.size(), src.type());
	glslScalarOperation(shaderScalarOperation, scalar, src.texid(), _dst.texid(), OPCODE_MAX);
	dst = _dst;
}

void max(const GlsMat& src0, const GlsMat& src1, GlsMat& dst){
	GLS_Assert(src0.glType() == GL_FLOAT);
	GLS_Assert(src1.glType() == GL_FLOAT);
	GlsMat _dst = GlsMat(src0.size(), src0.type());
	glslBinaryOperation(shaderBinaryOperation, src0.texid(), src1.texid(), _dst.texid(), OPCODE_MAX);
	dst = _dst;
}

void mulSpectrums(const GlsMat& src0, const GlsMat& src1, GlsMat& dst, bool conj){
	GLS_Assert(src0.glSizedFormat() == GL_RG32F);
	GLS_Assert(src1.glSizedFormat() == GL_RG32F);
	GlsMat _dst = GlsMat(src0.size(), src0.type());
	if (conj)glslBinaryOperation(shaderBinaryOperation, src0.texid(), src1.texid(), _dst.texid(), OPCODE_MUL_SPCETRUM_CONJ);
	else glslBinaryOperation(shaderBinaryOperation, src0.texid(), src1.texid(), _dst.texid(), OPCODE_MUL_SPCETRUM);
	dst = _dst;
}

void mulSpectrumsPhaseOnly(const GlsMat& src0, const GlsMat& src1, GlsMat& dst){
	GLS_Assert(src0.glSizedFormat() == GL_RG32F);
	GLS_Assert(src1.glSizedFormat() == GL_RG32F);
	GlsMat _dst = GlsMat(src0.size(), src0.type());
	glslBinaryOperation(shaderBinaryOperation, src0.texid(), src1.texid(), _dst.texid(), OPCODE_MUL_SPCETRUM_POC);
	dst = _dst;
}

void magSpectrums(const GlsMat& src, GlsMat& dst){
	GLS_Assert(src.glSizedFormat() == GL_RG32F);
	vec4 scalar(1.0, 1.0, 1.0, 1.0);
	GlsMat _dst = GlsMat(src.size(), CV_MAKE_TYPE(src.depth(), 1));
	glslScalarOperation(shaderScalarOperation, scalar, src.texid(), _dst.texid(), OPCODE_MAG_SPCETRUM);
	dst = _dst;
}

void logMagSpectrums(const GlsMat& src, GlsMat& dst, float offset){
	GLS_Assert(src.glSizedFormat() == GL_RG32F);
	vec4 scalar(offset);
	GlsMat _dst = GlsMat(src.size(), CV_MAKE_TYPE(src.depth(), 1));
	glslScalarOperation(shaderScalarOperation, scalar, src.texid(), _dst.texid(), OPCODE_LOG_MAG_SPCETRUM);
	dst = _dst;
}


void log(const GlsMat& src, GlsMat& dst){
	GLS_Assert(src.glType() == GL_FLOAT);
	vec4 scalar(1.0, 1.0, 1.0, 1.0);
	GlsMat _dst = GlsMat(src.size(), src.type());
	glslScalarOperation(shaderScalarOperation, scalar, src.texid(), _dst.texid(), OPCODE_LOG);
	dst = _dst;
}

void exp(const GlsMat& src, GlsMat& dst){
	GLS_Assert(src.glType() == GL_FLOAT);
	vec4 scalar(1.0, 1.0, 1.0, 1.0);
	GlsMat _dst = GlsMat(src.size(), src.type());
	glslScalarOperation(shaderScalarOperation, scalar, src.texid(), _dst.texid(), OPCODE_EXP);
	dst = _dst;
}

void pow(const GlsMat& src, const float& power, GlsMat& dst){
	GLS_Assert(src.glType() == GL_FLOAT);
	vec4 scalar(power, power, power, power);
	GlsMat _dst = GlsMat(src.size(), src.type());
	glslScalarOperation(shaderScalarOperation, scalar, src.texid(), _dst.texid(), OPCODE_POW);
	dst = _dst;
}





}//namespace gls