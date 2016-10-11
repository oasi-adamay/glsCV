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
	int test_glsPhaseCorrelatet(int cvtype, Size size = Size(32, 32)){
		cout << "Size:" << size << endl;

		int errNum = 0; 

		double sft_x = randu<double>() - 0.5;
		double sft_y = randu<double>() - 0.5;
		//画像幅の1/4 -> ±12.5%
		sft_x *= size.width/4;
		sft_y *= size.height/4;

		//Point2d shift(4.2,3.5);
		//Point2d shift(-1.0,+2.0);
		//Point2d shift(7.37318, -0.982752);
		Point2d shift(sft_x, sft_y);

		Mat affine(2, 3, CV_64FC1);
		{
			Mat_<double>m(affine);
			m(0, 0) = 1.0;
			m(0, 1) = 0.0;
			m(0, 2) = shift.x;
			m(1, 0) = 0.0;
			m(1, 1) = 1.0;
			m(1, 2) = shift.y;
		}
//			cout << affine << endl;


		Mat imgSrc = Mat::zeros(size, cvtype);
#if 0
		{
			Point center = Point(size.width / 2, size.height / 2);
			int radius = size.height / 4;

			Mat src = Mat::zeros(size, cvtype);
			cv::circle(src, center, radius, Scalar(255));
			cv::line(src, Point(0, size.height / 2), Point(size.width, size.height / 2), Scalar(255));
		}
#else
		{
			FillRandU<T>(imgSrc);
		}
#endif

		Mat imgSft;

		int flag = INTER_CUBIC; // | WARP_INVERSE_MAP;
		cv::warpAffine(imgSrc, imgSft, affine, imgSrc.size(), flag);

		Mat window;
		cv::createHanningWindow(window, imgSrc.size(),CV_32FC1);

		GlsMat _imgSrc = (GlsMat)imgSrc;
		GlsMat _imgSft = (GlsMat)imgSft;
		GlsMat _window = (GlsMat)window;


		double res_cv;
		double res_gl;
		Point2d mv_cv;
		Point2d mv_gl;
		{
			Timer tmr("cv::phaseCorrelateRes:\t");
			mv_cv = cv::phaseCorrelate(imgSrc, imgSft, window, &res_cv);

		}
		{
			Timer tmr("gls::phaseCorrelateRes:\t");
			//			mv_gl = gls::phaseCorrelateRes((GlsMat)imgSrc, (GlsMat)imgSft, (GlsMat)window, &res_gl);
			mv_gl = gls::phaseCorrelate(_imgSrc, _imgSft, _window, &res_gl);
		}

		//cout << "sft:" << shift << endl;
		//cout << "mv(cv):" << mv_cv << "\t(" << mv_cv - shift << ")" << endl;
		//cout << "mv(gl):" << mv_gl << "\t(" << mv_gl - shift << ")" << endl;
		//cout << "res(cv):" << res_cv << endl;
		//cout << "res(gl):" << res_gl << endl;


		//Mat imgDst;

		int _errNum = errNum;
		const double eps = 0.25;
		//理論値との誤差がCV+epsより大きければNG
		if (abs(shift.x - mv_cv.x) + eps < abs(shift.x - mv_gl.x)){ errNum++; }
		if (abs(shift.y - mv_cv.y) + eps < abs(shift.y - mv_gl.y)){ errNum++; }

		//CVのピーク推定値もおおよそなので、スペック甘目
		const double eps2 = 0.3;
		if (res_cv < 1.0 && abs(res_cv - res_gl) > eps2){ errNum++; }

		if (_errNum != errNum){
			cout << "sft:" << shift << endl;
			cout << "mv(cv):" << mv_cv << "\t(" << mv_cv - shift << ")" << endl;
			cout << "mv(gl):" << mv_gl << "\t(" << mv_gl - shift << ")" << endl;
			cout << "res(cv):" << res_cv << endl;
			cout << "res(gl):" << res_gl << endl;
		}

		return errNum;
	}




	TEST_CLASS(UnitTest_glsPhaseCorrelatet)
	{
	public:


		TEST_METHOD(glsPhaseCorrelatet_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsPhaseCorrelatet<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(glsPhaseCorrelatet_CV_32FC1)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsPhaseCorrelatet_CV_32FC1_64_benchmark)
		{
			cout << __FUNCTION__ << endl;
			int loop = 10;
			int errNum = 0;
			Size size(64, 64);
			for (int i = 0; i < loop; i++){
					errNum += test_glsPhaseCorrelatet<float>(CV_32FC1, size);
			}
			Assert::AreEqual(0, errNum);
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(glsPhaseCorrelatet_CV_32FC1_64_benchmark)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsPhaseCorrelatet_CV_32FC1_256_benchmark)
		{
			cout << __FUNCTION__ << endl;
			int loop = 10;
			int errNum = 0;
			Size size(256, 256);
			for (int i = 0; i < loop; i++){
				errNum += test_glsPhaseCorrelatet<float>(CV_32FC1, size);
			}
			Assert::AreEqual(0, errNum);
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(glsPhaseCorrelatet_CV_32FC1_256_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsPhaseCorrelatet_CV_32FC1_512_benchmark)
		{
			cout << __FUNCTION__ << endl;
			int loop = 10;
			int errNum = 0;
			Size size(512, 512);
			for (int i = 0; i < loop; i++){
				errNum += test_glsPhaseCorrelatet<float>(CV_32FC1, size);
			}
			Assert::AreEqual(0, errNum);
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(glsPhaseCorrelatet_CV_32FC1_512_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
			END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsPhaseCorrelatet_CV_32FC1_1024_benchmark)
		{
			cout << __FUNCTION__ << endl;
			int loop = 10;
			int errNum = 0;
			Size size(1024, 1024);
			for (int i = 0; i < loop; i++){
				errNum += test_glsPhaseCorrelatet<float>(CV_32FC1, size);
			}
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsPhaseCorrelatet_CV_32FC1_1024_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsPhaseCorrelatet_CV_32FC1_2048_benchmark)
		{
			cout << __FUNCTION__ << endl;
			int loop = 10;
			int errNum = 0;
			Size size(2048, 2048);
			for (int i = 0; i < loop; i++){
				errNum += test_glsPhaseCorrelatet<float>(CV_32FC1, size);
			}
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsPhaseCorrelatet_CV_32FC1_2048_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()

	};


}