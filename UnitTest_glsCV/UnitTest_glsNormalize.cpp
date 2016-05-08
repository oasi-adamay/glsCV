#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


#include "glsCV.h"
#include "UnitTest_Common.h"



namespace UnitTest_glsCV
{

	template <typename T>
	int test_glsNormalize(int cvtype, double alpha, double beta ,int normType, int ulps = 0){
		const int width = 24;
		const int height = 32;
//		const int width = 8;
//		const int height = 6;



		Mat imgSrc(Size(width, height), cvtype);
		FillRandU<T>(imgSrc);

		Mat imgRef;
		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;

		cv::normalize(imgSrc, imgRef, alpha, beta, normType);

		glsMat glsSrc(imgSrc);

		glsMat glsDst;

		glsNormalize(glsSrc, glsDst, alpha, beta, normType);

		glsDst.CopyTo(imgDst);		// download

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst, ulps)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsNormalize)
	{
	public:
		//glsNormalize
		TEST_METHOD(glsNormalize_CV_32FC1_0_1_NORM_MINMAX)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNormalize<float>(CV_32FC1, 0.0, 1.0, NORM_MINMAX,4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsNormalize_CV_32FC1_05_2_NORM_MINMAX)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNormalize<float>(CV_32FC1, 0.5, 2.0, NORM_MINMAX,4);
			Assert::AreEqual(0, errNum);
		}

	};
}