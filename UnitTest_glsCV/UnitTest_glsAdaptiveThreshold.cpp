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
	int test_glsAdaptiveThreshold(int cvtype, int adaptiveMethod, int thresholdType, int blockSize){
		Size size(32, 24);
		cout << "Size:" << size << endl;

		
		int ulps = 1;

		double thresh = 0;
		double maxVal = 0;
		double C = 0;
		if (CV_MAT_DEPTH(cvtype) == CV_32F){
			thresh = 0.5;
			maxVal = 0.75;
		}
		else if (CV_MAT_DEPTH(cvtype) == CV_8U){
			thresh = 128;
			maxVal = 192;
			C = 5;
		}
		else if (CV_MAT_DEPTH(cvtype) == CV_16U){
			thresh = 32768;
			maxVal = 45000;
		}


		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);

		Mat imgRef;
		Mat imgDst;


		cv::adaptiveThreshold(imgSrc, imgRef, maxVal, adaptiveMethod, thresholdType, blockSize,C);

		GlsMat glsSrc(imgSrc);
		GlsMat glsDst;

		gls::adaptiveThreshold(glsSrc, glsDst, maxVal, adaptiveMethod, thresholdType, blockSize,C);

		glsDst.download(imgDst);		// download

		int errNum = 0;
		/// TODO 2値化なのでどのようにエラーを定義するか。
//		if (!AreEqual<T>(imgRef, imgDst, ulps)) errNum = -1;

		cout << imgRef << endl;
		cout << imgDst << endl;
		cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsAdaptiveThreshold)
	{
	public:
		//! basic
		BEGIN_TEST_METHOD_ATTRIBUTE(glsAdaptiveThreshold_CV_8UC1_ADAPTIVE_THRESH_MEAN_Cx5)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsAdaptiveThreshold_CV_8UC1_ADAPTIVE_THRESH_MEAN_Cx5)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsAdaptiveThreshold<uchar>(CV_8UC1, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY,5);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsAdaptiveThreshold_CV_8UC1_ADAPTIVE_THRESH_GAUSSIAN_C_Cx5)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsAdaptiveThreshold<uchar>(CV_8UC1, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 5);
			Assert::AreEqual(0, errNum);
		}


	};
}