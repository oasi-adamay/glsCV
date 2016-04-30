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
		if (intDiff <= maxUlps) return true;

		cout << "A:" << A << "\t";
		cout << "B:" << B << "\t";
		cout << "intDiff:" << intDiff << "\t";
		cout << endl;
			
		return false;
	}

	template <typename T>
	bool AreEqual(T val0, T val1, int maxUlps){
		return val0 == val1;
	}

	template<> bool AreEqual<float>(float val0, float val1, int maxUlps){
		return AlmostEqualUlpsAbsEps(val0, val1, maxUlps, FLT_MIN);
	}

	template <typename T>
	int test_glsBasicOperationT(const int cvtype, int flags, int maxUlps = 1){
		const int width = 32;
		const int height = 24;

		int errNum = 0;

		Mat imgSrc0 = Mat(Size(width, height), cvtype);
		Mat imgSrc1 = Mat(Size(width, height), cvtype);
		Mat imgDst = Mat::zeros(imgSrc0.size(), imgSrc0.type());
		Mat imgRef = Mat::zeros(imgSrc0.size(), imgSrc0.type());
		Scalar scalar;

		cout << "Size:" << imgSrc0.size() << endl;

		//---------------------------------
		//init Src image
		{

			RNG rng(0xFFFFFFFF);
			for (int y = 0; y < imgSrc0.rows; y++){
				for (int x = 0; x < imgSrc0.cols; x++){
					T* pSrc0 = imgSrc0.ptr<T>(y, x);
					T* pSrc1 = imgSrc1.ptr<T>(y, x);
					for (int ch = 0; ch < imgSrc0.channels(); ch++){
						*pSrc0++ = randu<T>();
						*pSrc1++ = randu<T>();
					}
				}
			}

			//prevent div by 0
			for (int y = 0; y < imgSrc1.rows; y++){
				for (int x = 0; x < imgSrc1.cols; x++){
					T* pSrc1 = imgSrc1.ptr<T>(y, x);
					for (int ch = 0; ch < imgSrc0.channels(); ch++){
						if (fabs((float)(*pSrc1)) < FLT_MIN) *pSrc1++ = 1;
					}
				}
			}

			scalar[0] = (float)rng.gaussian(1.0);
			scalar[1] = (float)rng.gaussian(1.0);
			scalar[2] = (float)rng.gaussian(1.0);
			scalar[3] = (float)rng.gaussian(1.0);

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
		case(12) : cv::mulSpectrums(imgSrc0, imgSrc1, imgRef, 0, false); break;
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
		{
			//verify
			for (int y = 0; y < imgRef.rows; y++){
				for (int x = 0; x < imgRef.cols; x++){
					T* pRef = imgRef.ptr<T>(y, x);
					T* pDst = imgDst.ptr<T>(y, x);
					for (int ch = 0; ch < imgRef.channels(); ch++){
						T ref = *pRef++;
						T dst = *pDst++;
						if (!AreEqual<T>(ref, dst, maxUlps)){
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



	TEST_CLASS(UnitTest_glsBasicOperation)
	{
	public:

		TEST_METHOD(TestMethod_glsAdd)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC4,0);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsSubtract)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC3,1);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(TestMethod_glsMultiply)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2,2);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsDivide)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, 3, 4);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsAdd_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, 4);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsSubtract_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, 5);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(TestMethod_glsMultiply_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, 6);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsDivide_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, 7,4);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMin)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, 8);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMax)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1,  9);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMin_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, 10);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMax_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, 11);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMulSpectrums)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2, 12 , 1024);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMulSpectrumsConj)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2, 13 , 128);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMulSpectrumsPhaseOnly)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2, 14 , 128);
			Assert::AreEqual(0, errNum);
		}



	};
}