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
#include "Timer.h"

//#ifdef _DEBUG
#if 1
#define _TMR_(...)  Timer tmr(__VA_ARGS__)
#else
#define _TMR_(...)
#endif



namespace UnitTest_glsCV
{

	template <typename T>
	int test_glsCopy(int cvtype, Size size = Size(32,24)){

		Mat imgSrc = Mat(size, cvtype);
		Mat imgRef;
		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;

		//---------------------------------
		//init Src image
		FillRandU<T>(imgSrc);

		{
			_TMR_("-cv::copyTo:\t");
			imgSrc.copyTo(imgRef);
		}

		//----------------------
		GlsMat glsSrc(imgSrc);		//create GlsMat and  upload
		GlsMat glsDst;
		{
			_TMR_("-glsCopy:\t");
			gls::copy(glsSrc, glsDst);	//copy texture
		}


		glsDst.download(imgDst);		// download

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst)) errNum = -1;

		//cout << imgSrc << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgSrc << endl;


		return errNum;
	}








	TEST_CLASS(UnitTest_glsCopy)
	{
	public:
		//glsCopy
		//! basic
		BEGIN_TEST_METHOD_ATTRIBUTE(glsCopy_CV_32FC1)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsCopy_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopy_CV_32FC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy<float>(CV_32FC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopy_CV_32FC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy<float>(CV_32FC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopy_CV_32FC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy<float>(CV_32FC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopy_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy<uchar>(CV_8UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopy_CV_16UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy<ushort>(CV_16UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopy_CV_32SC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy<int>(CV_32SC1);
			Assert::AreEqual(0, errNum);
		}

		BEGIN_TEST_METHOD_ATTRIBUTE(glsCopy_CV_32FC1_1024x1024)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsCopy_CV_32FC1_1024x1024)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy<float>(CV_32FC1, Size(1024, 1024));
			Assert::AreEqual(0, errNum);
		}

	};

	template <typename T>
	int test_glsCopyRect(int cvtype){
		int width = 32;
		int height = 24;
		//		const Rect rect(4, 3, 15, 13);
		Rect rect(4, 3, 8, 6);


		Mat imgSrc = Mat(Size(width, height), cvtype);
		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;
		cout << "Rect:" << rect << endl;

		//---------------------------------
		//init Src image
		FillRandU<T>(imgSrc);


		//----------------------
		GlsMat glsSrc(imgSrc);		//create GlsMat and  upload
		GlsMat glsDst;
		gls::copyRect(glsSrc, glsDst, rect);	//copy texture within rect
		glsDst.download(imgDst);		// download

		int errNum = 0;
		Mat roi = Mat(imgSrc, rect);
		if (!AreEqual<T>(roi, imgDst)) errNum = -1;

		//cout << roi << endl;
		//cout << imgDst << endl;
		//cout << imgDst - roi << endl;


		return errNum;
	}

	TEST_CLASS(UnitTest_glsCopyRect)
	{
	public:
		//glsCopyRect
		//! basic
		BEGIN_TEST_METHOD_ATTRIBUTE(glsCopyRect_CV_32FC1)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsCopyRect_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopyRect_CV_32FC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect<float>(CV_32FC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopyRect_CV_32FC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect<float>(CV_32FC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopyRect_CV_32FC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect<float>(CV_32FC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopyRect_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect<uchar>(CV_8UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopyRect_CV_16UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect<ushort>(CV_16UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopyRect_CV_32SC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect<int>(CV_32SC1);
			Assert::AreEqual(0, errNum);
		}


	};




	template <typename T>
	int test_glsTiled(int cvtype){
		const int width = 32;
		const int height = 24;
		//const int width = 12;
		//const int height = 8;
		Size blkNum = Size(2, 2);

		Mat imgSrc = Mat(Size(width, height), cvtype);
		Mat imgDst(imgSrc.size(), imgSrc.type());

		cout << "Size:" << imgSrc.size() << endl;

		//---------------------------------
		//init Src image
		FillRandU<T>(imgSrc);

		//----------------------
		GlsMat glsSrc(imgSrc);					//create GlsMat and  upload
		vector<vector<GlsMat>> glsDst;
		gls::tiled(glsSrc, glsDst, blkNum);		//copy texture

		if (glsDst.size() != blkNum.height) return -1;
		if (glsDst[0].size() != blkNum.width) return -1;


		for (int by = 0; by < blkNum.height; by++){
			for (int bx = 0; bx < blkNum.width ; bx++){
#if 0
				Mat tmp;
				glsDst[by][bx].download(tmp);					// download

				//cout << "(" << by << "," << bx << ")" << endl;
				//cout << tmp << endl;

				Rect rect(bx* (width / blkNum.width), by* (height / blkNum.height), (width / blkNum.width), (height / blkNum.height));
				Mat roi = Mat(imgDst, rect);
				tmp.copyTo(roi);
#else
				Rect rect(bx* (width / blkNum.width), by* (height / blkNum.height), (width / blkNum.width), (height / blkNum.height));
				Mat roi = Mat(imgDst, rect);
				glsDst[by][bx].download(roi);					// download
#endif
			}
		}


		int errNum = 0;
		if (!AreEqual<T>(imgSrc, imgDst)) errNum = -1;

		//cout << imgSrc << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgSrc << endl;


		return errNum;
	}

	TEST_CLASS(UnitTest_glsTiled)
	{
	public:
		//! basic
		BEGIN_TEST_METHOD_ATTRIBUTE(glsTiles_CV_32FC1)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsTiles_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsTiled<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsTiles_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsTiled<uchar>(CV_8UC1);
			Assert::AreEqual(0, errNum);
		}

	};

	template <typename T>
	int test_glsUntiled(int cvtype){
		//const int width = 32;
		//const int height = 24;
		const int width = 8;
		const int height = 6;
		Size blkNum = Size(2, 2);

		Mat imgSrc = Mat(Size(width, height), cvtype);
		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;

		//---------------------------------
		//init Src image
		FillRandU<T>(imgSrc);

		vector<vector<GlsMat>> glsSrc(blkNum.height, vector<GlsMat>(blkNum.width));

		for (int by = 0; by < blkNum.height; by++){
			for (int bx = 0; bx < blkNum.width; bx++){
				Rect rect(bx* (width / blkNum.width), by* (height / blkNum.height), (width / blkNum.width), (height / blkNum.height));
				Mat roi = Mat(imgSrc, rect);
				glsSrc[by][bx] = (GlsMat)roi;					// download
			}
		}


		GlsMat glsDst;
		gls::untiled(glsSrc, glsDst);			//copy texture  with untiling
		glsDst.download(imgDst);				// download

		int errNum = 0;
		if (!AreEqual<T>(imgSrc, imgDst)) errNum = -1;

		//cout << imgSrc << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgSrc << endl;


		return errNum;
	}

	TEST_CLASS(UnitTest_glsUntiled)
	{
	public:
		//! basic
		BEGIN_TEST_METHOD_ATTRIBUTE(glsUntiled_CV_32FC1)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsUntiled_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsUntiled<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsUntiled_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsUntiled<uchar>(CV_8UC1);
			Assert::AreEqual(0, errNum);
		}

	};


}