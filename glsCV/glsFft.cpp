#include "stdafx.h"


#include "glsFft.h"
#include "Timer.h"

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef DEBUG
#define _TMR_(...)  Timer tmr(__VA_ARGS__)
#else
#define _TMR_(...)
#endif


//-----------------------------------------------------------------------------
//global 
glsShaderFft* shaderFft = 0;
//glsShaderConj* shaderConj = 0;

void glsFftInit(void){
	shaderFft = new glsShaderFft();
//	shaderConj = new glsShaderConj();
}

void glsFftTerminate(void){
	delete shaderFft;
//	delete shaderConj;
}




//-----------------------------------------------------------------------------
//glsShaderFft
glsShaderFft::glsShaderFft(void)
	:glsShaderBase()
{

	const char vertexShaderCode[] = 
"#version 330 core\n"
"layout (location = 0)in  vec2 position;\n"
"void main(void)\n"
"{\n"
"   gl_Position  = vec4(position,0.0,1.0);\n"
"}\n"
;


	const char fragmentShaderCode[] = 
"#version 330 core\n"
"precision highp float;\n"
"uniform sampler2D	texSrc0;\n"
"uniform sampler2D	texSrc1;\n"
"uniform sampler2D	texW;\n"
"uniform  int i_flag;	//bit0:(0:holizontal 1:vertical)\n"
"uniform  int i_N;\n"
"uniform  int i_p;\n"
"uniform  int i_q;\n"
"uniform  float f_xscl;\n"
"uniform  float f_yscl;\n"
"uniform  float f_xconj;\n"
"uniform  float f_yconj;\n"
"\n"
"layout (location = 0) out vec2 dst0;\n"
"layout (location = 1) out vec2 dst1;\n"
"\n"
"#define FLAG_DIR	 (1<<0)\n"
"\n"
"\n"
"int insertZeroBits(\n"
"	const int src,\n"
"	const int idx,\n"
"	const int num\n"
"	)\n"
"{\n"
"	int ret = src << num;\n"
"	ret &= ~((1 << (idx + num)) - 1);\n"
"	ret |= src & ((1 << idx) - 1);\n"
"	return ret;\n"
"}\n"
"\n"
"void main(void)\n"
"{\n"
"	int p = i_p;\n"
"	int q = i_q;\n"
"	int N = i_N;\n"
"	int dir = ((i_flag & FLAG_DIR)==0) ?0:1;\n"
"	float xscl = f_xscl;\n"
"	float yscl = f_yscl;\n"
"	float xconj = f_xconj;\n"
"	float yconj = f_yconj;\n"
"\n"
"	int n;\n"
"	vec2 x0;\n"
"	vec2 x1;\n"
"	vec2 w;\n"
"\n"
"	n= int(gl_FragCoord[dir]);\n"
"	int iw = (n >> q) << q;\n"
"	int ix0 = insertZeroBits(n, q, 1);\n"
"	int ix1 = ix0 + (1 << q);\n"
"	w = texelFetch(texW,ivec2(iw,0),0).rg;\n"
"\n"
"\n"
"	if(dir ==0){\n"
"		if(ix0 < N/2) x0 = texelFetch(texSrc0,ivec2(ix0,gl_FragCoord.y),0).rg;\n"
"		else x0 = texelFetch(texSrc1,ivec2(ix0-N/2,gl_FragCoord.y),0).rg;\n"
"\n"
"		if(ix1 < N/2) x1 = texelFetch(texSrc0,ivec2(ix1,gl_FragCoord.y),0).rg;\n"
"		else x1 = texelFetch(texSrc1,ivec2(ix1-N/2,gl_FragCoord.y),0).rg;\n"
"	}\n"
"	else{\n"
"		if(ix0 < N/2) x0 = texelFetch(texSrc0,ivec2(gl_FragCoord.x,ix0),0).rg;\n"
"		else x0 = texelFetch(texSrc1,ivec2(gl_FragCoord.x,ix0-N/2),0).rg;\n"
"\n"
"		if(ix1 < N/2) x1 = texelFetch(texSrc0,ivec2(gl_FragCoord.x,ix1),0).rg;\n"
"		else x1 = texelFetch(texSrc1,ivec2(gl_FragCoord.x,ix1-N/2),0).rg;\n"
"	}\n"
"\n"
"//	x0 = x0*xscl;\n"
"//	x1 = x1*xscl;\n"
"	x0.g = x0.g*xconj;\n"
"	x1.g = x1.g*xconj;\n"
"\n"
"	vec2 tmp;\n"
"	tmp.r = x1.r * w.r - x1.g * w.g;\n"
"	tmp.g = x1.r * w.g + x1.g * w.r;\n"
"\n"
"	vec2 y0;\n"
"	vec2 y1;\n"
"\n"
"	y0 = x0 + tmp;\n"
"	y1 = x0 - tmp;\n"
"\n"
"	y0 = y0*yscl;\n"
"	y1 = y1*yscl;\n"
"	y0.g = y0.g*yconj;\n"
"	y1.g = y1.g*yconj;\n"
"\n"
"	dst0 = y0;\n"
"	dst1 = y1;\n"
"\n"
"}\n"
;
	
	

	// Create and compile our GLSL program from the shaders
//	LoadShadersFile("Fft_vs.glsl", "FftRadix2_fs.glsl");
	LoadShadersCode(vertexShaderCode, fragmentShaderCode);

	// Attribute & Uniform location
	position = glGetAttribLocation(program, "position");
	texSrc[0] = glGetUniformLocation(program, "texSrc0");
	texSrc[1] = glGetUniformLocation(program, "texSrc1");
	texW = glGetUniformLocation(program, "texW");
	i_p = glGetUniformLocation(program, "i_p");
	i_q = glGetUniformLocation(program, "i_q");
	i_N = glGetUniformLocation(program, "i_N");
	i_flag = glGetUniformLocation(program, "i_flag");
	f_xscl = glGetUniformLocation(program, "f_xscl");
	f_yscl = glGetUniformLocation(program, "f_yscl");
	f_xconj = glGetUniformLocation(program, "f_xconj");
	f_yconj = glGetUniformLocation(program, "f_yconj");
}





//---------------------------------------------------------------------------
//value is power of 2
static bool IsPow2(unsigned int x){
	return (((x)&(x - 1)) == 0);
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
static void glsFftProcess(
	const glsShaderFft* shader,	//progmra ID
	const vector<GLuint>& texSrc,	//src texture IDs
	const vector<GLuint>& texDst,	//dst texture IDs
	const GLuint texW,				//twidle texture
	const int flag,					//bit0 : horizontal:0 vertical:1
	const int p,					//p 
	const int q,					//q
	const int N,					//N
	const float xscl,				//xscl
	const float yscl,				//yscl
	const float xconj,				//xconj
	const float yconj				//yconj
)
{
	Size size = getTextureSize(texSrc[0]);
	int width = size.width;
	int height = size.height;

//	Timer tmr("glsFftProcess:\t");
	//program
	{
		glUseProgram(shader->program);
	}


	//uniform
	{
		glUniform1i(shader->i_flag, flag);
		glUniform1i(shader->i_p, p);
		glUniform1i(shader->i_q, q);
		glUniform1i(shader->i_N, N);
		glUniform1f(shader->f_xscl, xscl);
		glUniform1f(shader->f_yscl, yscl);
		glUniform1f(shader->f_xconj, xconj);
		glUniform1f(shader->f_yconj, yconj);
	}


	//Bind Texture
	{
		int id = 0;
		for (int i = 0; i < texSrc.size(); i++, id++){
			glActiveTexture(GL_TEXTURE0 + id);
			glBindTexture(GL_TEXTURE_2D, texSrc[i]);
			glUniform1i(shader->texSrc[i], id);
		}

		{
			glActiveTexture(GL_TEXTURE0 + id);
			glBindTexture(GL_TEXTURE_2D, texW);
			glUniform1i(shader->texW, id);
			id++;
		}
	}


	//dst texture
	{
		for (int i = 0; i < texDst.size(); i++){
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texDst[i], 0);
		}
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		GLenum bufs[] =
		{
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
		};
		glDrawBuffers(2, bufs);
	}
	

	//Viewport
	{
		glViewport(0, 0, width, height);
	}

	//Render!!
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glFlush();
	}

//	glFinish();



}







//-----------------------------------------------------------------------------
// execute FFT 
void glsFft(glsMat& texture, int flag){

	int N = texture.cols;
	assert(IsPow2(N));

	glsFBO fbo(2);
	glsVAO vao(shaderFft->position);

	glsMat texTmp(texture.size(), texture.type(), texture.blkNum());
	glsMat texW(Size(N / 2, 1), texture.type());

	//---------------------------------
	// upload twidle texture
	{
		_TMR_("-twidle:  \t");

		Mat w(Size(N / 2, 1), CV_32FC2);
		#ifdef _OPENMP
		#pragma omp parallel for
		#endif
		for (int n = 0; n < N / 2; n++){
			float jw = (float)(-2 * M_PI * n / N);
			Vec2f val(cos(jw), sin(jw));
			w.at<Vec2f>(0, n) = val;
		}
		texW.CopyFrom(w);


		//vector<vec2> w(N / 2);
		// --- twidle ----
		//#ifdef _OPENMP
		//#pragma omp parallel for
		//#endif
		//for (int n = 0; n < N / 2; n++){
		//	float jw = (float)(-2 * M_PI * n / N);
		//	w[n][0] = cos(jw);
		//	w[n][1] = sin(jw);
		//}
		//void* data = &w[0];

		//glBindTexture(GL_TEXTURE_2D, texW);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)w.size(), 1, texture.glFormat(), texture.glType(), data);
		//glBindTexture(GL_TEXTURE_2D, 0);
	}


	glsMat* texid[2] = { &texture, &texTmp };


	//Execute
	int bank = 0;

	{
		_TMR_("-execute:\t");

		int Q = 0;
		while ((1 << Q) < N){ Q++; }

		vector<GLuint> texSrc(2);
		vector<GLuint> texDst(2);

		// --- FFT rows ----
		for (int p = 0, q = Q - 1; q >= 0; p++, q--, bank = bank ^ 1) {
			for (int i = 0; i < 2; i++){
				for (int j = 0; j < 2; j++){
					texSrc[j] = texid[bank    ]->at(i, j);
					texDst[j] = texid[bank ^ 1]->at(i, j);
				}
				float yscl = ((flag & GLS_FFT_SCALE) && (q == 0)) ? 1.0f / (float)N : 1.0f;
				float xscl = 1.0f;
				float xconj = ((flag & GLS_FFT_INVERSE) && (p == 0)) ? -1.0f : 1.0f;
				float yconj = 1.0f;
				glsFftProcess(shaderFft, texSrc, texDst, texW.texArray[0], 0, p, q, N, xscl, yscl, xconj,yconj);
			}
		}
		// --- FFT cols ----
		for (int p = 0, q = Q - 1; q >= 0; p++, q--, bank = bank ^ 1) {
			for (int j = 0; j < 2; j++){
				for (int i = 0; i < 2; i++){
					texSrc[i] = texid[bank    ]->at(i, j);
					texDst[i] = texid[bank ^ 1]->at(i, j);
				}
				float yscl = ((flag & GLS_FFT_SCALE) && (q == 0)) ? 1.0f / (float)N : 1.0f;
				float xscl =  1.0f;
				float xconj = 1.0f;
				float yconj = ((flag & GLS_FFT_INVERSE) && (q == 0)) ? -1.0f : 1.0f;
				glsFftProcess(shaderFft, texSrc, texDst, texW.texArray[0], 1, p, q, N, xscl, yscl, xconj, yconj);
			}
		}
	}


}



void glsFft(const Mat& src, Mat& dst, int flag){
	CV_Assert(src.type() == CV_32FC2);
	CV_Assert(src.cols == src.rows);

	int N = src.cols;
	CV_Assert(IsPow2(N));

	glsMat texture(src.size(), src.type(), Size(2, 2));

	//---------------------------------
	//upload
	texture.CopyFrom(src);

	//---------------------------------
	//fft
	glsFft(texture, flag);

	//---------------------------------
	//download
	texture.CopyTo(dst);


}




//=============================================================================
glsShaderConj::glsShaderConj(void)
	:glsShaderBase()
{
#if 1
	const char vertexShaderCode[] = 
		"#version 330 core\n"
		"layout (location = 0)in  vec2 position;\n"
		"void main(void)\n"
		"{\n"
		"   gl_Position  = vec4(position,0.0,1.0);\n"
		"}\n"
		;

	const char fragmentShaderCode[] =
		"#version 330 core\n"
		"precision highp float;\n"
		"uniform sampler2D	texSrc;\n"
		"layout (location = 0) out vec2 dst;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	vec2 src = texelFetch(texSrc, ivec2(gl_FragCoord.xy),0).rg;\n"
		"	dst = vec2(src.r, -1.0*src.g);\n"
		"}\n"
		;

	LoadShadersCode(vertexShaderCode, fragmentShaderCode);


#else
	// Create and compile our GLSL program from the shaders
	LoadShadersFile("Fft_vs.glsl", "Conj_fs.glsl");
#endif

	// Attribute & Uniform location
	position = glGetAttribLocation(program, "position");

	texSrc = glGetUniformLocation(program, "texSrc");

}

static void glslConjProcess(
	const glsShaderConj* shader,	//progmra ID
	const vector<GLuint>& texSrc,	//src texture IDs
	const vector<GLuint>& texDst	//dst texture IDs
	)
{
	Size size = getTextureSize(texSrc[0]);
	int width = size.width;
	int height = size.height;

	//program
	{
		glUseProgram(shader->program);
	}

	//uniform
	{
	}

	for (int i = 0; i < texSrc.size(); i++){
		//Bind Texture
		{
			int id = 0;
			glActiveTexture(GL_TEXTURE0 + id);
			glBindTexture(GL_TEXTURE_2D, texSrc[i]);
			glUniform1i(shader->texSrc, id);
		}
		//dst texture
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texDst[i], 0);
			assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
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
	}
}



