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




namespace UnitTest_glsCV
{

	template <typename T>
	int test_glsResize(int cvtype, int interpolation, double fx = 1.0, double fy = 1.0){
		int ulps = 4;
		float eps = 0.01f;
		Size size(32, 24);
		Size dsize(0,0);

//		Size dsize(32, 24);
//		Size dsize(64, 48);
//		Size dsize(48, 32);
//		double fx = 0;
//		double fy = 0;
		
		cout << "Size:" << size << endl;

		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);

		Mat imgRef;
		Mat imgDst;
		GlsMat glsSrc(imgSrc);
		GlsMat glsDst;

		cv::resize(imgSrc, imgRef, dsize, fx, fy, interpolation);
		gls::resize(glsSrc, glsDst, dsize, fx, fy, interpolation);

		imgDst = (Mat)glsDst;

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst, ulps, eps)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsResize)
	{
	public:
		TEST_METHOD(glsResize_CV_32FC1_INTER_NEAREST_x1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsResize<float>(CV_32FC1, INTER_NEAREST, 1.0, 1.0);
			Assert::AreEqual(0, errNum);
		}

		//TEST_METHOD(glsResize_CV_32FC1_INTER_NEAREST_x1p5)
		//{
		//	cout << __FUNCTION__ << endl;
		//	int errNum = test_glsResize<float>(CV_32FC1, INTER_NEAREST, 1.5, 1.5);
		//	Assert::AreEqual(0, errNum);
		//}


		TEST_METHOD(glsResize_CV_32FC1_INTER_NEAREST_x2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsResize<float>(CV_32FC1, INTER_NEAREST,2.0,2.0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsResize_CV_32FC1_INTER_LINEAR_x1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsResize<float>(CV_32FC1, INTER_LINEAR, 1.0, 1.0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsResize_CV_32FC1_INTER_LINEAR_x1p5)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsResize<float>(CV_32FC1, INTER_LINEAR, 1.5, 1.5);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(glsResize_CV_32FC1_INTER_LINEAR_x2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsResize<float>(CV_32FC1, INTER_LINEAR, 2.0, 2.0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsResize_CV_32FC1_INTER_CUBIC_x1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsResize<float>(CV_32FC1, INTER_CUBIC, 1.0, 1.0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsResize_CV_32FC1_INTER_CUBIC_x1p5)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsResize<float>(CV_32FC1, INTER_CUBIC, 1.5, 1.5);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsResize_CV_32FC1_INTER_CUBIC_x2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsResize<float>(CV_32FC1, INTER_CUBIC, 2.0, 2.0);
			Assert::AreEqual(0, errNum);
		}
		
		TEST_METHOD(glsResize_CV_8UC1_INTER_NEAREST_x1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsResize<uchar>(CV_8UC1, INTER_NEAREST, 1.0, 1.0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsResize_CV_8UC1_INTER_NEAREST_x2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsResize<uchar>(CV_8UC1, INTER_NEAREST, 2.0, 2.0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsResize_CV_8UC1_INTER_LINEAR_x1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsResize<uchar>(CV_8UC1, INTER_LINEAR, 1.0, 1.0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsResize_CV_8UC1_INTER_LINEAR_x2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsResize<uchar>(CV_8UC1, INTER_LINEAR, 2.0, 2.0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsResize_CV_8UC1_INTER_CUBIC_x1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsResize<uchar>(CV_8UC1, INTER_CUBIC, 1.0, 1.0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsResize_CV_8UC1_INTER_CUBIC_x2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsResize<uchar>(CV_8UC1, INTER_CUBIC, 2.0, 2.0);
			Assert::AreEqual(0, errNum);
		}





	};

}