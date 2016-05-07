#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "HookCoutCerr.h"
#include "Timer.h"

#include "glsCV.h"
#include "UnitTest_Common.h"

//Lib 
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "glfw3dll.lib")

#pragma comment (lib, "glsCV.lib")


namespace UnitTest_glsCV
{

	template <typename T>
	int test_glsMat_CopyTo(const int cvtype, const Size size = Size(32, 24), const Size blkNum = Size(1, 1)){
		Mat imgSrc = Mat(size, cvtype);
		Mat imgDst = Mat::zeros(imgSrc.size(), imgSrc.type());
//		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;
		cout << "blkNum:" << blkNum << endl;

		//---------------------------------
		//init Src image
		FillRandU<T>(imgSrc);

		glsMat glsSrc(imgSrc, blkNum);		//create glsMat and  upload
		glsSrc.CopyTo(imgDst);		// download


		//verify
		int errNum = 0;
		if (!AreEqual<T>(imgSrc, imgDst)) errNum = -1;

		//if (errNum){
		//	cout << imgSrc << endl;
		//	cout << imgDst << endl;
		//}

		return errNum;
	}

	TEST_CLASS(UnitTest_glsMat)
	{
	public:
		TEST_METHOD(glsMat_Copy_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<uchar>(CV_8UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_8UC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<uchar>(CV_8UC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_8UC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<uchar>(CV_8UC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_8UC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<uchar>(CV_8UC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_8SC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<schar>(CV_8SC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_8SC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<schar>(CV_8SC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_8SC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<schar>(CV_8SC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_8SC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<schar>(CV_8SC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_16SC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<short>(CV_16SC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_16SC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<short>(CV_16SC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_16SC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<short>(CV_16SC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_16SC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<short>(CV_16SC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_16UC1)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<ushort>(CV_16UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_16UC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<ushort>(CV_16UC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_16UC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<ushort>(CV_16UC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_16UC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<ushort>(CV_16UC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_32SC1)
		{
			cout << __FUNCTION__ << endl;
			const int width  = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<int>(CV_32SC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_32SC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<int>(CV_32SC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_32SC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<int>(CV_32SC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_32SC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<int>(CV_32SC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_32FC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<float>(CV_32FC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_32FC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<float>(CV_32FC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_32FC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<float>(CV_32FC4);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsMat_Copy_CV_8UC1_4x8)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<uchar>(CV_8UC1, Size(4, 8));
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMat_Copy_CV_8UC1_4x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<uchar>(CV_8UC1, Size(4, 3));
			Assert::AreEqual(0, errNum);
		}

		////@ To do Error!! 
		//TEST_METHOD(glsMat_Copy_CV_8UC1_3x4)
		//{
		//	cout << __FUNCTION__ << endl;
		//	int errNum = test_glsMat_CopyTo<uchar>(CV_8UC1, Size(3, 4));
		//	Assert::AreEqual(0, errNum);
		//}

		TEST_METHOD(glsMat_Copy_CV_32FC1_5x5)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<float>(CV_32FC1, Size(5, 5));
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsMat_Copy_CV_32FC1_1024x1024)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<float>(CV_32FC1, Size(1024, 1024));
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsMat_Copy_CV_32FC1_1024x1024_2x2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMat_CopyTo<float>(CV_32FC1, Size(1024, 1024), Size(2, 2));
			Assert::AreEqual(0, errNum);
		}



	};


}