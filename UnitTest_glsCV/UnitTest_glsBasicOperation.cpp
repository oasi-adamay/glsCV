#include "stdafx.h"
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Timer.h"
#include "glsCV.h"
#include "math.h"

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
		assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);
		int aInt = *(int*)&A;
		// Make aInt lexicographically ordered as a twos-complement int
		if (aInt < 0)
			aInt = 0x80000000 - aInt;
		// Make bInt lexicographically ordered as a twos-complement int
		int bInt = *(int*)&B;
		if (bInt < 0)
			bInt = 0x80000000 - bInt;
		int intDiff = abs(aInt - bInt);
		if (intDiff <= maxUlps)
			return true;
		return false;
	}



	int test_glsBasicOperation(const int width, const int height, const int flags){
		int ULPS = 4096;
		float EPS = 1e-3f;
		Mat imgSrc0 = Mat(Size(width, height), CV_32FC2);
		Mat imgSrc1 = Mat(Size(width, height), CV_32FC2);
		Mat imgDst = Mat::zeros(imgSrc0.size(), imgSrc0.type());
		Mat imgRef = Mat::zeros(imgSrc1.size(), imgSrc1.type());
		Scalar scalar;

		cout << "Size:" << imgSrc0.size() << endl;

		//---------------------------------
		//init Src image
		{
			RNG rng(0xFFFFFFFF);

			const int width = imgSrc0.cols;
			const int height = imgSrc0.rows;
			for (int y = 0; y < height; y++){
				for (int x = 0; x < width; x++){
					imgSrc0.at<Vec2f>(y, x) = Vec2f((float)rng.gaussian(1.0), (float)rng.gaussian(1.0));
					imgSrc1.at<Vec2f>(y, x) = Vec2f((float)rng.gaussian(1.0), (float)rng.gaussian(1.0));
				}
			}

			scalar[0] = (float)rng.gaussian(1.0);
			scalar[1] = (float)rng.gaussian(1.0);
			scalar[2] = (float)rng.gaussian(1.0);
			scalar[3] = (float)rng.gaussian(1.0);

		}


		{
			const int width = imgSrc1.cols;
			const int height = imgSrc1.rows;
			for (int y = 0; y < height; y++){
				for (int x = 0; x < width; x++){
					if (fabs(imgSrc1.at<Vec2f>(y, x)[0])<FLT_MIN) imgSrc1.at<Vec2f>(y, x)[0] = FLT_MIN;	//prevent div 0
					if (fabs(imgSrc1.at<Vec2f>(y, x)[1])<FLT_MIN) imgSrc1.at<Vec2f>(y, x)[1] = FLT_MIN;	//prevent div 0
				}
			}
		}


		//---------------------------------
		switch (flags){
		case(0) : cv::add(imgSrc0, imgSrc1, imgRef); break;
		case(1) : cv::subtract(imgSrc0, imgSrc1, imgRef); break;
		case(2) : cv::multiply(imgSrc0, imgSrc1, imgRef); break;
		case(3) : cv::divide(imgSrc0, imgSrc1, imgRef); break;
		case(4) : cv::add(scalar, imgSrc1, imgRef); break;
		case(5) : cv::subtract(scalar, imgSrc1, imgRef); break;
		case(6) : cv::multiply(scalar, imgSrc1, imgRef); break;
		case(7) : cv::divide(scalar, imgSrc1, imgRef); break;
		case(8) : cv::min(imgSrc0, imgSrc1, imgRef); break;
		case(9) : cv::max(imgSrc0, imgSrc1, imgRef); break;
		case(10) : cv::min(scalar, imgSrc1, imgRef); break;
		case(11) : cv::max(scalar, imgSrc1, imgRef); break;
		case(12) : cv::mulSpectrums(imgSrc0, imgSrc1, imgRef,0,false); break;
		case(13) : cv::mulSpectrums(imgSrc0, imgSrc1, imgRef, 0, true); break;
		case(14) :
			Mat corr;
			cv::mulSpectrums(imgSrc0, imgSrc1, corr, 0, true);
			vector<Mat> tmp(2);
			cv::split(corr, tmp);
			Mat mag;
			cv::magnitude(tmp[0], tmp[1], mag);
			cv::divide(tmp[0], mag, tmp[0]);
			cv::divide(tmp[1], mag, tmp[1]);
			cv::merge(tmp, imgRef);
			break;
		};

		//---------------------------------
		switch (flags){
		case(0) : glsAdd(imgSrc0, imgSrc1, imgDst); break;
		case(1) : glsSubtract(imgSrc0, imgSrc1, imgDst); break;
		case(2) : glsMultiply(imgSrc0, imgSrc1, imgDst); break;
		case(3) : glsDivide(imgSrc0, imgSrc1, imgDst); break;
		case(4) : glsAdd(scalar, imgSrc1, imgDst); break;
		case(5) : glsSubtract(scalar, imgSrc1, imgDst); break;
		case(6) : glsMultiply(scalar, imgSrc1, imgDst); break;
		case(7) : glsDivide(scalar, imgSrc1, imgDst); break;
		case(8) : glsMin(imgSrc0, imgSrc1, imgDst); break;
		case(9) : glsMax(imgSrc0, imgSrc1, imgDst); break;
		case(10) : glsMin(scalar, imgSrc1, imgDst); break;
		case(11) : glsMax(scalar, imgSrc1, imgDst); break;
		case(12) : glsMulSpectrums(imgSrc0, imgSrc1, imgDst, false); break;
		case(13) : glsMulSpectrums(imgSrc0, imgSrc1, imgDst, true); break;
		case(14) : glsMulSpectrumsPhaseOnly(imgSrc0, imgSrc1, imgDst); break;
		};

		//verify
		int errNum = 0;
		{
			//verify
			int width = imgSrc0.cols;
			int height = imgSrc0.rows;
			for (int y = 0; y < height; y++){
				for (int x = 0; x < width; x++){
					Vec2f ref = imgRef.at<Vec2f>(y, x);
					Vec2f dst = imgDst.at<Vec2f>(y, x);
					Vec2f err = dst - ref;
					if (!AlmostEqualUlpsAbsEps(ref[0], dst[0], ULPS, EPS)){
						cout << cv::format("r(%4d,%4d)\t", x, y);
						cout << cv::format("%8g\t", dst[0]);
						cout << cv::format("%8g\t", ref[0]);
						cout << cv::format("%8g\t", err[0]);
						cout << endl;
						errNum++;
					}
					if (!AlmostEqualUlpsAbsEps(ref[1], dst[1], ULPS, EPS)){
						cout << cv::format("i(%4d,%4d)\t", x, y);
						cout << cv::format("%8g\t", dst[1]);
						cout << cv::format("%8g\t", ref[1]);
						cout << cv::format("%8g\t", err[1]);
						cout << endl;
						errNum++;
					}
				}
			}
		}
		cout << "errNum:" << errNum << endl;

		return errNum;
	}


	TEST_CLASS(UnitTest_glsBasicOperation)
	{
	public:

		TEST_METHOD(TestMethod_glsAdd)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width, height, 0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsSubtract)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width, height, 1);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(TestMethod_glsMultiply)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width,height,2);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsDivide)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width, height, 3);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsAdd_S)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width, height, 4);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsSubtract_S)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width, height, 5);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(TestMethod_glsMultiply_S)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width, height, 6);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsDivide_S)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width, height, 7);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMin)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width, height, 8);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMax)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width, height, 9);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMin_S)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width, height, 10);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMax_S)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width, height, 11);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMulSpectrums)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width, height, 12);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMulSpectrumsConj)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width, height, 13);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMulSpectrumsPhaseOnly)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 32;
			int errNum = test_glsBasicOperation(width, height, 14);
			Assert::AreEqual(0, errNum);
		}



	};
}