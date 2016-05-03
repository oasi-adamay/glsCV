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
	static
		bool AlmostEqualUlpsAbsEps(float A, float B, int maxUlps, float maxDiff)
	{
		// Check if the numbers are really close -- needed
		// when comparing numbers near zero.
		float absDiff = fabs(A - B);
		if (absDiff <= maxDiff)
			return true;

		// Make sure maxUlps is non-negative and small enough that the
		// default NAN won't compare as equal to anything.
		GLS_Assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);
		int aInt = *(int*)&A;
		// Make aInt lexicographically ordered as a twos-complement int
		if (aInt < 0)
			aInt = 0x80000000 - aInt;
		// Make bInt lexicographically ordered as a twos-complement int
		int bInt = *(int*)&B;
		if (bInt < 0)
			bInt = 0x80000000 - bInt;
		int intDiff = abs(aInt - bInt);
		if (intDiff <= maxUlps) return true;

		cout << "A:" << A << "\t";
		cout << "B:" << B << "\t";
		cout << "intDiff:" << intDiff << "\t";
		cout << endl;

		return false;
	}

	template <typename T>
	static bool AreEqual(T val0, T val1, int maxUlps){
		return val0 == val1;
	}

	template<> static bool AreEqual<float>(float val0, float val1, int maxUlps){
		return AlmostEqualUlpsAbsEps(val0, val1, maxUlps, FLT_MIN);
	}


	template <typename Ts, typename Td>
	int test_glsConvert(const int typeS, const int typeD, const float scale = 1.0f){
		const int width = 32;
		const int height = 24;
		Mat imgSrc = Mat(Size(width, height), typeS);
		Mat imgRef; 

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
		Mat imgDst = Mat::zeros(imgRef.size(), imgRef.type());

		glsMat glsSrc(imgSrc);		//create glsMat and  upload
//		glsMat glsDst(imgRef, false); //create glsMat not upload!
		glsMat glsDst;

		glsConvert(glsSrc, glsDst, scale);
		glsDst.CopyTo(imgDst);		// download

		if (imgDst.size() != imgRef.size()){
			cout << "imgDst.size() != imgRef.size()" << endl;
			return -1;
		}
		if (imgDst.type() != imgRef.type()){
			cout << "imgDst.type() != imgRef.type()" << endl;
			return -1;
		}


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
		TEST_METHOD(glsConvert_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<uchar, float>(CV_8UC1, CV_32FC1,1.0f/256.0f);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvert_CV_8UC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<uchar, float>(CV_8UC2, CV_32FC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvert_CV_8UC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<uchar, float>(CV_8UC3, CV_32FC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvert_CV_8UC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<uchar, float>(CV_8UC4, CV_32FC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvert_CV_16UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<ushort, float>(CV_16UC1, CV_32FC1,  1.0f / 65536.0f);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvert_CV_32SC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<int, float>(CV_32SC1, CV_32FC1);
			Assert::AreEqual(0, errNum);
		}


	};




	template <typename Ts, typename Td>
	int test_glsCvtColor(const int typeS, const int code, int maxUlps = 2){
		const int width = 32;
		const int height = 24;
		Mat imgSrc = Mat(Size(width, height), typeS);
		Mat imgRef;

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
		cv::cvtColor(imgSrc, imgRef, code);

		Mat imgDst = Mat::zeros(imgRef.size(), imgRef.type());

		glsMat glsSrc(imgSrc);		//create glsMat and  upload
//		glsMat glsDst(imgRef, false); //create glsMat not upload!
		glsMat glsDst;				 

		glsCvtColor(glsSrc, glsDst, code);
		glsDst.CopyTo(imgDst);		// download

		if (imgDst.size() != imgRef.size()){
			cout << "imgDst.size() != imgRef.size()" << endl;
			return -1;
		}
		if (imgDst.type() != imgRef.type()){
			cout << "imgDst.type() != imgRef.type()" << endl;
			return -1;
		}


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
						if (!AreEqual<Td>(ref, dst, maxUlps)){
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

	TEST_CLASS(UnitTest_glsCvtColor)
	{
	public:
		TEST_METHOD(glsCvtColor_CV_32FC3_BGR2RGB)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCvtColor<float, float>(CV_32FC3, CV_BGR2RGB);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCvtColor_CV_32FC3_RGB2BGR)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCvtColor<float, float>(CV_32FC3, CV_RGB2BGR);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCvtColor_CV_32FC3_RGB2GARY)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCvtColor<float, float>(CV_32FC3, CV_RGB2GRAY);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCvtColor_CV_32FC3_BGR2GARY)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCvtColor<float, float>(CV_32FC3, CV_BGR2GRAY);
			Assert::AreEqual(0, errNum);
		}

	};


}