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
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


#include "glsCV.h"
#include "UnitTest_Common.h"
#include "..\waifu2x-converter-gls\src\convolutionalNeuralNetwork.hpp"



namespace UnitTest_glsCV
{

	template <typename T>
	int test_glsConvolutionalNeuralNetwork(int cvtype, int inputLayers, int outputLayers, Size ksize = Size(3, 3)){

		Size size(32,24);
		cout << "Size:" << size << endl;
		int ulps = 16;

		int dims = 3;
		int ipSize[3] = { inputLayers, size.height, size.width };
		int opSize[3] = { outputLayers, size.height, size.width };

		Mat inputPlanes = Mat(dims, ipSize, cvtype);
		Mat outputPlanes = Mat(dims, opSize, cvtype);

		std::vector<cv::Mat>weights(inputLayers*outputLayers, Mat(ksize, CV_32FC1));
		std::vector<double> biases(outputLayers);

		FillRandU<T>(inputPlanes);
		for (size_t i = 0; i < weights.size(); i++){
			FillRandU<float>(weights[i]);
			weights[i] -= 0.5f;
		}
		for (size_t i = 0; i < biases.size(); i++){
			biases[i] = randu<double>() - 0.5;
		}
		
		{
			CNN::convolutionalNeuralNetwork(
				inputPlanes,				///! input [planes][rows][cols]
				outputPlanes,				///! output [planes][rows][cols]
				weights,					///! kernels [inputPlanes*outputPlanes]([ksize][ksize])
				biases						///! bias [outputPlanes]
				);
		}

		GlsMat _inputPlanes = (GlsMat)inputPlanes;
		GlsMat _outputPlanes = GlsMat(dims, opSize, cvtype);

		{
			gls::convolutionalNeuralNetwork(
				_inputPlanes,				///! input [planes][rows][cols]
				_outputPlanes,				///! output [planes][rows][cols]
				weights,					///! kernels [inputPlanes*outputPlanes]([ksize][ksize])
				biases						///! bias [outputPlanes]
				);
		}


		int errNum = 0;
		if (!AreEqual<T>(inputPlanes, (Mat)_inputPlanes, ulps)) errNum -= 1;
		if (!AreEqual<T>(outputPlanes, (Mat)_outputPlanes, ulps)) errNum -= 1;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsConvolutionalNeuralNetwork)
	{
	public:
		//glsConvolutionalNeuralNetwork

		TEST_METHOD(glsConvolutionalNeuralNetwork_8to8_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork<float>(CV_32FC1, 8, 8, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsConvolutionalNeuralNetwork_8to8_3x3)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsConvolutionalNeuralNetwork_1to8_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork<float>(CV_32FC1, 1, 8, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsConvolutionalNeuralNetwork_8to1_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork<float>(CV_32FC1, 8, 1, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsConvolutionalNeuralNetwork_8to8_5x5)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork<float>(CV_32FC1, 8, 8, Size(5, 5));
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsConvolutionalNeuralNetwork_1to8_5x5)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork<float>(CV_32FC1, 1, 8, Size(5, 5));
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsConvolutionalNeuralNetwork_8to1_5x5)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork<float>(CV_32FC1, 8, 1, Size(5, 5));
			Assert::AreEqual(0, errNum);
		}



	};
}