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

/*!
@file	glsConvolutionalNeuralNetwork.cpp
@brief	convolutional neural network
@author	oasi-adamay	
 */

#include "stdafx.h"

/*-----------------------------------------------------------------------------
include
*/
#include "glsMacro.h"
#include "GlsMat.h"
#include "glsShader.h"
#include "glsConvolutionalNeuralNetwork.hpp"

namespace gls {



// glsConvolutionalNeuralNetwork shader
class glsShaderConvolutionalNeuralNetworkBase : public glsShaderBase
{
protected:
	list<string> UniformNameList(void);

public:
	glsShaderConvolutionalNeuralNetworkBase(const string& _name) :glsShaderBase(_name){}
};

list<string> glsShaderConvolutionalNeuralNetworkBase::UniformNameList(void){
	list<string> lst;
	lst.push_back("texSrc");
	lst.push_back("weights");
	lst.push_back("bias");
	lst.push_back("srcChannels");
	lst.push_back("dstChannels");
	return lst;
}

class  glsShaderConvolutionalNeuralNetwork : public glsShaderConvolutionalNeuralNetworkBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderConvolutionalNeuralNetwork(void) :glsShaderConvolutionalNeuralNetworkBase(__FUNCTION__){}

};
glsShaderConvolutionalNeuralNetwork ShaderConvolutionalNeuralNetwork;


class  glsShaderConvolutionalNeuralNetwork3x3 : public glsShaderConvolutionalNeuralNetworkBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderConvolutionalNeuralNetwork3x3(void) :glsShaderConvolutionalNeuralNetworkBase(__FUNCTION__){}
};
glsShaderConvolutionalNeuralNetwork3x3 ShaderConvolutionalNeuralNetwork3x3;

class  glsShaderConvolutionalNeuralNetworkVec4 : public glsShaderConvolutionalNeuralNetworkBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderConvolutionalNeuralNetworkVec4(void) :glsShaderConvolutionalNeuralNetworkBase(__FUNCTION__){}

};
glsShaderConvolutionalNeuralNetworkVec4 ShaderConvolutionalNeuralNetworkVec4;

class  glsShaderConvolutionalNeuralNetwork3x3Vec4 : public glsShaderConvolutionalNeuralNetworkBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderConvolutionalNeuralNetwork3x3Vec4(void) :glsShaderConvolutionalNeuralNetworkBase(__FUNCTION__){}

};
glsShaderConvolutionalNeuralNetwork3x3Vec4 ShaderConvolutionalNeuralNetwork3x3Vec4;




string glsShaderConvolutionalNeuralNetwork::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core	\n
precision highp float;	\n
uniform sampler2DArray	texSrc; \n
uniform sampler2DArray	weights; \n
uniform float bias; \n
layout(location = 0) out float dst; \n
void main()\n
{\n
	ivec3 texSize = textureSize(texSrc, 0); \n
	ivec3 kSize = textureSize(weights, 0); \n
	int kxsize = kSize.x; \n
	int kysize = kSize.y; \n
	int kxp = kxsize / 2; \n
	int kxm = -kxp; \n
	int kyp = kysize / 2; \n
	int kym = -kyp; \n

	// Convolution & Accumulate	\n
	float s = 0.0; \n
	for (int i = 0; i < texSize.z; i++) { \n
		ivec3 uvt0 = ivec3(gl_FragCoord.xy, i); \n
		ivec3 uvt1 = ivec3(kxp, kyp, i); \n
		for (int ky = kym; ky <= kyp; ky++){	\n
			for (int kx = kxm; kx <= kxp; kx++){	\n
				ivec3 ofs = ivec3(kx, ky, 0); \n
				float data = texelFetch(texSrc, uvt0 + ofs, 0).r; \n
				float wei = texelFetch(weights, uvt1 + ofs, 0).r; \n
				s += data*wei; \n
			} \n
		} \n
	}\n
	// Leaky ReLU\n
	s += bias; \n
	s = max(s, 0) + min(s, 0) * 0.1; \n
	dst = s; \n
}\n
);
	return fragmentShaderCode;
}

string glsShaderConvolutionalNeuralNetwork3x3::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core	\n
precision highp float;	\n
uniform sampler2DArray	texSrc; \n
uniform vec3	weights[3 * 128]; \n
uniform float bias; \n
layout(location = 0) out float dst; \n
void main()\n
{\n
	ivec3 texSize = textureSize(texSrc, 0); \n
		// Convolution & Accumulate	\n
		float s = 0.0; \n
	for (int i = 0; i < texSize.z; i++) { \n
		vec3 t0, t1, t2; \n
		vec3 k0, k1, k2; \n
		ivec3 uvt = ivec3(gl_FragCoord.xy, i); \n
		t0 = vec3(	\n
			texelFetchOffset(texSrc, uvt, 0, ivec2(-1, -1)).r, \n
			texelFetchOffset(texSrc, uvt, 0, ivec2( 0, -1)).r,\n
			texelFetchOffset(texSrc, uvt, 0, ivec2( 1, -1)).r); \n
		t1 = vec3(	\n
			texelFetchOffset(texSrc, uvt, 0, ivec2(-1, 0)).r, \n
			      texelFetch(texSrc, uvt, 0).r,\n
			texelFetchOffset(texSrc, uvt, 0, ivec2( 1, 0)).r); \n
		t2 = vec3(	\n
			texelFetchOffset(texSrc, uvt, 0, ivec2(-1, 1)).r,\n
			texelFetchOffset(texSrc, uvt, 0, ivec2( 0, 1)).r,\n
			texelFetchOffset(texSrc, uvt, 0, ivec2( 1, 1)).r); \n
		uvt = ivec3(1, 1, i);\n
		k0 = weights[i * 3 + 0];	\n
		k1 = weights[i * 3 + 1];	\n
		k2 = weights[i * 3 + 2];	\n
		s += dot(t0, k0) + dot(t1, k1) + dot(t2, k2);\n
	}\n
	// Leaky ReLU\n
	s += bias; \n
	s = max(s, 0) + min(s, 0) * 0.1; \n
	dst = s; \n
}\n
);
	return fragmentShaderCode;
}

string glsShaderConvolutionalNeuralNetworkVec4::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
		#version 330 core	\n
		precision highp float;	\n
		uniform sampler2DArray	texSrc; \n
		uniform sampler2DArray	weights; \n
		uniform vec4 bias; \n
		uniform int srcChannels; \n
		uniform int dstChannels; \n
		layout(location = 0) out vec4 dst; \n
		void main()\n
	{ \n
	ivec3 texSize = textureSize(texSrc, 0); \n
	ivec3 kSize = textureSize(weights, 0); \n
	int kxsize = kSize.x; \n
	int kysize = kSize.y; \n
	int kxp = kxsize / 2; \n
	int kxm = -kxp; \n
	int kyp = kysize / 2; \n
	int kym = -kyp; \n

	// Convolution & Accumulate	\n
	vec4 s; \n
	for (int dCh = 0; dCh < dstChannels; dCh++) { \n
		vec4 accm = vec4(0.0); \n
		for (int i = 0; i < texSize.z; i++) { \n
			ivec3 uvt0 = ivec3(gl_FragCoord.xy, i); \n
			ivec3 uvt1 = ivec3(kxp, kyp, i + texSize.z *dCh); \n
			for (int ky = kym; ky <= kyp; ky++){	\n
				for (int kx = kxm; kx <= kxp; kx++){	\n
					vec4 data = texelFetch(texSrc, uvt0 + ivec3(kx, ky, 0), 0); \n
					vec4 wei = texelFetch(weights, uvt1 + ivec3(kx, ky, 0), 0); \n
					accm += data*wei; \n
				} \n
			} \n
		}\n
		float _s = 0.0;	\n 
		for (int sCh = 0; sCh < srcChannels; sCh++) { \n
			_s += accm[sCh];	\n
		}	\n
		s[dCh] = _s;
	}\n
	// Leaky ReLU\n
	s += bias; \n
	s = max(s, 0) + min(s, 0) * 0.1; \n
	dst = s; \n
}\n
);
	return fragmentShaderCode;
}

string glsShaderConvolutionalNeuralNetwork3x3Vec4::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
		#version 330 core	\n
		precision highp float;	\n
		uniform sampler2DArray	texSrc; \n
//		uniform vec4 weights[4 * (128 / 4) * 3 * 3]; \n
		uniform vec4 weights[1022]; \n
		uniform vec4 bias; \n
		uniform int srcChannels; \n
		uniform int dstChannels; \n
		layout(location = 0) out vec4 dst; \n
		void main()\n
	{ \n
	ivec3 texSize = textureSize(texSrc, 0); \n

	// Convolution & Accumulate	\n
	vec4 s; \n
	for (int dCh = 0; dCh < dstChannels; dCh++) { \n
		vec4 accm = vec4(0.0); \n
		for (int i = 0; i < texSize.z; i++) { \n
			ivec3 uvt0 = ivec3(gl_FragCoord.xy, i); \n
			int base = dCh * (texSize.z * 3 * 3) + i*(3*3);\n
			vec4 data;\n
			vec4 wei;\n
			data = texelFetchOffset(texSrc, uvt0, 0, ivec2(-1, -1)); \n
			wei = weights[base + 0]; \n
			accm += data*wei; \n
			data = texelFetchOffset(texSrc, uvt0, 0, ivec2(+0, -1)); \n
			wei = weights[base + 1]; \n
			accm += data*wei; \n
			data = texelFetchOffset(texSrc, uvt0, 0, ivec2(+1, -1)); \n
			wei = weights[base + 2]; \n
			accm += data*wei; \n
			data = texelFetchOffset(texSrc, uvt0, 0, ivec2(-1, +0)); \n
			wei = weights[base + 3]; \n
			accm += data*wei; \n
			data = texelFetchOffset(texSrc, uvt0, 0, ivec2(+0, +0)); \n
			wei = weights[base + 4]; \n
			accm += data*wei; \n
			data = texelFetchOffset(texSrc, uvt0, 0, ivec2(+1, +0)); \n
			wei = weights[base + 5]; \n
			accm += data*wei; \n
			data = texelFetchOffset(texSrc, uvt0, 0, ivec2(-1, +1)); \n
			wei = weights[base + 6]; \n
			accm += data*wei; \n
			data = texelFetchOffset(texSrc, uvt0, 0, ivec2(+0, +1)); \n
			wei = weights[base + 7]; \n
			accm += data*wei; \n
			data = texelFetchOffset(texSrc, uvt0, 0, ivec2(+1, +1)); \n
			wei = weights[base + 8]; \n
			accm += data*wei; \n
		}\n
		float _s = 0.0;	\n 
		for (int sCh = 0; sCh < srcChannels; sCh++) { \n
			_s += accm[sCh];	\n
		}	\n
		s[dCh] = _s;
	}\n
	// Leaky ReLU\n
	s += bias; \n
	s = max(s, 0) + min(s, 0) * 0.1; \n
	dst = s; \n
}\n
);
	return fragmentShaderCode;
}



void convolutionalNeuralNetwork(
	GlsMat &inputPlanes,				///! input [planes][rows][cols]
	GlsMat &outputPlanes,				///! output [planes][rows][cols]
	cv::Mat &weights,					///! kernels [inputPlanes*outputPlanes][ksize][ksize]
	std::vector<double>& biases			///! bias [outputPlanes]
	)
{
	CV_Assert(inputPlanes.dims == 3);
	CV_Assert(outputPlanes.dims == 3);
	CV_Assert(weights.dims == 3);
	const int n_inputPlanes = inputPlanes.size[0];
	const int n_inputChannels = inputPlanes.channels();
	const int n_outputPlanes = outputPlanes.size[0];
	const int n_outputChannels = outputPlanes.channels();
	const int n_weightPlanes = weights.size[0];
	const int n_weightChannels = weights.channels();
	CV_Assert(n_inputPlanes * n_inputChannels *
		n_outputPlanes * n_outputChannels
		== n_weightPlanes * n_weightChannels);

	cv::Size ipSize = cv::Size(inputPlanes.size[2], inputPlanes.size[1]);
	cv::Size kSize = Size(weights.size[2], weights.size[1]);
	int _kSize[3] = { n_inputPlanes * n_outputChannels, kSize.height, kSize.width };

	GLint max_fragment_uniform_vectors;
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &max_fragment_uniform_vectors);

	GLint max_uniform_block_size;	// in byte?
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_uniform_block_size);
	

	glsShaderConvolutionalNeuralNetworkBase* shader = 0;
	if (n_inputChannels == 1 && n_outputChannels == 1){
		if (kSize.width == 3 && kSize.height == 3
			&& max_fragment_uniform_vectors > (3 * 128 + 16)){
			//kernel size == 3x3 で、uniform変数の割り当てられるならば、特殊化したシェーダを利用
			shader = &ShaderConvolutionalNeuralNetwork3x3;
		}
		else{
			shader = &ShaderConvolutionalNeuralNetwork;
		}
	}
	else{
		if (kSize.width == 3 && kSize.height == 3 
			&& n_inputChannels != 1 
			&& n_inputPlanes * n_outputChannels *3*3 <1020){
//			cout << "use ShaderConvolutionalNeuralNetwork3x3Vec4" << endl;
			shader = &ShaderConvolutionalNeuralNetwork3x3Vec4;
//			shader = &ShaderConvolutionalNeuralNetworkVec4;
		}
		else{
//			cout << "use ShaderConvolutionalNeuralNetworkVec4" << endl;
			shader = &ShaderConvolutionalNeuralNetworkVec4;
		}
	}

	glUseProgram(shader->program());

	glsVAO vao(glGetAttribLocation(shader->program(), "position"));
	glsFBO fbo;		//create  & bind FBO
	GlsMat kernelPlanes(3, _kSize, weights.type());

	for (int opIndex = 0; opIndex < outputPlanes.size[0]; opIndex++) {
		cv::Mat kernels = cv::Mat(3, _kSize, weights.type(), weights.ptr<float>(_kSize[0] * opIndex));
		if (shader == &ShaderConvolutionalNeuralNetwork ||
			shader == &ShaderConvolutionalNeuralNetworkVec4	){
			kernelPlanes.upload(kernels);		//upload kernels to texture
		}

		//setup dest tex
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, outputPlanes.texid(), 0, opIndex);

		//setup src tex
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, inputPlanes.texid());
		glUniform1i(shader->uniformLocArray[0], 0);

		float bias[4];
		for (int i = 0; i < n_outputChannels; i++){
			bias[i] = (float)biases[n_outputChannels*opIndex+i];
		}

		if (shader == &ShaderConvolutionalNeuralNetwork){
			//setup kernel tex
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D_ARRAY, kernelPlanes.texid());
			glUniform1i(shader->uniformLocArray[1], 1);
			glUniform1f(shader->uniformLocArray[2], bias[0]);
		}
		else if(shader == &ShaderConvolutionalNeuralNetwork3x3){
			glUniform3fv(shader->uniformLocArray[1], 3 * inputPlanes.size[0], kernels.ptr<float>());
			glUniform1f(shader->uniformLocArray[2], bias[0]);
		}
		else if (shader == &ShaderConvolutionalNeuralNetworkVec4){
			//setup kernel tex
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D_ARRAY, kernelPlanes.texid());

			glUniform1i(shader->uniformLocArray[1], 1);
			glUniform4fv(shader->uniformLocArray[2], 1,bias);
			glUniform1i(shader->uniformLocArray[3], n_inputChannels);
			glUniform1i(shader->uniformLocArray[4], n_outputChannels);

		}
		else if (shader == &ShaderConvolutionalNeuralNetwork3x3Vec4){
			if (n_inputChannels==1){
				glUniform1fv(shader->uniformLocArray[1], 3 * 3 * n_outputChannels * inputPlanes.size[0], kernels.ptr<float>());
			}
			else{
				glUniform4fv(shader->uniformLocArray[1], 3 * 3 * n_outputChannels * inputPlanes.size[0], kernels.ptr<float>());
			}
			glUniform4fv(shader->uniformLocArray[2], 1, bias);
			glUniform1i(shader->uniformLocArray[3], n_inputChannels);
			glUniform1i(shader->uniformLocArray[4], n_outputChannels);
		}
		else{
			GLS_Assert(0 && "unreachable");
		}


		shader->DrawBuffers(1);

		//Viewport
		glViewport(0, 0, ipSize.width, ipSize.height);

		//Render!!
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glFlush();
		//glFinish();

	}
	glFinish();
	GL_CHECK_ERROR();
}


}//namespace