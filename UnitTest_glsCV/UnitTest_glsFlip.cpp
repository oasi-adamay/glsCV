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
	int test_glsFlip(int cvtype, int flipCode){
		int ulps = 0;
		Size size(32, 24);
		cout << "Size:" << size << endl;

		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);

		Mat imgRef;
		Mat imgDst;
		GlsMat glsSrc(imgSrc);
		GlsMat glsDst;

		cv::flip(imgSrc, imgRef, flipCode);
		gls::flip(glsSrc, glsDst, flipCode);

		imgDst = (Mat)glsDst;

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst, ulps)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsFlip)
	{
	public:
		TEST_METHOD(glsFlip_CV_32FC1_0)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsFlip<float>(CV_32FC1, 0);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsFlip_CV_32FC2_0)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsFlip<float>(CV_32FC2, 0);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsFlip_CV_32FC3_0)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsFlip<float>(CV_32FC3, 0);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsFlip_CV_32FC4_0)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsFlip<float>(CV_32FC4, 0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsFlip_CV_32FC1_1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsFlip<float>(CV_32FC1, 1);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsFlip_CV_32FC1_m1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsFlip<float>(CV_32FC1, -1);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsFlip_CV_8UC1_0)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsFlip<uchar>(CV_8UC1, 0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsFlip_CV_16UC1_0)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsFlip<ushort>(CV_16UC1, 0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsFlip_CV_32SC1_0)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsFlip<int>(CV_32SC1, 0);
			Assert::AreEqual(0, errNum);
		}


	};

}