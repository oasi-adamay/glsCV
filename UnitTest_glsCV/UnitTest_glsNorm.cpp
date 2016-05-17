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
	int test_glsNorm(int cvtype, int normType, Size size = Size(32, 24)){
		int ulps = 4;
		cout << "Size:" << size << endl;
		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);


		double normRef;
		double norm;

		GlsMat glsSrc(imgSrc);

		int loop = 1;
		for (int i = 0; i < loop; i++){
			_TMR_("cv::norm:\t");
			normRef = cv::norm(imgSrc, normType);
		}
		for (int i = 0; i < loop; i++){
			_TMR_("gls::norm:\t");
			norm = gls::norm(glsSrc, normType);
		}



		cout << normRef  << endl;
		cout << norm << endl;


		int errNum = 0;
		if (!AreEqual<float>((float)normRef, (float)norm, ulps))errNum++;

		cout << "errNum:" << errNum << endl;

		return errNum;
	}

	TEST_CLASS(UnitTest_glsNorm)
	{
	public:
		TEST_METHOD(glsNorm_CV_32FC1_NORM_INF)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNorm<float>(CV_32FC1, NORM_INF);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsNorm_CV_32FC2_NORM_INF)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNorm<float>(CV_32FC2, NORM_INF);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsNorm_CV_32FC3_NORM_INF)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNorm<float>(CV_32FC3, NORM_INF);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsNorm_CV_32FC4_NORM_INF)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNorm<float>(CV_32FC4, NORM_INF);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsNorm_CV_32FC1_NORM_L1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNorm<float>(CV_32FC1, NORM_L1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsNorm_CV_32FC2_NORM_L1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNorm<float>(CV_32FC2, NORM_L1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsNorm_CV_32FC3_NORM_L1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNorm<float>(CV_32FC3, NORM_L1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsNorm_CV_32FC4_NORM_L1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNorm<float>(CV_32FC4, NORM_L1);
			Assert::AreEqual(0, errNum);
		}




		TEST_METHOD(glsNorm_CV_32FC1_NORM_L2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNorm<float>(CV_32FC1, NORM_L2);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsNorm_CV_32FC2_NORM_L2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNorm<float>(CV_32FC2, NORM_L2);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsNorm_CV_32FC3_NORM_L2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNorm<float>(CV_32FC3, NORM_L2);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsNorm_CV_32FC4_NORM_L2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNorm<float>(CV_32FC4, NORM_L2);
			Assert::AreEqual(0, errNum);
		}



	};

}