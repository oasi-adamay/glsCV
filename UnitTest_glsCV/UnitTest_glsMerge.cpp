#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


#include "glsCV.h"
#include "UnitTest_Common.h"



namespace UnitTest_glsCV
{

	template <typename T>
	int test_glsMerge(int cvtype, int cn,int flag = 0){
//		const int width = 32;
//		const int height = 24;
		const int width = 8;
		const int height = 6;
		Size blkNum;
		if (flag == 0)blkNum = Size(1, 1);
		else blkNum = Size(2, 2);

		vector<Mat> plnSrc(cn);
		for (int c = 0; c < cn; c++){
			plnSrc[c] = Mat(Size(width, height), cvtype);
			//init Src image
			FillRandU<T>(plnSrc[c]);
		}

		Mat imgRef;
		Mat imgDst;

		cout << "Size:" << plnSrc[0].size() << endl;

		cv::merge(plnSrc,imgRef);

		//----------------------
		vector<glsMat> plnGlsSrc(cn);
		for (int c = 0; c < cn; c++){
			plnGlsSrc[c] = glsMat(plnSrc[c]);
		}

		glsMat glsDst;
		glsMerge(plnGlsSrc, glsDst);	//copy texture

		glsDst.CopyTo(imgDst);		// download

		int errNum = 0;
		if (!AreEqual(imgRef, imgDst)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}




	TEST_CLASS(UnitTest_glsMerge)
	{
	public:
		//glsMerge
		TEST_METHOD(glsMerge_CV_8UC1_2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMerge<uchar>(CV_8UC1, 2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMerge_CV_8UC1_3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMerge<uchar>(CV_8UC1, 3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMerge_CV_8UC1_4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMerge<uchar>(CV_8UC1, 4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMerge_CV_32FC1_2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMerge<float>(CV_32FC1,2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMerge_CV_32FC1_2_TILED)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMerge<float>(CV_32FC1, 2 , 1);
			Assert::AreEqual(0, errNum);
		}

	};


}