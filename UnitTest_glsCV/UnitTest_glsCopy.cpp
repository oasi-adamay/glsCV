#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


#include "glsCV.h"
#include "UnitTest_Common.h"



namespace UnitTest_glsCV
{

	template <typename T>
	int test_glsCopy(int cvtype, int flag = 0){
		const int width = 32;
		const int height = 24;
//		const int width = 8;
//		const int height = 6;
		Size blkNum;
		if (flag == 0)blkNum = Size(1, 1);
		else blkNum = Size(2, 2);

		Mat imgSrc = Mat(Size(width, height), cvtype);
		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;

		//---------------------------------
		//init Src image
//		cv::randu(imgSrc, Scalar(0), Scalar(255));
		{
			RNG rng(0xFFFFFFFF);
			for (int y = 0; y < imgSrc.rows; y++){
				for (int x = 0; x < imgSrc.cols; x++){
					T* pSrc = imgSrc.ptr<T>(y, x);
					for (int ch = 0; ch < imgSrc.channels(); ch++){
						*pSrc++ = randu<T>();
					}
				}
			}
		}


		//----------------------
//		glsMat glsSrc(imgSrc);		//create glsMat and  upload
		glsMat glsSrc(imgSrc.size(),imgSrc.type(),blkNum);
		glsSrc.CopyFrom(imgSrc);
		glsMat glsDst;
		glsCopy(glsSrc, glsDst);	//copy texture
		glsDst.CopyTo(imgDst);		// download

		int errNum = 0;
		if (!AreEqual(imgSrc, imgDst)) errNum = -1;

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

		if (flag == 2){
			width = 256;
			height = 256;
			rect = Rect(0, 0, width, height);
			blkNum = Size(2, 2);
		}


		Mat imgSrc = Mat(Size(width, height), cvtype);
		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;
		cout << "Rect:" << rect << endl;

		//---------------------------------
		//init Src image
//		cv::randu(imgSrc, Scalar(0), Scalar(255));
		{
			RNG rng(0xFFFFFFFF);
			for (int y = 0; y < imgSrc.rows; y++){
				for (int x = 0; x < imgSrc.cols; x++){
					T* pSrc = imgSrc.ptr<T>(y, x);
					for (int ch = 0; ch < imgSrc.channels(); ch++){
						*pSrc++ = randu<T>();
					}
				}
			}
		}


		//----------------------
		glsMat glsSrc(imgSrc);		//create glsMat and  upload
		glsMat glsDst;
		glsCopyRect(glsSrc, glsDst, rect, blkNum);	//copy texture within rect
		glsDst.CopyTo(imgDst);		// download

		int errNum = 0;
		Mat roi = Mat(imgSrc, rect);
		if (!AreEqual(roi, imgDst)) errNum = -1;

		//cout << roi << endl;
		//cout << imgDst << endl;
		//cout << imgDst - roi << endl;


		return errNum;
	}





	TEST_CLASS(UnitTest_glsCopy)
	{
	public:
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
			int errNum = test_glsCopy<float>(CV_32FC2,1);
			Assert::AreEqual(0, errNum);
		}


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

		TEST_METHOD(glsCopyRect_CV_32FC2_TILING)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect<float>(CV_32FC2, 2);
			Assert::AreEqual(0, errNum);
		}






	};


}