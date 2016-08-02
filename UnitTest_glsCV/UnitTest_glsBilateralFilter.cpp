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
	int test_glsBilateralFilter(int cvtype, int ksize = 5, Size size = Size(32, 24)){
		int ulps = 0;
		double sigmaSpace = 0.3*(ksize / 2.0 - 1) + 0.8;
		double sigmaColor = 0.1;

		cout << "Size:" << size << endl;
		cout << "ksize:" << ksize << endl;
		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);

		Mat imgRef;
		Mat imgDst;

		int loop = (size.width >= 256)? 10 : 1;

		for (int i = 0; i < loop;i++){
			_TMR_("cv::bilateralFilter:\t");
			cv::bilateralFilter(imgSrc, imgRef, ksize, sigmaColor, sigmaSpace);
		}

		GlsMat glsSrc(imgSrc);
		GlsMat glsDst;


		for (int i = 0; i < loop; i++){
			_TMR_("gls::bilateralFilter:\t");
			gls::bilateralFilter(glsSrc, glsDst, ksize, sigmaColor, sigmaSpace);
		}

		glsDst.download(imgDst);		// download

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst, ulps)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsBilateralFilter)
	{
	public:

		TEST_METHOD(glsBilateralFilter_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBilateralFilter<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsBilateralFilter_CV_32FC1)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()


		TEST_METHOD(glsBilateralFilter_CV_32FC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBilateralFilter<float>(CV_32FC3);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(glsBilateralFilter_CV_32FC1_5x5_1024x1024)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBilateralFilter<float>(CV_32FC1, 5,  Size(1024, 1024));
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsBilateralFilter_CV_32FC1_5x5_1024x1024)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()


	};



}