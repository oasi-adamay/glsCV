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
	int test_glsNormalize(int cvtype, double alpha, double beta ,int normType, int ulps = 0){
		const int width = 24;
		const int height = 32;
//		const int width = 8;
//		const int height = 6;



		Mat imgSrc(Size(width, height), cvtype);
		FillRandU<T>(imgSrc);

		Mat imgRef;
		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;

		cv::normalize(imgSrc, imgRef, alpha, beta, normType);

		GlsMat glsSrc(imgSrc);

		GlsMat glsDst;

		gls::normalize(glsSrc, glsDst, alpha, beta, normType);

		glsDst.CopyTo(imgDst);		// download

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst, ulps)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsNormalize)
	{
	public:
		//glsNormalize
		TEST_METHOD(glsNormalize_CV_32FC1_0_1_NORM_MINMAX)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNormalize<float>(CV_32FC1, 0.0, 1.0, NORM_MINMAX,4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsNormalize_CV_32FC1_05_2_NORM_MINMAX)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNormalize<float>(CV_32FC1, 0.5, 2.0, NORM_MINMAX,4);
			Assert::AreEqual(0, errNum);
		}

	};
}