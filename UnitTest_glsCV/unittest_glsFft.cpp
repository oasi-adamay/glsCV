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
#include "Timer.h"
#include "Unittest_common.h"

#include "glsCV.h"



using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest_glsCV
{	

	//static
	//bool AlmostEqualUlpsAbsEps(float A, float B, int maxUlps, float maxDiff)
	//{
	//	// Check if the numbers are really close -- needed
	//	// when comparing numbers near zero.
	//	float absDiff = fabs(A - B);
	//	if (absDiff <= maxDiff)
	//		return true;

	//	// Make sure maxUlps is non-negative and small enough that the
	//	// default NAN won't compare as equal to anything.
	//	GLS_Assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);
	//	int aInt = *(int*)&A;
	//	// Make aInt lexicographically ordered as a twos-complement int
	//	if (aInt < 0)
	//		aInt = 0x80000000 - aInt;
	//	// Make bInt lexicographically ordered as a twos-complement int
	//	int bInt = *(int*)&B;
	//	if (bInt < 0)
	//		bInt = 0x80000000 - bInt;
	//	int intDiff = abs(aInt - bInt);
	//	if (intDiff <= maxUlps)
	//		return true;
	//	return false;
	//}


	int test_glsFft(const int N, const int flags){
		int ULPS = 64;
		float EPS = 1e-4f;
		Mat imgSrc = Mat(Size(N, N), CV_32FC2);
		Mat imgFft = Mat::zeros(imgSrc.size(), imgSrc.type());
		Mat imgFftRef = Mat::zeros(imgSrc.size(), imgSrc.type());

		cout << "Size:" << imgSrc.size() << endl;

		//---------------------------------
		//init Src image
		FillRandU<float>(imgSrc);

#if 1
		//---------------------------------
		//CPU FFT(cv::dft)
		{
			Timer tmr("cv:dft:   \t");
			cv::dft(imgSrc, imgFftRef, flags);
		}
#else
		//---------------------------------
		//CPU FFT(cv::dft)
		{
			Timer tmr("cv:dft:   \t");
			void fft_dit_Stockham_radix2_type0(const Mat& src, Mat &dst);
			fft_dit_Stockham_radix2_type0(imgSrc, imgFftRef);
		}
#endif

		//---------------------------------
		{
			int _flags = 0;
			if (flags & DFT_SCALE)	_flags |= GLS_FFT_SCALE;
			if (flags & DFT_INVERSE)_flags |= GLS_FFT_INVERSE;
#if 1
			glsMat _src(imgSrc);
			glsMat _dst;
			{
				Timer tmr("glsFft:  \t");
				glsFft(_src, _dst,_flags);
			}
			_dst.CopyTo(imgFft);
#elif 1
			glsMat _src(imgSrc.size(), imgSrc.type(),Size(2,2));
			_src.CopyFrom(imgSrc);
			glsMat _dst;
			{
				Timer tmr("glsFft:  \t");
				glsFft(_src, _dst, _flags);
			}
			_dst.CopyTo(imgFft);
#endif

		}

		//verify
		int errNum = 0;
		if (!AreEqual<float>(imgFftRef, imgFft, ULPS, EPS)) errNum = -1;

		cout << "errNum:" << errNum << endl;

		return errNum;
	}


	TEST_CLASS(UnitTest_glsFft)
	{
	public:


		TEST_METHOD(FFT)
		{
			cout << __FUNCTION__ << endl;
			const int N = 256;
			const int flags = 0;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}

//		TEST_METHOD(FFT_time)
//		{
//			cout << __FUNCTION__ << endl;
//			const int N = 256;
////			const int N = 1024;
//			const int flags = 0;
//			int errNum = 0;
//			for (int i = 0; i < 100; i++){
//				errNum += test_glsFft(N, flags);
//			}
//			Assert::AreEqual(0, errNum);
//		}


		TEST_METHOD(FFT_SCALE)
		{
			cout << __FUNCTION__ << endl;
			const int N = 128;
			const int flags = DFT_SCALE;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(IFFT)
		{
			cout << __FUNCTION__ << endl;
			const int N = 64;
			const int flags = DFT_INVERSE;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(IFFT_SCALE)
		{
			cout << __FUNCTION__ << endl;
			const int N = 32;
			const int flags = DFT_INVERSE + DFT_SCALE;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}



	};
}