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
	int test_glsCopy(int cvtype, Size size = Size(32,24), Size blkNum = Size(1,1)){

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
//		glsMat glsSrc(imgSrc);		//create glsMat and  upload
		glsMat glsSrc(imgSrc.size(),imgSrc.type(),blkNum);
		glsSrc.CopyFrom(imgSrc);
		glsMat glsDst;
		{
			_TMR_("-glsCopy:\t");
			glsCopy(glsSrc, glsDst);	//copy texture
		}


		glsDst.CopyTo(imgDst);		// download

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst)) errNum = -1;

		//cout << imgSrc << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgSrc << endl;


		return errNum;
	}

	template <typename T>
	int test_glsCopyTiled(int cvtype){
		const int width = 32;
		const int height = 24;
		Size blkNum = Size(2, 2);

		Mat imgSrc = Mat(Size(width, height), cvtype);
		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;

		//---------------------------------
		//init Src image
		FillRandU<T>(imgSrc);

		//----------------------
		glsMat glsSrc(imgSrc);					//create glsMat and  upload
		glsMat glsDst;
		glsCopyTiled(glsSrc, glsDst, blkNum);	//copy texture
		glsDst.CopyTo(imgDst);					// download

		int errNum = 0;
		if (glsDst.blkNum() != blkNum) errNum = -1;
		if (!AreEqual<T>(imgSrc, imgDst)) errNum = -1;

		//cout << imgSrc << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgSrc << endl;


		return errNum;
	}

	template <typename T>
	int test_glsCopyUntiled(int cvtype){
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

		//----------------------
		glsMat glsSrc(imgSrc.size(), imgSrc.type(), blkNum);
		glsSrc.CopyFrom(imgSrc);

		glsMat glsDst;
		glsCopyUntiled(glsSrc, glsDst);		//copy texture  with untiling
		glsDst.CopyTo(imgDst);				// download

		int errNum = 0;
		if (glsDst.blkNum() != Size(1,1)) errNum = -1;
		if (!AreEqual<T>(imgSrc, imgDst)) errNum = -1;

		//cout << imgSrc << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgSrc << endl;


		return errNum;
	}



	template <typename T>
	int test_glsCopyRect(int cvtype, int flag = 0){
		int width = 32;
		int height = 24;
//		const Rect rect(4, 3, 15, 13);
		Rect rect(4, 3, 8, 6);
		Size blkNum;
		if (flag == 0) blkNum = Size(1, 1);
		else blkNum = Size(2, 2);


		Mat imgSrc = Mat(Size(width, height), cvtype);
		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;
		cout << "Rect:" << rect << endl;

		//---------------------------------
		//init Src image
		FillRandU<T>(imgSrc);


		//----------------------
		glsMat glsSrc(imgSrc);		//create glsMat and  upload
		glsMat glsDst;
		glsCopyRect(glsSrc, glsDst, rect, blkNum);	//copy texture within rect
		glsDst.CopyTo(imgDst);		// download

		int errNum = 0;
		Mat roi = Mat(imgSrc, rect);
		if (!AreEqual<T>(roi, imgDst)) errNum = -1;

		//cout << roi << endl;
		//cout << imgDst << endl;
		//cout << imgDst - roi << endl;


		return errNum;
	}





	TEST_CLASS(UnitTest_glsCopy)
	{
	public:
		//glsCopy

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
		TEST_METHOD(glsCopy_CV_32FC2_TILING)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy<float>(CV_32FC2, Size(32, 24), Size(2, 2));
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsCopy_CV_32FC1_1024x1024)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy<float>(CV_32FC1, Size(1024, 1024));
			Assert::AreEqual(0, errNum);
		}


		//glsCopyTiled
		TEST_METHOD(glsCopyTiled_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyTiled<uchar>(CV_8UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopyTiled_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyTiled<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}

		//glsCopyUntiled
		TEST_METHOD(glsCopyUntiled_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyUntiled<uchar>(CV_8UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopyUntiled_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyUntiled<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}

		//glsCopyRect
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

		// tiling
		TEST_METHOD(glsCopyRect_CV_32FC1_TILING)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect<float>(CV_32FC1,1);
			Assert::AreEqual(0, errNum);
		}



	};


}