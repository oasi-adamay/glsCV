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
class glsShaderConvolutionalNeuralNetwork : public glsShaderBase
{
protected:
	string FragmentShaderCode(void);
	//	list<string> UniformNameList(void);

public:
	glsShaderConvolutionalNeuralNetwork(void) :glsShaderBase(__FUNCTION__){}

};

string glsShaderConvolutionalNeuralNetwork::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core	\n
precision highp float;	\n
uniform sampler2DArray	texSrc; \n
uniform sampler2DArray	texKernel; \n
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
		k0 = vec3(	\n
			texelFetchOffset(texKernel, uvt, 0, ivec2(-1, -1)).r, \n
			texelFetchOffset(texKernel, uvt, 0, ivec2( 0, -1)).r,\n
			texelFetchOffset(texKernel, uvt, 0, ivec2( 1, -1)).r); \n
		k1 = vec3(	\n
			texelFetchOffset(texKernel, uvt, 0, ivec2(-1, 0)).r,\n
			texelFetch(texKernel, uvt, 0).r,\n
			texelFetchOffset(texKernel, uvt, 0, ivec2( 1, 0)).r); \n
		k2 = vec3(	\n
			texelFetchOffset(texKernel, uvt, 0, ivec2(-1, 1)).r, \n
			texelFetchOffset(texKernel, uvt, 0, ivec2( 0, 1)).r,\n
			texelFetchOffset(texKernel, uvt, 0, ivec2( 1, 1)).r); \n
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

//-----------------------------------------------------------------------------
//global 
glsShaderConvolutionalNeuralNetwork ShaderConvolutionalNeuralNetwork;


void convolutionalNeuralNetwork(
	GlsMat &inputPlanes,				///! input [planes][rows][cols]
	GlsMat &outputPlanes,				///! output [planes][rows][cols]
	std::vector<cv::Mat>&weights,		///! kernels [inputPlanes*outputPlanes]([ksize][ksize])
	std::vector<double>& biases			///! bias [outputPlanes]
	)
{
	CV_Assert(inputPlanes.dims == 3);
	CV_Assert(outputPlanes.dims == 3);
	CV_Assert(inputPlanes.size[0] * outputPlanes.size[0] == weights.size());

	cv::Size ipSize = cv::Size(inputPlanes.size[2], inputPlanes.size[1]);

	glsShaderConvolutionalNeuralNetwork*shader = &ShaderConvolutionalNeuralNetwork;

	glUseProgram(shader->program());

	glsVAO vao(glGetAttribLocation(shader->program(), "position"));
	glsFBO fbo(1);	//create  & bind FBO


	for (int opIndex = 0; opIndex < outputPlanes.size[0]; opIndex++) {
		cv::Size kSize = weights[0].size();
		int _sz[3] = { inputPlanes.size[0], kSize.height, kSize.width };

		//密なkernel配列を作成
		cv::Mat kernels = cv::Mat(3, _sz, CV_32FC1);
		for (int ipIndex = 0; ipIndex < inputPlanes.size[0]; ipIndex++) {
			cv::Mat kernel = cv::Mat(kSize, CV_32FC1, kernels.ptr<float>(ipIndex));
			weights[inputPlanes.size[0] * opIndex + ipIndex].copyTo(kernel);
		}
		GlsMat kernelPlanes = (GlsMat)kernels;



		//setup dest tex
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, outputPlanes.texid(), 0, opIndex);

		//setup src tex
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, inputPlanes.texid());
		glUniform1i(glGetUniformLocation(shader->program(), "texSrc"), 0);		///TODO

		//setup kernel tex
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D_ARRAY, kernelPlanes.texid());
		glUniform1i(glGetUniformLocation(shader->program(), "texKernel"), 1);	///TODO

		//uniform
		shader->setUniform("bias", (float)biases[opIndex]);		///TODO

		shader->DrawBuffers(1);

		//Viewport
		glViewport(0, 0, ipSize.width, ipSize.height);

		//Render!!
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		//glFlush();
		glFinish();

	}

}


}//namespace