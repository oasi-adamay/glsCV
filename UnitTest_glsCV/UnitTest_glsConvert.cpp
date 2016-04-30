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

	template <typename Ts, typename Td>
	int test_glsConvert(const int typeS, const int typeD, const float scale = 1.0f,  const int width = 32, const int height = 24){
		Mat imgSrc = Mat(Size(width, height), typeS);
		Mat imgRef; 
		Mat imgDst = Mat::zeros(imgRef.size(), imgRef.type());

		cout << "Size:" << imgSrc.size() << endl;

		//---------------------------------
		//init Src image
		{
			RNG rng(0xFFFFFFFF);
			for (int y = 0; y < imgSrc.rows; y++){
				for (int x = 0; x < imgSrc.cols; x++){
					Ts* pSrc = imgSrc.ptr<Ts>(y, x);
					for (int ch = 0; ch < imgSrc.channels(); ch++){
						*pSrc++ = randu<Ts>();
					}
				}
			}
		}
		imgSrc.convertTo(imgRef, typeD, scale);

		glsMat glsSrc(imgSrc);		//create glsMat and  upload
		glsMat glsDst(imgRef, false); //create glsMat not upload!

		glsConvert(glsSrc, glsDst, scale);
		glsDst.CopyTo(imgDst);		// download

		//verify
		int errNum = 0;
		{
			//verify
			for (int y = 0; y < imgRef.rows; y++){
				for (int x = 0; x < imgRef.cols; x++){
					Td* pRef = imgRef.ptr<Td>(y, x);
					Td* pDst = imgDst.ptr<Td>(y, x);
					for (int ch = 0; ch < imgRef.channels(); ch++){
						Td ref = *pRef++;
						Td dst = *pDst++;
						if (ref != dst){
							cout << cv::format("(%4d,%4d,%4d)\t", y, x, ch);
							cout << ref << "\t";
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

	TEST_CLASS(UnitTest_glsConvert)
	{
	public:
		TEST_METHOD(TestMethod_glsConvert_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<uchar, float>(CV_8UC1, CV_32FC1,1.0f/256.0f);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsConvert_CV_8UC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<uchar, float>(CV_8UC2, CV_32FC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsConvert_CV_8UC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<uchar, float>(CV_8UC3, CV_32FC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsConvert_CV_8UC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<uchar, float>(CV_8UC4, CV_32FC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsConvert_CV_16UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<ushort, float>(CV_16UC1, CV_32FC1,  1.0f / 65536.0f);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsConvert_CV_32SC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<int, float>(CV_32SC1, CV_32FC1);
			Assert::AreEqual(0, errNum);
		}


	};


}