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
@file	convolutionalNeuralNetwork.cpp
@brief	convolutional neural network
@author	oasi-adamay	
 */

#include "convolutionalNeuralNetwork.hpp"

namespace CNN {

void ReLU_accumlate_filter2D(
	cv::Mat &inputPlanes,				// input [planes][rows][cols]
	cv::Mat &outputPlane,				// output [rows][cols]
	cv::Mat &kernels,					// kernels [planes][ksize][ksize]
	const float bias					// bias
	)
{
	CV_Assert(inputPlanes.dims == 3);
	CV_Assert(kernels.dims == 3);
	CV_Assert(outputPlane.dims == 2);
	CV_Assert(inputPlanes.size[0] == kernels.size[0]);


	cv::Size ipSize = cv::Size(inputPlanes.size[2], inputPlanes.size[1]);	//input image size
	cv::Size kSize = cv::Size(kernels.size[2], kernels.size[1]);			//kernel size
	cv::Mat uIntermediatePlane = cv::Mat::zeros(ipSize, CV_32FC1);			//intermediate

	//2D-convolution and accumlate
	// output = Σ (input(k) * kernel(k))
	for (int ipIndex = 0; ipIndex < inputPlanes.size[0]; ipIndex++) {
		cv::Mat inputPlane = cv::Mat(ipSize, CV_32FC1, inputPlanes.ptr<float>(ipIndex));
		cv::Mat kernel = cv::Mat(kSize, CV_32FC1, kernels.ptr<float>(ipIndex));
		cv::Mat filterOutput;
//		cv::filter2D(inputPlane, filterOutput, -1, kernel, cv::Point(-1, -1), 0.0, cv::BORDER_REPLICATE);
		cv::filter2D(inputPlane, filterOutput, -1, kernel, cv::Point(-1, -1), 0.0, cv::BORDER_CONSTANT);	//compatibility for gls::convolutionalNeuralNetwork
		
		//		cv::add(uIntermediatePlane, filterOutput, uIntermediatePlane);
		cv::accumulate(filterOutput, uIntermediatePlane);

		//std::cout << "i:" << ipIndex << "\to:" << opIndex << std::endl;
		//std::cout << kernel << std::endl;
	}

	//bias
	cv::add(uIntermediatePlane, bias, uIntermediatePlane);

#if 1
	//activation function (ReLU - Rectified Linear Unit)
	uIntermediatePlane = cv::max(uIntermediatePlane*0.1, uIntermediatePlane);
#else
	cv::Mat moreThanZero = cv::Mat(ipSize, CV_32FC1, 0.0);
	cv::Mat lessThanZero = cv::Mat(ipSize, CV_32FC1, 0.0);
	cv::max(uIntermediatePlane, 0.0, moreThanZero);
	cv::min(uIntermediatePlane, 0.0, lessThanZero);
	cv::scaleAdd(lessThanZero, 0.1, moreThanZero, uIntermediatePlane);
#endif

	uIntermediatePlane.copyTo(outputPlane);

}

void convolutionalNeuralNetwork(
	cv::Mat &inputPlanes,				///! input [planes][rows][cols]
	cv::Mat &outputPlanes,				///! output [planes][rows][cols]
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


#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int opIndex = 0; opIndex < outputPlanes.size[0]; opIndex++) {
		int _sz[3] = { inputPlanes.size[0], kSize.height, kSize.width };
		cv::Mat kernels = cv::Mat(3, _sz, CV_32FC1, weights.ptr<float>(inputPlanes.size[0] * opIndex));
		cv::Mat outputPlane = cv::Mat(ipSize, CV_32FC1, outputPlanes.ptr<float>(opIndex));
		ReLU_accumlate_filter2D(inputPlanes, outputPlane, kernels, (float)biases[opIndex]);
	}

}




}
