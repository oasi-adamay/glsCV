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
	int test_glsSepFilter2D(int cvtype, int ulps = 0, Size ksize = Size(5, 5), Size size = Size(32, 24)){

		cout << "Size:" << size << endl;
		cout << "ksize:" << ksize << endl;
		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);
		double delta = 0.5;

		Mat imgRef;
		Mat imgDst;

		Mat kernel = cv::getGaussianKernel(ksize.width, 3.0 ,CV_32F);

//		Mat kernelX = kernel.reshape(0,1);
		Mat kernelX = kernel;	//Ç«ÇøÇÁÇ≈Ç‡OK
		Mat kernelY = kernel;

		cout << kernelX << endl;
		cout << kernelY << endl;



		{
			cv::sepFilter2D(imgSrc, imgRef, imgSrc.depth(), kernelX, kernelY,Point(-1,-1),delta);
		}

		GlsMat glsSrc(imgSrc);
		GlsMat glsDst;


		{
			gls::sepFilter2D(glsSrc, glsDst, imgSrc.depth(), kernelX, kernelY,Point(-1, -1), delta);
		}

		glsDst.download(imgDst);		// download

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst, ulps)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsSepFilter2D)
	{
	public:
		TEST_METHOD(glsSepFilter2D_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsSepFilter2D<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsSepFilter2D_CV_32FC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsSepFilter2D<float>(CV_32FC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsSepFilter2D_CV_32FC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsSepFilter2D<float>(CV_32FC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsSepFilter2D_CV_32FC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsSepFilter2D<float>(CV_32FC4);
			Assert::AreEqual(0, errNum);
		}


	};


	template <typename T>
	int test_glsGaussianBlur(int cvtype, int ulps = 0,Size ksize = Size(5, 5), Size size = Size(32, 24)){

		cout << "Size:" << size << endl;
		cout << "ksize:" << ksize << endl;
		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);

		Mat imgRef;
		Mat imgDst;

		int loop = (size.width >= 256)? 10 : 1;

		for (int i = 0; i < loop;i++){
			_TMR_("cv::GaussianBlur:\t");
			cv::GaussianBlur(imgSrc, imgRef, ksize,0,0);
		}

		GlsMat glsSrc(imgSrc);
		GlsMat glsDst;


		for (int i = 0; i < loop; i++){
			_TMR_("gls::GaussianBlur:\t");
			gls::GaussianBlur(glsSrc, glsDst, ksize, 0, 0);
		}

		glsDst.download(imgDst);		// download

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst, ulps)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsGaussianBlur)
	{
	public:
		TEST_METHOD(glsGaussianBlur_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsGaussianBlur<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsGaussianBlur_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsGaussianBlur<uchar>(CV_8UC1,1);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsGaussianBlur_CV_16UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsGaussianBlur<ushort>(CV_16UC1, 1);
			Assert::AreEqual(0, errNum);
		}


		//! benchmark
		BEGIN_TEST_METHOD_ATTRIBUTE(glsGaussianBlur_CV_32FC1_5x5_1024x1024)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
			END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsGaussianBlur_CV_32FC1_5x5_1024x1024)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsGaussianBlur<float>(CV_32FC1, 0, Size(5, 5),  Size(1024, 1024));
			Assert::AreEqual(0, errNum);
		}


	};


	template <typename T>
	int test_glsBoxFilter(int cvtype, int ulps = 0, Size ksize = Size(5, 5), Size size = Size(32, 24)){

		cout << "Size:" << size << endl;
		cout << "ksize:" << ksize << endl;
		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);

		Mat imgRef;
		Mat imgDst;

		int loop = (size.width >= 256) ? 10 : 1;

		for (int i = 0; i < loop; i++){
			_TMR_("cv::boxFilter:\t");
			cv::boxFilter(imgSrc, imgRef, imgSrc.depth(),ksize);
		}

		GlsMat glsSrc(imgSrc);
		GlsMat glsDst;


		for (int i = 0; i < loop; i++){
			_TMR_("gls::boxFilter:\t");
			gls::boxFilter(glsSrc, glsDst, glsSrc.depth(),ksize);
		}

		glsDst.download(imgDst);		// download

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst, ulps)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsBoxFilter)
	{
	public:
		TEST_METHOD(glsBoxFilter_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBoxFilter<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsBoxFilter_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBoxFilter<uchar>(CV_8UC1,1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsBoxFilter_CV_16UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBoxFilter<ushort>(CV_16UC1,1);
			Assert::AreEqual(0, errNum);
		}



		//! benchmark
		BEGIN_TEST_METHOD_ATTRIBUTE(glsBoxFilter_CV_32FC1_5x5_1024x1024)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
			END_TEST_METHOD_ATTRIBUTE()

			TEST_METHOD(glsBoxFilter_CV_32FC1_5x5_1024x1024)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBoxFilter<float>(CV_32FC1, 0, Size(5, 5), Size(1024, 1024));
			Assert::AreEqual(0, errNum);
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(glsBoxFilter_CV_32FC1_33x33_1024x1024)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
			END_TEST_METHOD_ATTRIBUTE()

			TEST_METHOD(glsBoxFilter_CV_32FC1_33x33_1024x1024)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBoxFilter<float>(CV_32FC1, 0, Size(33, 33), Size(1024, 1024));
			Assert::AreEqual(0, errNum);
		}


	};


	template <typename T>
	int test_glsSobel(int cvtype, int xorder, int yorder ,int ksize = 3, int ulps = 0, Size size = Size(32, 24)){
		double scale = 1.25;
		double delta = 0.5;

		cout << "Size:" << size << endl;
		cout << "ksize:" << ksize << endl;
		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);

		Mat imgRef;
		Mat imgDst;

		int loop = (size.width >= 256) ? 10 : 1;

		for (int i = 0; i < loop; i++){
			_TMR_("cv::Sobel:\t");
			cv::Sobel(imgSrc, imgRef, imgSrc.depth(), xorder, yorder, ksize, scale, delta);
		}

		GlsMat glsSrc(imgSrc);
		GlsMat glsDst;


		for (int i = 0; i < loop; i++){
			_TMR_("gls::Sobel:\t");
			gls::Sobel(glsSrc, glsDst, glsSrc.depth(), xorder, yorder, ksize, scale, delta);
		}

		glsDst.download(imgDst);		// download

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst, ulps)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsSobel)
	{
	public:
		TEST_METHOD(glsSobel_CV_32FC1_1_0)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsSobel<float>(CV_32FC1,1,0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsSobel_CV_32FC1_0_1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsSobel<float>(CV_32FC1, 0, 1);
			Assert::AreEqual(0, errNum);
		}

	};



	template <typename T>
	int test_glsLaplacian(int cvtype, int ksize = 1, Size size = Size(32, 24)){
		//TODO åÎç∑
		int ulps = 16;
		float diff = 0.00001f;
		if (ksize == 1) ulps = 4;
		else if(ksize == 3) ulps = 16;
		else  ulps = 64;

		double scale = 1.25;
		double delta = 0.5;
//		double scale = 1.0;
//		double delta = 0.0;

		cout << "Size:" << size << endl;
		cout << "ksize:" << ksize << endl;
		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);

		Mat imgRef;
		Mat imgDst;

		int loop = (size.width >= 256) ? 10 : 1;

		for (int i = 0; i < loop; i++){
			_TMR_("cv::Laplacian:\t");
			cv::Laplacian(imgSrc, imgRef, imgSrc.depth(), ksize, scale, delta);
		}

		GlsMat glsSrc(imgSrc);
		GlsMat glsDst;


		for (int i = 0; i < loop; i++){
			_TMR_("gls::Laplacian:\t");
			gls::Laplacian(glsSrc, glsDst, glsSrc.depth(), ksize, scale, delta);
		}

		glsDst.download(imgDst);		// download

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst, ulps ,diff)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsLaplacian)
	{
	public:
		TEST_METHOD(glsLaplacian_CV_32FC1_1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsLaplacian<float>(CV_32FC1, 1);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsLaplacian_CV_32FC1_3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsLaplacian<float>(CV_32FC1, 3);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsLaplacian_CV_32FC1_5)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsLaplacian<float>(CV_32FC1, 5);
			Assert::AreEqual(0, errNum);
		}


	};



	template <typename T>
	int test_glsFilter2D(int cvtype, Size ksize = Size(5, 5), Size size = Size(32, 24)){
		int ulps = 16;
		double delta = 0.5;
		Point anchor = Point(-1, -1);

		cout << "Size:" << size << endl;
		cout << "ksize:" << ksize << endl;
		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);

		Mat imgRef;
		Mat imgDst;

		Mat kernel = Mat(ksize,CV_32FC1);
		FillRandU<float>(kernel);

		int loop = (size.width >= 256) ? 10 : 1;

		for (int i = 0; i < loop; i++){
			_TMR_("cv::filter2D:\t");
			cv::filter2D(imgSrc, imgRef, -1, kernel, anchor, delta);
		}

		GlsMat glsSrc(imgSrc);
		GlsMat glsDst;


		for (int i = 0; i < loop; i++){
			_TMR_("gls::filter2D:\t");
			gls::filter2D(glsSrc, glsDst, -1, kernel, anchor, delta);
		}

		glsDst.download(imgDst);		// download

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst, ulps)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}

	TEST_CLASS(UnitTest_glsFilter2D)
	{
	public:
		TEST_METHOD(glsFilter2D_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsFilter2D<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
	};


}