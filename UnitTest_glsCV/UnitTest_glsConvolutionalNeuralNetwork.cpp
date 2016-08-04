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
		int ulps = 4;
		float diff = 1e-5f;


		int dims = 3;
		int ipSize[3] = { inputLayers, size.height, size.width };
		int opSize[3] = { outputLayers, size.height, size.width };
		int _kSize[3] = { inputLayers*outputLayers, ksize.height, ksize.width };

		Mat inputPlanes = Mat(dims, ipSize, cvtype);
		Mat outputPlanes = Mat(dims, opSize, cvtype);

		cv::Mat weights(dims, _kSize, CV_32FC1);
		std::vector<double> biases(outputLayers);

		FillRandU<T>(inputPlanes);
		FillRandU<T>(weights);
		weights -= 0.5f;

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
		if (!AreEqual<T>(inputPlanes, (Mat)_inputPlanes, ulps, diff)) errNum -= 1;
		if (!AreEqual<T>(outputPlanes, (Mat)_outputPlanes, ulps, diff)) errNum -= 1;


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

		TEST_METHOD(glsConvolutionalNeuralNetwork_1to32_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork<float>(CV_32FC1, 1, 32, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvolutionalNeuralNetwork_32to64_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork<float>(CV_32FC1, 32, 64, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvolutionalNeuralNetwork_64to128_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork<float>(CV_32FC1, 64, 128, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvolutionalNeuralNetwork_128to128_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork<float>(CV_32FC1, 128, 128, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvolutionalNeuralNetwork_128to1_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork<float>(CV_32FC1, 128, 1, Size(3, 3));
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

	template <typename T>
	void pack(Mat& src, Mat& dst,int channels = 4){
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

	template <typename T>
	void unpack(Mat& src, Mat& dst){
		CV_Assert(src.dims == 3);

		const int srcPlanes = src.size[0];
		const Size srcSize(src.size[2], src.size[1]);
		const int srcChannels = src.channels();

		const int dstChannels = 1;
		const int dstPlanes = srcPlanes * srcChannels;
		const Size dstSize = srcSize;

		const int sizes[3] = { dstPlanes, dstSize.height, dstSize.width };

		dst = Mat(3, sizes, CV_MAKETYPE(src.depth(), dstChannels));

		for (int i = 0; i < src.size[0]; i++){
			for (int j = 0; j < src.size[1]; j++){
				for (int k = 0; k < src.size[2]; k++){
					for (int cn = 0; cn < srcChannels; cn++){
						int _i = i * srcChannels + cn;
						int _j = j;
						int _k = k;
						int _cn = 0;
						*(dst.ptr<T>(_i, _j, _k) + _cn) = *(src.ptr<T>(i, j, k) + cn);
					}
				}
			}
		}

	}


	template <typename T>
	int test_glsConvolutionalNeuralNetwork_Pack(int cvtype, int inputLayers, int outputLayers, Size ksize = Size(3, 3)){

		Size size(32, 24);
//		Size size(5, 4);
		cout << "Size:" << size << endl;
		int ulps = 4;
		float diff = 1e-4f;

		const int channels = 4;
		int ipCh = inputLayers / channels > 0 ? channels : inputLayers;	//input channels
		int opCh = outputLayers / channels > 0 ? channels : outputLayers;	//output channels

		int dims = 3;
		int ipSize[3] = { inputLayers, size.height, size.width };
		int opSize[3] = { outputLayers, size.height, size.width };
		int _kSize[3] = { inputLayers*outputLayers, ksize.height, ksize.width };

		Mat inputPlanes = Mat(dims, ipSize, cvtype);
		Mat outputPlanes = Mat(dims, opSize, cvtype);

		cv::Mat weights(dims, _kSize, CV_32FC1);
		std::vector<double> biases(outputLayers);

		FillRandU<T>(inputPlanes);
		FillRandU<T>(weights);
		weights -= 0.5f;

#if 0
		{
			Mat _inputPlanes;
			Mat __inputPlanes;
			pack<T>(inputPlanes, _inputPlanes);
			unpack<T>(_inputPlanes, __inputPlanes);
			int errNum = 0;
			if (!AreEqual<T>(inputPlanes, __inputPlanes, ulps, diff)) errNum -= 1;
			Assert::AreEqual(0, errNum);

		}
#endif

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
#if 0
		Mat _inputPlanes;
		Mat _outputPlanes;
		Mat _weights;

		pack<T>(inputPlanes, _inputPlanes, channels);
		pack<T>(weights, _weights, _inputPlanes.channels());
		pack<T>(outputPlanes, _outputPlanes, channels);

		{
			CNN::convolutionalNeuralNetwork(
				_inputPlanes,				///! input [planes][rows][cols]
				_outputPlanes,				///! output [planes][rows][cols]
				_weights,					///! kernels [inputPlanes*outputPlanes]([ksize][ksize])
				biases						///! bias [outputPlanes]
				);
		}
		Mat __outputPlanes;
		unpack<T>(_outputPlanes, __outputPlanes);


		int errNum = 0;
		if (!AreEqual<T>(outputPlanes, __outputPlanes, ulps, diff)) errNum -= 1;


#else
		Mat _inputPlanes;
		Mat _outputPlanes;
		Mat _weights;

		pack<T>(inputPlanes, _inputPlanes, channels);
		pack<T>(weights, _weights, _inputPlanes.channels());
		pack<T>(outputPlanes, _outputPlanes, channels);

		GlsMat __inputPlanes = (GlsMat)_inputPlanes;
		GlsMat __outputPlanes(_outputPlanes.dims, _outputPlanes.size, _outputPlanes.type());

		{
			gls::convolutionalNeuralNetwork(
				__inputPlanes,				///! input [planes][rows][cols]
				__outputPlanes,				///! output [planes][rows][cols]
				_weights,					///! kernels [inputPlanes*outputPlanes]([ksize][ksize])
				biases						///! bias [outputPlanes]
				);
		}


		int errNum = 0;
		unpack<T>((Mat)__outputPlanes, _outputPlanes);

		if (!AreEqual<T>(outputPlanes, _outputPlanes, ulps, diff)) errNum -= 1;
#endif

		return errNum;
	}

	TEST_CLASS(UnitTest_glsConvolutionalNeuralNetwork_Pack)
	{
	public:
		//glsConvolutionalNeuralNetwork
		TEST_METHOD(glsConvolutionalNeuralNetwork_Pack_8to8_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork_Pack<float>(CV_32FC1, 8, 8, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsConvolutionalNeuralNetwork_Pack_8to8_3x3)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsConvolutionalNeuralNetwork_Pack_1to8_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork_Pack<float>(CV_32FC1, 1, 8, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsConvolutionalNeuralNetwork_Pack_8to1_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork_Pack<float>(CV_32FC1, 8, 1, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsConvolutionalNeuralNetwork_Pack_1to32_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork_Pack<float>(CV_32FC1, 1, 32, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvolutionalNeuralNetwork_Pack_32to64_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork_Pack<float>(CV_32FC1, 32, 64, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvolutionalNeuralNetwork_Pack_64to128_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork_Pack<float>(CV_32FC1, 64, 128, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvolutionalNeuralNetwork_Pack_128to128_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork_Pack<float>(CV_32FC1, 128, 128, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvolutionalNeuralNetwork_Pack_128to1_3x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvolutionalNeuralNetwork_Pack<float>(CV_32FC1, 128, 1, Size(3, 3));
			Assert::AreEqual(0, errNum);
		}



	};


}