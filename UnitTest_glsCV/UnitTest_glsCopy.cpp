#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


#include "glsCV.h"
#include "UnitTest_Common.h"



namespace UnitTest_glsCV
{

	int test_glsCopy(int cvtype){
		const int width = 32;
		const int height = 24;

		Mat imgSrc = Mat(Size(width, height), cvtype);
		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;

		//---------------------------------
		//init Src image
		cv::randu(imgSrc, Scalar(0), Scalar(255));

		//----------------------
		glsMat glsSrc(imgSrc);		//create glsMat and  upload
		glsMat glsDst;
		glsCopy(glsSrc, glsDst);	//copy texture
		glsDst.CopyTo(imgDst);		// download

		int errNum = 0;
		if (!AreEqual(imgSrc, imgDst)) errNum = -1;


		return errNum;
	}

	int test_glsCopyRect(int cvtype,int flag = 0){
		const int width = 32;
		const int height = 24;
//		const Rect rect(4, 3, 15, 13);
		const Rect rect(4, 3, 8, 6);
		Size blkNum;
		if (flag == 0) blkNum = Size(1, 1);
		else blkNum = Size(2, 2);

		Mat imgSrc = Mat(Size(width, height), cvtype);
		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;
		cout << "Rect:" << rect << endl;

		//---------------------------------
		//init Src image
		cv::randu(imgSrc, Scalar(0), Scalar(255));

		//----------------------
		glsMat glsSrc(imgSrc);		//create glsMat and  upload
		glsMat glsDst;
		glsCopy(glsSrc, glsDst, rect, blkNum);	//copy texture within rect
		glsDst.CopyTo(imgDst);		// download

		int errNum = 0;
		Mat roi = Mat(imgSrc, rect);
		if (!AreEqual(roi, imgDst)) errNum = -1;

		return errNum;
	}





	TEST_CLASS(UnitTest_glsCopy)
	{
	public:
		TEST_METHOD(glsCopy_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopy_CV_32FC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy(CV_32FC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopy_CV_32FC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy(CV_32FC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopy_CV_32FC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy(CV_32FC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopy_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy(CV_8UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopy_CV_16UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy(CV_16UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopy_CV_32SC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopy(CV_32SC1);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsCopyRect_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopyRect_CV_32FC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect(CV_32FC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopyRect_CV_32FC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect(CV_32FC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopyRect_CV_32FC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect(CV_32FC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopyRect_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect(CV_8UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopyRect_CV_16UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect(CV_16UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCopyRect_CV_32SC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect(CV_32SC1);
			Assert::AreEqual(0, errNum);
		}

		// tiling
		TEST_METHOD(glsCopyRect_CV_32FC1_TILING)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCopyRect(CV_32FC1,1);
			Assert::AreEqual(0, errNum);
		}







	};


}