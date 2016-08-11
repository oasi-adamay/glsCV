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
	static void fftShift(Mat& out)
	{
		if (out.rows == 1 && out.cols == 1)
		{
			// trivially shifted.
			return;
		}

		vector<Mat> planes;
		split(out, planes);

		int xMid = out.cols >> 1;
		int yMid = out.rows >> 1;

		bool is_1d = xMid == 0 || yMid == 0;

		if (is_1d)
		{
			xMid = xMid + yMid;

			for (size_t i = 0; i < planes.size(); i++)
			{
				Mat tmp;
				Mat half0(planes[i], Rect(0, 0, xMid, 1));
				Mat half1(planes[i], Rect(xMid, 0, xMid, 1));

				half0.copyTo(tmp);
				half1.copyTo(half0);
				tmp.copyTo(half1);
			}
		}
		else
		{
			for (size_t i = 0; i < planes.size(); i++)
			{
				// perform quadrant swaps...
				Mat tmp;
				Mat q0(planes[i], Rect(0, 0, xMid, yMid));
				Mat q1(planes[i], Rect(xMid, 0, xMid, yMid));
				Mat q2(planes[i], Rect(0, yMid, xMid, yMid));
				Mat q3(planes[i], Rect(xMid, yMid, xMid, yMid));

				q0.copyTo(tmp);
				q3.copyTo(q0);
				tmp.copyTo(q3);

				q1.copyTo(tmp);
				q2.copyTo(q1);
				tmp.copyTo(q2);
			}
		}

		merge(planes, out);
	}


	int test_glsFft(const int N, const int flags,const int dir = -1){
		int ULPS = 64;
		float EPS = 1e-4f;
		Size size;
		if (dir<0){	//2D
			size = Size(N, N);
		}
		else if (dir == 0){	//horizontal 1D
			size = Size(N, 1);
		}
		else{ //vertical 1D
			size = Size(1 , N);
		}
		
		Mat imgSrc = Mat(size, CV_32FC2);
		Mat imgFft = Mat::zeros(imgSrc.size(), imgSrc.type());
		Mat imgFftRef = Mat::zeros(imgSrc.size(), imgSrc.type());

		cout << "Size:" << imgSrc.size() << endl;

		//---------------------------------
		//init Src image
		FillRandU<float>(imgSrc);

		//reference select
#if 1	
		//---------------------------------
		//CPU FFT(cv::dft)
		{
			Timer tmr("cv:dft:   \t");
			cv::dft(imgSrc, imgFftRef, flags);
			if (flags & gls::GLS_FFT_SHIFT){
				fftShift(imgFftRef);
			}
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

		//target select
#if 0
		//---------------------------------
		//CPU FFT(cv::dft)
		{
			Timer tmr("Stockham:   \t");
			void fft_dit_Stockham_radix2_type0(const Mat& src, Mat &dst);
			fft_dit_Stockham_radix2_type0(imgSrc, imgFft);
		}
#elif 0
		{
			Timer tmr("Stockham:   \t");
			//void fft_dif_Stockham_radix4_type0(const Mat& src, Mat &dst);
			//fft_dif_Stockham_radix4_type0(imgSrc, imgFft);
			void fft_dit_Stockham_radix4_type0(const Mat& src, Mat &dst);
			fft_dit_Stockham_radix4_type0(imgSrc, imgFft);
		}


#else
		//---------------------------------
		{
			int _flags = 0;
			if (flags & DFT_SCALE)	_flags |= gls::GLS_FFT_SCALE;
			if (flags & DFT_INVERSE)_flags |= gls::GLS_FFT_INVERSE;
			if (flags & gls::GLS_FFT_SHIFT)_flags |= gls::GLS_FFT_SHIFT;
			GlsMat _src(imgSrc);
			GlsMat _dst;
			{
				Timer tmr("glsFft:  \t");
				gls::fft(_src, _dst,_flags);
				glFinish();
			}
			_dst.download(imgFft);
		}
#endif

		//verify
		int errNum = 0;
		if (!AreEqual<float>(imgFftRef, imgFft, ULPS, EPS)) errNum = -1;

		cout << "errNum:" << errNum << endl;

		return errNum;
	}


	TEST_CLASS(UnitTest_glsFft)
	{
	public:

		TEST_METHOD(FFT_256x256)
		{
			cout << __FUNCTION__ << endl;
			const int N = 256;
			const int flags = 0;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_256x256)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(FFT_32x32)
		{
			cout << __FUNCTION__ << endl;
			const int N = 32;
			const int flags = 0;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(FFT_16x16)
		{
			cout << __FUNCTION__ << endl;
			const int N = 16;
			const int flags = 0;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(FFT_32x32_SHFIT)
		{
			cout << __FUNCTION__ << endl;
			const int N = 16;
			const int flags = gls::GLS_FFT_SHIFT;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(FFT_128x128_SCALE)
		{
			cout << __FUNCTION__ << endl;
			const int N = 128;
			const int flags = DFT_SCALE;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(IFFT_64x64)
		{
			cout << __FUNCTION__ << endl;
			const int N = 64;
			const int flags = DFT_INVERSE;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(IFFT_32x32_SCALE)
		{
			cout << __FUNCTION__ << endl;
			const int N = 32;
			const int flags = DFT_INVERSE + DFT_SCALE;
			int errNum = test_glsFft(N, flags);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(FFT_64x64_SCALE_benchmark)
		{
			cout << __FUNCTION__ << endl;
			const int N = 64;
			const int flags = DFT_SCALE;
			int errNum = 0;
			int loop = 10;
			for (int i = 0; i < loop; i++){
				errNum += test_glsFft(N, flags);
			}
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_64x64_SCALE_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(FFT_128x128_SCALE_benchmark)
		{
			cout << __FUNCTION__ << endl;
			const int N = 128;
			const int flags = DFT_SCALE;
			int errNum = 0;
			int loop = 10;
			for (int i = 0; i < loop; i++){
				errNum += test_glsFft(N, flags);
			}
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_128x128_SCALE_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(FFT_256x256_SCALE_benchmark)
		{
			cout << __FUNCTION__ << endl;
			const int N = 256;
			const int flags = DFT_SCALE;
			int errNum = 0;
			int loop = 10;
			for (int i = 0; i < loop; i++){
				errNum += test_glsFft(N, flags);
			}
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_256x256_SCALE_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(FFT_512x512_SCALE_benchmark)
		{
			cout << __FUNCTION__ << endl;
			const int N = 512;
			const int flags = DFT_SCALE;
			int errNum = 0;
			int loop = 10;
			for (int i = 0; i < loop; i++){
				errNum += test_glsFft(N, flags);
			}
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_512x512_SCALE_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()


		TEST_METHOD(FFT_1024x1024_SCALE_benchmark)
		{
			cout << __FUNCTION__ << endl;
			const int N = 1024;
			const int flags = DFT_SCALE;
			int errNum = 0;
			int loop = 5;
			for (int i = 0; i < loop; i++){
				errNum += test_glsFft(N, flags);
			}
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_1024x1024_SCALE_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()



		TEST_METHOD(FFT_2048x2048_SCALE_benchmark)
		{
			cout << __FUNCTION__ << endl;
			const int N = 2048;
			const int flags = DFT_SCALE;
			int errNum = 0;
			int loop = 5;
			for (int i = 0; i < loop; i++){
				errNum += test_glsFft(N, flags);
			}
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_2048x2048_SCALE_benchmark)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()


		TEST_METHOD(FFT_32H)
		{
			cout << __FUNCTION__ << endl;
			const int N = 32;
			const int flags = 0;
			int errNum = test_glsFft(N, flags, 0);
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_32H)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
			//TEST_IGNORE()
		END_TEST_METHOD_ATTRIBUTE()


		TEST_METHOD(FFT_64V)
		{
			cout << __FUNCTION__ << endl;
			const int N = 64;
			const int flags = 0;
			int errNum = test_glsFft(N, flags, 1);
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(FFT_64V)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
			//TEST_IGNORE()
		END_TEST_METHOD_ATTRIBUTE()



	};
}