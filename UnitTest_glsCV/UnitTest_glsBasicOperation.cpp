/*
Copyright (c) 2016, oasi-adamay
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of glsCV nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "stdafx.h"
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "Timer.h"
#include "glsCV.h"
#include "math.h"
#include "UnitTest_Common.h"

namespace UnitTest_glsCV
{
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
		LOG_MAG_SPECTRUMS,
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

			FillRandU<T>(imgSrc0);
			FillRandU<T>(imgSrc1);


			//prevent div by 0
			for (int y = 0; y < imgSrc1.rows; y++){
				for (int x = 0; x < imgSrc1.cols; x++){
					T* pSrc1 = imgSrc1.ptr<T>(y, x);
					for (int ch = 0; ch < imgSrc0.channels(); ch++){
						if (fabs((float)(*pSrc1)) < FLT_MIN) *pSrc1++ = 1;
					}
				}
			}

			scalar[0] = randu<float>();
			scalar[1] = randu<float>();
			scalar[2] = randu<float>();
			scalar[3] = randu<float>();

		}

		GlsMat glsSrc0(imgSrc0);
		GlsMat glsSrc1(imgSrc1);
		GlsMat glsDst;

		//---------------------------------
		switch (flags){
		case(E_TEST::ADD) :
			cv::add(imgSrc0, imgSrc1, imgRef); 
			gls::add(glsSrc0, glsSrc1, glsSrc1);
			break;
		case(E_TEST::SUB):
			cv::subtract(imgSrc0, imgSrc1, imgRef); 
			gls::subtract(glsSrc0, glsSrc1, glsSrc1);
			break;
		case(E_TEST::MUL) :
			cv::multiply(imgSrc0, imgSrc1, imgRef); 
			gls::multiply(glsSrc0, glsSrc1, glsSrc1);
			break;
		case(E_TEST::DIV) :
			cv::divide(imgSrc0, imgSrc1, imgRef); 
			gls::divide(glsSrc0, glsSrc1, glsSrc1);
			break;
		case(E_TEST::ADD_S) :
			cv::add(scalar, imgSrc1, imgRef); 
			gls::add(scalar, glsSrc1, glsSrc1);
			break;
		case(E_TEST::SUB_S) :
			cv::subtract(scalar, imgSrc1, imgRef);
			gls::subtract(scalar, glsSrc1, glsSrc1);
			break;
		case(E_TEST::MUL_S) :
			cv::multiply(scalar, imgSrc1, imgRef); 
			gls::multiply(scalar, glsSrc1, glsSrc1);
			break;
		case(E_TEST::DIV_S) :
			cv::divide(scalar, imgSrc1, imgRef);
			gls::divide(scalar, glsSrc1, glsSrc1);
			break;
		case(E_TEST::MIN) :
			cv::min(imgSrc0, imgSrc1, imgRef); 
			gls::min(glsSrc0, glsSrc1, glsSrc1);
			break;
		case(E_TEST::MAX) :
			cv::max(imgSrc0, imgSrc1, imgRef);
			gls::max(glsSrc0, glsSrc1, glsSrc1);
			break;
		case(E_TEST::MIN_S) :
			cv::min(scalar, imgSrc1, imgRef); 
			gls::min(scalar, glsSrc1, glsSrc1);
			break;
		case(E_TEST::MAX_S) :
			cv::max(scalar, imgSrc1, imgRef); 
			gls::max(scalar, glsSrc1, glsSrc1);
			break;
		case(E_TEST::MUL_SPECTRUMS) :
			cv::mulSpectrums(imgSrc0, imgSrc1, imgRef, 0, false); 
			gls::mulSpectrums(glsSrc0, glsSrc1, glsSrc1, false);
			break;
		case(E_TEST::MUL_SPECTRUMS_CONJ) :
			cv::mulSpectrums(imgSrc0, imgSrc1, imgRef, 0, true); 
			gls::mulSpectrums(glsSrc0, glsSrc1, glsSrc1, true);
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
			gls::mulSpectrumsPhaseOnly(glsSrc0, glsSrc1, glsSrc1);
			break;
		case(E_TEST::MAG_SPECTRUMS) :
			{
				vector<Mat> tmp;
				cv::split(imgSrc1, tmp);
				cv::magnitude(tmp[0], tmp[1], imgRef);
			}
			gls::magSpectrums(glsSrc1, glsSrc1);
			break;
		case(E_TEST::LOG) :
			cv::log(imgSrc1, imgRef);
			gls::log(glsSrc1, glsSrc1);
			break;
		case(E_TEST::EXP) :
			cv::exp(imgSrc1, imgRef);
			gls::exp(glsSrc1, glsSrc1);
			break;
		case(E_TEST::POW) : 
			{
				float power = (float)scalar[0];
				cv::pow(imgSrc1, power, imgRef);
				gls::pow(glsSrc1, power, glsSrc1);
			}
			break;
		case(E_TEST::LOG_MAG_SPECTRUMS) :
			{
				vector<Mat> tmp;
				cv::split(imgSrc1, tmp);
				cv::magnitude(tmp[0], tmp[1], imgRef);
				cv::log(imgRef+1.0, imgRef);
			}
			gls::logMagSpectrums(glsSrc1, glsSrc1,1.0);
			break;
		};

//		glsSrc1.download(imgDst);
		imgDst = (Mat)glsSrc1;

		if (!AreEqual<T>(imgRef, imgDst, maxUlps)) errNum = -1;

		cout << "errNum:" << errNum << endl;

		return errNum;
	}



	TEST_CLASS(UnitTest_glsBasicOperation)
	{
	public:

		TEST_METHOD(glsAdd)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC4, E_TEST::ADD);
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsAdd)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()


		TEST_METHOD(glsSubtract)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC3,E_TEST::SUB);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(glsMultiply)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2, E_TEST::MUL);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsDivide)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::DIV, 4);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsAdd_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::ADD_S);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsSubtract_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::SUB_S);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(glsMultiply_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::MUL_S);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsDivide_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::DIV_S, 4);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsMin)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::MIN);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsMax)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::MAX);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsMin_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::MIN_S);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsMax_S)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::MAX_S);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsMulSpectrums)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2, E_TEST::MUL_SPECTRUMS, 1024);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsMulSpectrumsConj)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2, E_TEST::MUL_SPECTRUMS_CONJ, 128);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsMulSpectrumsPhaseOnly)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2, E_TEST::MUL_SPECTRUMS_POC, 512);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMagSpectrums)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2, E_TEST::MAG_SPECTRUMS);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsLog)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::LOG,64);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsExp)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::EXP);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsPow)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC1, E_TEST::POW,16);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsLogMagSpectrums)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsBasicOperationT<float>(CV_32FC2, E_TEST::LOG_MAG_SPECTRUMS,16);
			Assert::AreEqual(0, errNum);
		}

		

	};
}