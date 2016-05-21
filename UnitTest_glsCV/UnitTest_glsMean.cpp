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
	int test_glsMean(int cvtype,Size size = Size(32, 24)){
		int ulps = 4;
		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);

		cout << "Size:" << imgSrc.size() << endl;

		Scalar meanRef;
		Scalar mean;

		GlsMat glsSrc(imgSrc);

		{
			_TMR_("cv::mean:\t");
			meanRef = cv::mean(imgSrc);
		}
		{
			_TMR_("gls::mean:\t");
			mean = gls::mean(glsSrc);
		}

		cout << meanRef << "," << mean << endl;


		int errNum = 0;
		if (!AreEqual<Scalar>(meanRef, mean, ulps))errNum++;

		cout << "errNum:" << errNum << endl;

		return errNum;
	}

	TEST_CLASS(UnitTest_glsMean)
	{
	public:
		TEST_METHOD(glsMean_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMean<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMean_CV_32FC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMean<float>(CV_32FC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMean_CV_32FC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMean<float>(CV_32FC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMean_CV_32FC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMean<float>(CV_32FC4);
			Assert::AreEqual(0, errNum);
		}

	};

	template <typename T>
	int test_glsMeanStdDev(int cvtype, Size size = Size(32, 24)){
		int ulps = 4;
		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);

		cout << "Size:" << imgSrc.size() << endl;

		Scalar meanRef;
		Scalar mean;
		Scalar stddevRef;
		Scalar stddev;

		GlsMat glsSrc(imgSrc);

		{
			_TMR_("cv::meanStdDev:\t");
			cv::meanStdDev(imgSrc, meanRef, stddevRef);
		}
		{
			_TMR_("gls::meanStdDev:\t");
			gls::meanStdDev(glsSrc,mean,stddev);
		}

		cout << meanRef << "," << mean << endl;
		cout << stddevRef << "," << stddev << endl;


		int errNum = 0;
		if (!AreEqual<Scalar>(meanRef, mean, ulps))errNum++;
		if (!AreEqual<Scalar>(stddevRef, stddev, ulps))errNum++;

		cout << "errNum:" << errNum << endl;

		return errNum;
	}

	TEST_CLASS(UnitTest_glsMeanStdDev)
	{
	public:
		TEST_METHOD(glsMeanStdDev_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMeanStdDev<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMeanStdDev_CV_32FC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMeanStdDev<float>(CV_32FC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMeanStdDev_CV_32FC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMeanStdDev<float>(CV_32FC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMeanStdDev_CV_32FC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMeanStdDev<float>(CV_32FC4);
			Assert::AreEqual(0, errNum);
		}

	};


}