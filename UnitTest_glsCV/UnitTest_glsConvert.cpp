#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "HookCoutCerr.h"
#include "Timer.h"

#include "glsCV.h"
#include "UnitTest_Common.h"



namespace UnitTest_glsCV
{
	template <typename Ts, typename Td>
	int test_glsConvert(const int typeS, const int typeD, const float scale = 1.0f){
		const int width = 32;
		const int height = 24;
		Mat imgSrc = Mat(Size(width, height), typeS);
		Mat imgRef; 

		cout << "Size:" << imgSrc.size() << endl;
		//---------------------------------
		//init Src image
		FillRandU<Ts>(imgSrc);

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
		if (!AreEqual<Td>(imgRef, imgDst, 0)) errNum = -1;
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
		FillRandU<Ts>(imgSrc);

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
		if (!AreEqual<Td>(imgRef, imgDst, maxUlps)) errNum = -1;

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