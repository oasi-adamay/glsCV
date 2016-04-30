#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "HookCoutCerr.h"
#include "Timer.h"

#include "glsCV.h"

//Lib 
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "glfw3dll.lib")

#pragma comment (lib, "glsCV.lib")


namespace UnitTest_glsCV
{

	template <typename T>
	int test_glsMat_CopyTo(const int width, const int height, const int cvtype){
		Mat imgSrc = Mat(Size(width, height), cvtype);
		Mat imgDst = Mat::zeros(imgSrc.size(), imgSrc.type());

		cout << "Size:" << imgSrc.size() << endl;

		//---------------------------------
		//init Src image
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

		glsMat glsSrc(imgSrc);		//create glsMat and  upload
		glsSrc.CopyTo(imgDst);		// download

		//verify
		int errNum = 0;
		{
			//verify
			for (int y = 0; y < imgSrc.rows; y++){
				for (int x = 0; x < imgSrc.cols; x++){
					T* pSrc = imgSrc.ptr<T>(y, x);
					T* pDst = imgDst.ptr<T>(y, x);
					for (int ch = 0; ch < imgSrc.channels(); ch++){
						T src = *pSrc++;
						T dst = *pDst++;
						if (src != dst){
							cout << cv::format("(%4d,%4d,%4d)\t", y, x, ch);
							cout << src << "\t";
							cout << dst << "\t";
							cout << endl;
							errNum++;
						}
					}
				}
			}
		}
		cout << "errNum:" << errNum << endl;

		return errNum;
	}

	TEST_CLASS(UnitTest_glsMat)
	{
	public:
		TEST_METHOD(TestMethod_glsMat_Copy_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			const int width  = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<uchar>(width, height, CV_8UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_8UC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<uchar>(width, height, CV_8UC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_8UC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<uchar>(width, height, CV_8UC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_8UC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<uchar>(width, height, CV_8UC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_8SC1)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<schar>(width, height, CV_8SC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_8SC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<schar>(width, height, CV_8SC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_8SC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<schar>(width, height, CV_8SC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_8SC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<schar>(width, height, CV_8SC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_16SC1)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<short>(width, height, CV_16SC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_16SC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<short>(width, height, CV_16SC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_16SC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<short>(width, height, CV_16SC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_16SC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<short>(width, height, CV_16SC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_16UC1)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<ushort>(width, height, CV_16UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_16UC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<ushort>(width, height, CV_16UC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_16UC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<ushort>(width, height, CV_16UC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_16UC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<ushort>(width, height, CV_16UC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_32SC1)
		{
			cout << __FUNCTION__ << endl;
			const int width  = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<int>(width, height, CV_32SC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_32SC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<int>(width, height, CV_32SC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_32SC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<int>(width, height, CV_32SC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_32SC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<int>(width, height, CV_32SC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<float>(width, height, CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_32FC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<float>(width, height, CV_32FC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_32FC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<float>(width, height, CV_32FC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMat_Copy_CV_32FC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_glsMat_CopyTo<float>(width, height, CV_32FC4);
			Assert::AreEqual(0, errNum);
		}

	};


}