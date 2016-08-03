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
@file	glsConvolutionalNeuralNetwork.hpp
@brief	convolutional neural network
@author	oasi-adamay	
 */


#ifndef GLS_CONVOLUTIONAL_NEURAL_NETWORK_HPP_
#define GLS_CONVOLUTIONAL_NEURAL_NETWORK_HPP_

#include <opencv2/opencv.hpp>


namespace gls {

void convolutionalNeuralNetwork(
	GlsMat &inputPlanes,				///! input [planes][rows][cols]
	GlsMat &outputPlanes,				///! output [planes][rows][cols]
	cv::Mat &weights,					///! kernels [inputPlanes*outputPlanes][ksize][ksize]
	std::vector<double>& biases			///! bias [outputPlanes]
	);


}
#endif /* GLS_CONVOLUTIONAL_NEURAL_NETWORK_HPP_ */
