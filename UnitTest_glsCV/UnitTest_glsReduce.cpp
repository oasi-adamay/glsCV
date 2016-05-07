#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


#include "glsCV.h"
#include "UnitTest_Common.h"



namespace UnitTest_glsCV
{

	template <typename T>
	int test_glsReduce(int cvtype, int dim, int reduceOp, int ulps = 0){
		const int width = 24;
		const int height = 32;
//		const int width = 8;
//		const int height = 6;

//		Size blkNum(1,1);
		Size blkNum(2, 2);


		Mat imgSrc(Size(width, height), cvtype);
		FillRandU<T>(imgSrc);

		Mat imgRef;
		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;

		cv::reduce(imgSrc, imgRef, dim, reduceOp);

		glsMat glsSrc(imgSrc.size(), imgSrc.type(),blkNum);
		glsSrc.CopyFrom(imgSrc);

		glsMat glsDst;

		glsReduce(glsSrc, glsDst, dim, reduceOp);

		glsDst.CopyTo(imgDst);		// download

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst, ulps)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsReduce)
	{
	public:
		//glsMerge
		TEST_METHOD(glsReduce_CV_32FC1_0_CV_REDUCE_MAX)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC1, 0, CV_REDUCE_MAX);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsReduce_CV_32FC1_1_CV_REDUCE_MAX)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC1, 1, CV_REDUCE_MAX);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsReduce_CV_32FC1_0_CV_REDUCE_MIN)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC1, 0, CV_REDUCE_MIN);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsReduce_CV_32FC1_1_CV_REDUCE_MIN)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC1, 1, CV_REDUCE_MIN);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsReduce_CV_32FC1_0_CV_REDUCE_SUM)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC1, 0, CV_REDUCE_SUM,16);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsReduce_CV_32FC1_1_CV_REDUCE_SUM)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC1, 1, CV_REDUCE_SUM, 16);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsReduce_CV_32FC1_0_CV_REDUCE_AVG)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC1, 0, CV_REDUCE_AVG, 16);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsReduce_CV_32FC1_1_CV_REDUCE_AVG)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC1, 1, CV_REDUCE_AVG, 16);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsReduce_CV_32FC4_0_CV_REDUCE_MAX)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC4, 0, CV_REDUCE_MAX);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsReduce_CV_32FC4_1_CV_REDUCE_MAX)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC4, 1, CV_REDUCE_MAX);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsReduce_CV_32FC4_0_CV_REDUCE_MIN)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC4, 0, CV_REDUCE_MIN);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsReduce_CV_32FC4_1_CV_REDUCE_MIN)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC4, 1, CV_REDUCE_MIN);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsReduce_CV_32FC4_0_CV_REDUCE_SUM)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC4, 0, CV_REDUCE_SUM, 16);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsReduce_CV_32FC4_1_CV_REDUCE_SUM)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC4, 1, CV_REDUCE_SUM, 16);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsReduce_CV_32FC4_0_CV_REDUCE_AVG)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC4, 0, CV_REDUCE_AVG, 16);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsReduce_CV_32FC4_1_CV_REDUCE_AVG)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsReduce<float>(CV_32FC4, 1, CV_REDUCE_AVG, 16);
			Assert::AreEqual(0, errNum);
		}

	};

	template <typename T>
	int test_glsMinMaxLoc(int cvtype){
		const int width = 24;
		const int height = 32;
		//		const int width = 8;
		//		const int height = 6;

		//		Size blkNum(1,1);
		Size blkNum(2, 2);


		Mat imgSrc(Size(width, height), cvtype);
		FillRandU<T>(imgSrc);

		cout << "Size:" << imgSrc.size() << endl;

		double maxValRef;
		double minValRef;
		double maxVal;
		double minVal;

		cv::minMaxLoc(imgSrc, &minValRef, &maxValRef);


		glsMat glsSrc(imgSrc.size(), imgSrc.type(), blkNum);
		glsSrc.CopyFrom(imgSrc);
		glsMinMaxLoc(glsSrc, &minVal, &maxVal);

		cout << maxValRef << "," << maxVal << endl;
		cout << minValRef << "," << minVal << endl;

		int errNum = 0;
		if (!AreEqual<float>((float)maxValRef, (float)maxVal, 0))errNum++;
		if (!AreEqual<float>((float)minValRef, (float)minVal, 0))errNum++;

		return errNum;
	}

	TEST_CLASS(UnitTest_glsMinMaxLoc)
	{
	public:
		//glsMerge
		TEST_METHOD(glsMinMaxLoc_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMinMaxLoc<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
	};

}