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
	static bool AreEqual(T val0, T val1, int maxUlps){
		return val0 == val1;
	}

	template<> static bool AreEqual<float>(float val0, float val1, int maxUlps){
		return AlmostEqualUlpsAbsEps(val0, val1, maxUlps, FLT_MIN);
	}

	enum E_TEST {
		ADD,
		SUB,
		MUL,
		DIV,
		MIN,
		MAX,
		ADD_S,
		SUB_S,
		MUL_S,
		DIV_S,
		MIN_S,
		MAX_S,
		MUL_SPECTRUMS,
		MUL_SPECTRUMS_CONJ,
		MUL_SPECTRUMS_POC,
		MAG_SPECTRUMS,
		LOG,
		EXP,
		POW,
	};

	template <typename T>
	int test_glsBasicOperationT(const int cvtype, int flags, int maxUlps = 2){
		const int width = 32;
		const int height = 24;

		int errNum = 0;

		Mat imgSrc0 = Mat(Size(width, height), cvtype);
		Mat imgSrc1 = Mat(Size(width, height), cvtype);
//		Mat imgDst = Mat::zeros(imgSrc0.size(), imgSrc0.type());
//		Mat imgRef = Mat::zeros(imgSrc0.size(), imgSrc0.type());
		Mat imgDst;
		Mat imgRef;
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
		case(E_TEST::ADD) :
			cv::add(imgSrc0, imgSrc1, imgRef); 
			glsAdd(imgSrc0, imgSrc1, imgDst);
			break;
		case(E_TEST::SUB):
			cv::subtract(imgSrc0, imgSrc1, imgRef); 
			glsSubtract(imgSrc0, imgSrc1, imgDst); 
			break;
		case(E_TEST::MUL) :
			cv::multiply(imgSrc0, imgSrc1, imgRef); 
			glsMultiply(imgSrc0, imgSrc1, imgDst);
			break;
		case(E_TEST::DIV) :
			cv::divide(imgSrc0, imgSrc1, imgRef); 
			glsDivide(imgSrc0, imgSrc1, imgDst);
			break;
		case(E_TEST::ADD_S) :
			cv::add(scalar, imgSrc1, imgRef); 
			glsAdd(scalar, imgSrc1, imgDst);
			break;
		case(E_TEST::SUB_S) :
			cv::subtract(scalar, imgSrc1, imgRef);
			glsSubtract(scalar, imgSrc1, imgDst);
			break;
		case(E_TEST::MUL_S) :
			cv::multiply(scalar, imgSrc1, imgRef); 
			glsMultiply(scalar, imgSrc1, imgDst);
			break;
		case(E_TEST::DIV_S) :
			cv::divide(scalar, imgSrc1, imgRef);
			glsDivide(scalar, imgSrc1, imgDst);
			break;
		case(E_TEST::MIN) :
			cv::min(imgSrc0, imgSrc1, imgRef); 
			glsMin(imgSrc0, imgSrc1, imgDst);
			break;
		case(E_TEST::MAX) :
			cv::max(imgSrc0, imgSrc1, imgRef);
			glsMax(imgSrc0, imgSrc1, imgDst);
			break;
		case(E_TEST::MIN_S) :
			cv::min(scalar, imgSrc1, imgRef); 
			glsMin(scalar, imgSrc1, imgDst);
			break;
		case(E_TEST::MAX_S) :
			cv::max(scalar, imgSrc1, imgRef); 
			glsMax(scalar, imgSrc1, imgDst);
			break;
		case(E_TEST::MUL_SPECTRUMS) :
			cv::mulSpectrums(imgSrc0, imgSrc1, imgRef, 0, false); 
			glsMulSpectrums(imgSrc0, imgSrc1, imgDst, false);
			break;
		case(E_TEST::MUL_SPECTRUMS_CONJ) :
			cv::mulSpectrums(imgSrc0, imgSrc1, imgRef, 0, true); 
			glsMulSpectrums(imgSrc0, imgSrc1, imgDst, true);
			break;
		case(E_TEST::MUL_SPECTRUMS_POC) :
			{
				Mat corr;
				cv::mulSpectrums(imgSrc0, imgSrc1, corr, 0, true);
				vector<Mat> tmp(2);
				cv::split(corr, tmp);
				Mat mag;
				cv::magnitude(tmp[0], tmp[1], mag);
				cv::divide(tmp[0], mag, tmp[0]);
				cv::divide(tmp[1], mag, tmp[1]);
				cv::merge(tmp, imgRef);
			}
			glsMulSpectrumsPhaseOnly(imgSrc0, imgSrc1, imgDst);
			break;
		case(E_TEST::MAG_SPECTRUMS) :
			{
				vector<Mat> tmp;
				cv::split(imgSrc1, tmp);
				cv::magnitude(tmp[0], tmp[1], imgRef);
			}
			glsMagSpectrums(imgSrc1, imgDst);
			break;
		case(E_TEST::LOG) :
			cv::log(imgSrc1, imgRef);
			glsLog(imgSrc1, imgDst);
			break;
		case(E_TEST::EXP) :
			cv::exp(imgSrc1, imgRef);
			glsExp(imgSrc1, imgDst);
			break;
		case(E_TEST::POW) :
			float power = (float)scalar[0];
			cv::pow(imgSrc1, power, imgRef);
			glsPow(imgSrc1, power, imgDst);
			break;

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
			int errNum = test_glsBasicOperationT<float>(CV_32FC4, E_TEST::ADD);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsSubtract)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC3,E_TEST::SUB);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(TestMethod_glsMultiply)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2, E_TEST::MUL);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsDivide)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::DIV, 4);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsAdd_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::ADD_S);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsSubtract_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::SUB_S);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(TestMethod_glsMultiply_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::MUL_S);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsDivide_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::DIV_S, 4);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMin)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::MIN);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMax)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::MAX);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMin_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::MIN_S);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMax_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::MAX_S);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMulSpectrums)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2, E_TEST::MUL_SPECTRUMS, 1024);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMulSpectrumsConj)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2, E_TEST::MUL_SPECTRUMS_CONJ, 128);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsMulSpectrumsPhaseOnly)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2, E_TEST::MUL_SPECTRUMS_POC, 256);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(TestMethod_glsMagSpectrums)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2, E_TEST::MAG_SPECTRUMS);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsLog)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::LOG,64);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsExp)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::EXP);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(TestMethod_glsPow)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::POW);
			Assert::AreEqual(0, errNum);
		}


	};
}