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
		ivec3 uvt = ivec3(gl_FragCoord.xy, 0); \n
		for (int ky = kym; ky <= kyp; ky++){	\n
			for (int kx = kxm; kx <= kxp; kx++){	\n
				float data = texelFetch(texSrc, uvt + ivec3(kx,ky,i), 0).r; \n
				float wei = texelFetch(weights, ivec3(kx+kxp, ky+kyp, i), 0).r; \n
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
	CV_Assert(inputPlanes.size[0] * outputPlanes.size[0] == weights.size[0]);

	cv::Size ipSize = cv::Size(inputPlanes.size[2], inputPlanes.size[1]);
	cv::Size kSize = Size(weights.size[2], weights.size[1]);
	int _kSize[3] = { inputPlanes.size[0], kSize.height, kSize.width };


	GLint max_fragment_uniform_vectors;
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &max_fragment_uniform_vectors);

	glsShaderConvolutionalNeuralNetworkBase* shader;

	if (kSize.width == 3 && kSize.height == 3
		&& max_fragment_uniform_vectors > (3*128+2)){
		//kernel size == 3x3 で、uniform変数の割り当てられるならば、特殊化したシェーダを利用
		shader = &ShaderConvolutionalNeuralNetwork3x3;
	}
	else{
		shader = &ShaderConvolutionalNeuralNetwork;
	}

	glUseProgram(shader->program());

	glsVAO vao(glGetAttribLocation(shader->program(), "position"));
	glsFBO fbo(1);	//create  & bind FBO
	GlsMat kernelPlanes(3, _kSize ,CV_32FC1);

	for (int opIndex = 0; opIndex < outputPlanes.size[0]; opIndex++) {
		cv::Mat kernels = cv::Mat(3, _kSize, CV_32FC1, weights.ptr<float>(inputPlanes.size[0] * opIndex));
		if (shader == &ShaderConvolutionalNeuralNetwork){
			kernelPlanes.upload(kernels);		//upload kernels to texture
		}

		//setup dest tex
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, outputPlanes.texid(), 0, opIndex);

		//setup src tex
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, inputPlanes.texid());
		glUniform1i(shader->uniformLocArray[0], 0);

		if (shader == &ShaderConvolutionalNeuralNetwork){
			//setup kernel tex
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D_ARRAY, kernelPlanes.texid());
			glUniform1i(shader->uniformLocArray[1], 1);
		}
		else{
			glUniform3fv(shader->uniformLocArray[1], 3 * inputPlanes.size[0], kernels.ptr<float>());
		}

		//uniform
		glUniform1f(shader->uniformLocArray[2], (float)biases[opIndex]);

		shader->DrawBuffers(1);

		//Viewport
		glViewport(0, 0, ipSize.width, ipSize.height);

		//Render!!
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glFlush();
		//glFinish();

	}
	glFinish();

}


}//namespace