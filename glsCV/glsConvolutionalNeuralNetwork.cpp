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
	lst.push_back("kSize");
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

class  glsShaderConvolutionalNeuralNetworkPacked : public glsShaderConvolutionalNeuralNetworkBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderConvolutionalNeuralNetworkPacked(void) :glsShaderConvolutionalNeuralNetworkBase(__FUNCTION__){}

};
glsShaderConvolutionalNeuralNetworkPacked ShaderConvolutionalNeuralNetworkPacked;

class  glsShaderConvolutionalNeuralNetworkPacked3x3 : public glsShaderConvolutionalNeuralNetworkBase
{
protected:
	string FragmentShaderCode(void);

public:
	glsShaderConvolutionalNeuralNetworkPacked3x3(void) :glsShaderConvolutionalNeuralNetworkBase(__FUNCTION__){}

};
glsShaderConvolutionalNeuralNetworkPacked3x3 ShaderConvolutionalNeuralNetworkPacked3x3;




string glsShaderConvolutionalNeuralNetwork::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core	\n
precision highp float;	\n
uniform sampler2DArray	texSrc; \n
uniform sampler2DArray	weights; \n
//layout(std140) uniform block{ \n
//float weights[4 * (128 / 4) * 5 * 5]; \n
//};	\n
//uniform float weights[4 * (128 / 4) * 5 * 5]; \n
uniform float bias; \n
//uniform ivec2 kSize; \n
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
	int j = 0; \n
	for (int i = 0; i < texSize.z; i++) { \n
		ivec3 uvt0 = ivec3(gl_FragCoord.xy, i); \n
		ivec3 uvt1 = ivec3(kxp, kyp, i); \n
		for (int ky = kym; ky <= kyp; ky++){	\n
			for (int kx = kxm; kx <= kxp; kx++){	\n
				ivec3 ofs = ivec3(kx, ky, 0); \n
				float data = texelFetch(texSrc, uvt0 + ofs, 0).r; \n
				float wei = texelFetch(weights, uvt1 + ofs, 0).r; \n
				//float  wei = weights[j]; \n
				j = j + 1; \n
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

string glsShaderConvolutionalNeuralNetworkPacked::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
#version 330 core	\n
precision highp float;	\n
uniform sampler2DArray	texSrc; \n
layout(std140) uniform block{ \n
	vec4 weights[4 * (128 / 4) * 3 * 3]; \n
};	\n
uniform vec4 bias; \n
uniform int srcChannels; \n
uniform int dstChannels; \n
uniform ivec2 kSize; \n
layout(location = 0) out vec4 dst; \n
void main()\n
	{ \n
	ivec3 texSize = textureSize(texSrc, 0); \n
	int kxsize = kSize.x; \n
	int kysize = kSize.y; \n
	int kxp = kxsize / 2; \n
	int kxm = -kxp; \n
	int kyp = kysize / 2; \n
	int kym = -kyp; \n
	// Convolution & Accumulate	\n
	vec4 s; \n
	int j = 0;\n
	for (int dCh = 0; dCh < dstChannels; dCh++) { \n
		vec4 accm = vec4(0.0); \n
		for (int i = 0; i < texSize.z; i++) { \n
			ivec3 uvt0 = ivec3(gl_FragCoord.xy, i); \n
			ivec3 uvt1 = ivec3(kxp, kyp, i + texSize.z *dCh); \n
			for (int ky = kym; ky <= kyp; ky++){	\n
				for (int kx = kxm; kx <= kxp; kx++){	\n
					vec4 data = texelFetch(texSrc, uvt0 + ivec3(kx, ky, 0), 0); \n
					vec4 wei = weights[j]; \n
					j = j + 1; \n
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

string glsShaderConvolutionalNeuralNetworkPacked3x3::FragmentShaderCode(void){
	const char fragmentShaderCode[] = TO_STR(
		#version 330 core	\n
		precision highp float;	\n
		uniform sampler2DArray	texSrc; \n
//		uniform vec4 weights[4 * (128 / 4) * 3 * 3]; \n
		layout(std140) uniform block{ \n
			vec4 weights[4 * (128 / 4) * 3 * 3]; \n
		};	\n
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
			vec4 wei;\n
			vec4 data;\n
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
			//vec4 data[9]; \n
			//data[0] = texelFetchOffset(texSrc, uvt0, 0, ivec2(-1, -1)); \n
			//data[1] = texelFetchOffset(texSrc, uvt0, 0, ivec2(+0, -1)); \n
			//data[2] = texelFetchOffset(texSrc, uvt0, 0, ivec2(+1, -1)); \n
			//data[3] = texelFetchOffset(texSrc, uvt0, 0, ivec2(-1, +0)); \n
			//data[4] = texelFetchOffset(texSrc, uvt0, 0, ivec2(+0, +0)); \n
			//data[5] = texelFetchOffset(texSrc, uvt0, 0, ivec2(+1, +0)); \n
			//data[6] = texelFetchOffset(texSrc, uvt0, 0, ivec2(-1, +1)); \n
			//data[7] = texelFetchOffset(texSrc, uvt0, 0, ivec2(+0, +1)); \n
			//data[8] = texelFetchOffset(texSrc, uvt0, 0, ivec2(+1, +1)); \n
			//for (int i = 0; i < 9; i++){  \n
			//	wei = weights[base + i]; \n
			//	accm += data[i]*wei; \n
			//} \n
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

//! packed format に変換
//!	CV_Assert(src.dims == 3); CV_Assert(src.channels() == 1);
//! 
//! srcPlanes / channels > 0 の時
//! [plns][rows][cols](1) => [plns/channels][rows][cols](channels)
//! srcPlanes / channels == 0 の時
//! [plns][rows][cols](1) => [plns][rows][cols](1)
template <typename T> static void pack(Mat& src, Mat& dst, int channels = 4){
	CV_Assert(src.dims == 3);
	CV_Assert(src.channels() == 1);

	const int srcPlanes = src.size[0];
	const Size srcSize(src.size[2], src.size[1]);
	const int srcChannels = src.channels();

	//		const int dstChannels = channels;
	const int dstChannels = srcPlanes / channels > 0 ? channels : srcChannels;
	const int dstPlanes = srcPlanes / dstChannels;
	const Size dstSize = srcSize;

	CV_Assert(srcPlanes % dstChannels == 0);

	const int sizes[3] = { dstPlanes, dstSize.height, dstSize.width };

	dst = Mat(3, sizes, CV_MAKETYPE(src.depth(), dstChannels));

	for (int i = 0; i < src.size[0]; i++){
		for (int j = 0; j < src.size[1]; j++){
			for (int k = 0; k < src.size[2]; k++){
				for (int cn = 0; cn < srcChannels; cn++){
					int _i = i / dstChannels;
					int _j = j;
					int _k = k;
					int _cn = i % dstChannels;
					*(dst.ptr<T>(_i, _j, _k) + _cn) = *(src.ptr<T>(i, j, k) + cn);
				}
			}
		}
	}
}

//! expand channels for 3d Mat
//!	CV_Assert(src.dims == 3); CV_Assert(src.channels() == 1);
//! [plns][rows][cols](1) => [plns/channels][rows][cols](channels)
template <typename T> static void expand(Mat& src, Mat& dst, int channels = 4){
	CV_Assert(src.dims == 3);
	CV_Assert(src.channels() == 1);

	const int srcPlanes = src.size[0];
	const Size srcSize(src.size[2], src.size[1]);
	const int srcChannels = src.channels();

	const int dstChannels = channels;
	const int dstPlanes = srcPlanes;
	const Size dstSize = srcSize;
	const int sizes[3] = { dstPlanes, dstSize.height, dstSize.width };

	dst = Mat(3, sizes, CV_MAKETYPE(src.depth(), dstChannels));

	for (int i = 0; i < src.size[0]; i++){
		for (int j = 0; j < src.size[1]; j++){
			for (int k = 0; k < src.size[2]; k++){
				for (int cn = 0; cn < srcChannels; cn++){
					int _i = i;
					int _j = j;
					int _k = k;
					int _cn = cn;
					*(dst.ptr<T>(_i, _j, _k) + _cn) = *(src.ptr<T>(i, j, k) + cn);
				}
			}
		}
	}
}

//! 
static glsShaderConvolutionalNeuralNetworkBase* selectShader(
	const Size kSize,
	const int n_inputChannels,
	const int n_outputChannels
	)
{
	bool isKernel3x3 = (kSize.width == 3 && kSize.height == 3);
	bool isPacked = !(n_inputChannels == 1 && n_outputChannels == 1);

	GLint max_fragment_uniform_vectors;
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &max_fragment_uniform_vectors);

	GLint max_uniform_block_size;	// in byte?
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_uniform_block_size);

	if (isPacked){
		GLS_Assert(max_uniform_block_size / 4 >= (4 * (128 / 4) * 3 * 3));
		if (isKernel3x3) {
			//cout << "use ShaderConvolutionalNeuralNetworkPacked3x3" << endl;
			return &ShaderConvolutionalNeuralNetworkPacked3x3;
		}
		else {
			//cout << "use ShaderConvolutionalNeuralNetworkPacked" << endl;
			return &ShaderConvolutionalNeuralNetworkPacked;
		}
	}
	else{
		if (isKernel3x3 && max_fragment_uniform_vectors >= 3 * 128) {
			//cout << "use ShaderConvolutionalNeuralNetwork3x3" << endl;
			return &ShaderConvolutionalNeuralNetwork3x3;
		}
		else{
			//cout << "use ShaderConvolutionalNeuralNetwork" << endl;
			return &ShaderConvolutionalNeuralNetwork;
		}
	}
	GLS_Assert(0 && "unreachable");
	return 0;
}


void convolutionalNeuralNetwork(
	GlsMat &inputPlanes,				///! input [planes][rows][cols]
	GlsMat &outputPlanes,				///! output [planes][rows][cols]
	cv::Mat &_weights,					///! kernels [inputPlanes*outputPlanes][ksize][ksize]
	std::vector<double>& biases,		///! bias [outputPlanes]
	bool outputPacked					///! output plane to be packed.
	)
{
	CV_Assert(inputPlanes.dims == 3);
//	CV_Assert(outputPlanes.dims == 3);
	CV_Assert(_weights.dims == 3);


	cv::Size ipSize = cv::Size(inputPlanes.size[2], inputPlanes.size[1]);
	cv::Size kSize = Size(_weights.size[2], _weights.size[1]);

	const int n_inputPlanes = inputPlanes.size[0];
	const int n_inputChannels = inputPlanes.channels();
	const int n_outputChannels = outputPacked ?
		(biases.size() / 4 > 0 ? 4 : 1) :
		1;
	const int n_outputPlanes = (int)biases.size() / n_outputChannels;


	CV_Assert(n_inputPlanes * n_inputChannels *
		n_outputPlanes * n_outputChannels
		== _weights.size[0] * _weights.channels());

	Mat weights;
	if (_weights.channels() == inputPlanes.channels()){
		weights = _weights;
	}
	else{
		//if inputPlanes is packed, pack weights.  
		pack<float>(_weights, weights, inputPlanes.channels());
	}

	{	//allocate output planes
		int _size[3] = { n_outputPlanes, ipSize.height, ipSize.width };
		outputPlanes = gls::GlsMat(3, _size, CV_MAKETYPE(inputPlanes.depth(), n_outputChannels));
	}

	const int n_weightPlanes = weights.size[0];
	const int n_weightChannels = weights.channels();
	CV_Assert(n_inputPlanes * n_inputChannels *
		n_outputPlanes * n_outputChannels
		== n_weightPlanes * n_weightChannels);

	int _kSize[3] = { n_inputPlanes * n_outputChannels, kSize.height, kSize.width };


	glsShaderConvolutionalNeuralNetworkBase* shader = 0;
	shader = selectShader(kSize, n_inputChannels, n_outputChannels);
	glUseProgram(shader->program());

	glsVAO vao(glGetAttribLocation(shader->program(), "position"));
	glsFBO fbo;		
	GLuint ubo = 0;
	glGenBuffers(1, &ubo);

	GlsMat kernelTextures(3, _kSize, weights.type());

	for (int opIndex = 0; opIndex < outputPlanes.size[0]; opIndex++) {
		cv::Mat kernels = cv::Mat(3, _kSize, weights.type(), weights.ptr<float>(_kSize[0] * opIndex));

		float bias[4];
		for (int i = 0; i < n_outputChannels; i++){
			bias[i] = (float)biases[n_outputChannels*opIndex+i];
		}

		if (shader == &ShaderConvolutionalNeuralNetwork){
			//setup kernel tex
			kernelTextures.upload(kernels);		//upload kernels to texture
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D_ARRAY, kernelTextures.texid());
			glUniform1i(shader->uniformLocArray[1], 1);
			//setup bias
			glUniform1f(shader->uniformLocArray[2], bias[0]);
			GL_CHECK_ERROR();

		}
		else if(shader == &ShaderConvolutionalNeuralNetwork3x3){
			//setup kernel 
			glUniform3fv(shader->uniformLocArray[1], 3 * inputPlanes.size[0], kernels.ptr<float>());
			//setup bias
			glUniform1f(shader->uniformLocArray[2], bias[0]);
		}
		else if (shader == &ShaderConvolutionalNeuralNetworkPacked ||
				shader == &ShaderConvolutionalNeuralNetworkPacked3x3){
			Mat _kernels;
			if (n_inputChannels == 4){ _kernels = kernels; }
			else{ expand<float>(kernels, _kernels, 4); }

			//setup kernel 
			const int uniformBlockBinding = 0;
			const size_t size_in_byte = _kernels.total() * _kernels.elemSize();

			glBindBuffer(GL_UNIFORM_BUFFER, ubo);
			glBufferData(GL_UNIFORM_BUFFER, size_in_byte, _kernels.data, GL_STATIC_DRAW);
			glUniformBlockBinding(shader->program(), glGetUniformBlockIndex(shader->program(), "block"), uniformBlockBinding);
			glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockBinding, ubo);
			glUniform2iv(shader->uniformLocArray[5], 1, &kSize.width);
			//setup bias
			glUniform4fv(shader->uniformLocArray[2], 1, bias);
			//setup input output channels
			glUniform1i(shader->uniformLocArray[3], n_inputChannels);
			glUniform1i(shader->uniformLocArray[4], n_outputChannels);
			GL_CHECK_ERROR();
		}
		else{
			GLS_Assert(0 && "unreachable");
		}

		//setup src tex
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, inputPlanes.texid());
		glUniform1i(shader->uniformLocArray[0], 0);

		//setup dest tex
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, outputPlanes.texid(), 0, opIndex);
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

	glDeleteBuffers(1, &ubo);

}



}//namespace