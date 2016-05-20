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
#include "glsShader.h"

#include "glsDraw.h"

namespace gls
{

//-----------------------------------------------------------------------------
// glsDraw shader
class glsShaderDrawBase : public glsShaderBase
{

public:
	glsShaderDrawBase(const string& _name) :glsShaderBase(_name){}

};


//-----------------------------------------------------------------------------
// glsDraw shader
class glsShaderDraw : public glsShaderDrawBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderDraw(void) :glsShaderDrawBase(__FUNCTION__){}

};

//-----------------------------------------------------------------------------
// glsDraw shader
class glsShaderDrawU : public glsShaderDrawBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderDrawU(void) :glsShaderDrawBase(__FUNCTION__){}

};



//-----------------------------------------------------------------------------
//global 
glsShaderDraw ShaderDraw;
glsShaderDrawU ShaderDrawU;


//-----------------------------------------------------------------------------
//glsShaderDraw
string glsShaderDraw::FragmentShaderCode(void){
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform sampler2D	texSrc;\n"
"uniform float	scl;\n"
"uniform int 	flag;\n"
"\n"
"layout (location = 0) out vec4 dst;\n"
"\n"
"void main(void)\n"
"{\n"
"	ivec2 texSize = textureSize(texSrc,0);\n"
"	ivec2 texCord = ivec2(gl_FragCoord.xy)%texSize;\n"
"	vec4 data = texelFetch(texSrc, texCord,0);\n"
"	vec4 color;\n"
"	switch(flag&3){\n"
"	case(0):color = vec4(data.r*scl,data.g*scl,data.b*scl,data.g*scl);break;\n"
"	case(1):color = vec4(data.r*scl,data.r*scl,data.r*scl,1.0);break;\n"
"	case(2):color = vec4(data.r*scl,data.r*scl,data.r*scl,1.0);break;\n"
"	case(3):color = vec4(data.r*scl,data.g*scl,data.b*scl,1.0);break;\n"
"	}\n"
"	dst = color;\n"
"\n"
"}\n"
;
	return fragmentShaderCode;
}



//-----------------------------------------------------------------------------
//glsShaderDrawU
string glsShaderDrawU::FragmentShaderCode(void){
	const char fragmentShaderCode[] =
"#version 330 core\n"
"precision highp float;\n"
"uniform usampler2D	texSrc;\n"
"uniform float	scl;\n"
"uniform int 	flag;\n"
"\n"
"layout (location = 0) out vec4 dst;\n"
"\n"
"void main(void)\n"
"{\n"
"	ivec2 texSize = textureSize(texSrc,0);\n"
"	ivec2 texCord = ivec2(gl_FragCoord.xy)%texSize;\n"
"	vec4 data = vec4(texelFetch(texSrc, texCord,0));\n"
"	vec4 color;\n"
"	switch(flag&3){\n"
"	case(0):color = vec4(data.r*scl,data.g*scl,data.b*scl,data.g*scl);break;\n"
"	case(1):color = vec4(data.r*scl,data.r*scl,data.r*scl,1.0);break;\n"
"	case(2):color = vec4(data.r*scl,data.r*scl,data.r*scl,1.0);break;\n"
"	case(3):color = vec4(data.r*scl,data.g*scl,data.b*scl,1.0);break;\n"
"	}\n"
"	dst = color;\n"
"}\n"
;
	return fragmentShaderCode;
}


static Size getTextureSize(GLuint tex){
	int width;
	int height;

	//get texture size

	glBindTexture(GL_TEXTURE_2D, tex);
	glGetTexLevelParameteriv(
		GL_TEXTURE_2D, 0,
		GL_TEXTURE_WIDTH, &width
		);

	glGetTexLevelParameteriv(
		GL_TEXTURE_2D, 0,
		GL_TEXTURE_HEIGHT, &height
		);

	glBindTexture(GL_TEXTURE_2D, 0);

	return Size(width, height);
}




//---------------------------------------------------------------------------
//
static void glsDrawProcess(
	const glsShaderDrawBase* shader,	//progmra ID
	const GLuint& texSrc,				//src texture IDs
	const Rect  rect,					//src rect
	const Size  sizeDst,				//dst size
	const float scl,					// scaling 
	const int   flag					// flag [1:0] channnel 
	)
{
	int width = sizeDst.width;
	int height = sizeDst.height;

	//program
	{
		glUseProgram(shader->program());
	}

	//uniform
	{
		glUniform1f(glGetUniformLocation(shader->program(), "scl"), scl);
		glUniform1i(glGetUniformLocation(shader->program(), "flag"), flag);
	}


	//Bind Texture
	{
		int id = 0;
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, texSrc);
		glUniform1i(glGetUniformLocation(shader->program(),"texSrc"), id);
	}

	glsVAO vao(glGetAttribLocation(shader->program(), "position"));
	

	//Viewport
	{
//		glViewport(0, 0, width, height);
		glViewport(rect.x, rect.y, rect.width, rect.height);
	}

	//Render!!
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glFlush();
	}

	//	glFinish();

}

//-----------------------------------------------------------------------------
void draw(GlsMat& src){
	// Clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glsShaderDrawBase* shader = 0;
	float scl = 0;
	int flag = 0;

	switch (src.glType()){
	case(GL_FLOAT) :
		scl = 1.0f;
		shader = &ShaderDraw;
		break;
	case(GL_UNSIGNED_BYTE) :
		scl = 1.0f / 256.0f;
		shader = &ShaderDrawU;
		break;
	case(GL_UNSIGNED_SHORT) :
		scl = 1.0f / 65536.0f;
		shader = &ShaderDrawU;
		break;
	case(GL_UNSIGNED_INT) :
		scl = 1.0f / (65536.0f * 65536.0f);
		shader = &ShaderDrawU;
		break;
	//case(GL_BYTE) :
	//case(GL_SHORT) :
	//case(GL_INT) : shader = &ShaderDrawI; break;
	default: GLS_Assert(0);		//not implement
	}

	switch (src.glFormat()){
	case(GL_RED) :
	case(GL_RED_INTEGER) :
		flag |= 1;
		break;
	case(GL_RG) :
	case(GL_RG_INTEGER) :
	    flag |= 2;
		break;
	case(GL_RGB) :
	case(GL_RGB_INTEGER) :
		flag |= 3;
		break;
	case(GL_RGBA) :
	case(GL_RGBA_INTEGER) :
		break;
	default: GLS_Assert(0);		//not implement
	}

	Size sizeDst(src.size());

	Rect rect(0, 0, src.cols, src.rows);
	glsDrawProcess(shader, src.texid(), rect, sizeDst, scl, flag);


}

}//namespace gls



