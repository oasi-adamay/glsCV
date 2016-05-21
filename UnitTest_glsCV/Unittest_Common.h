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

#pragma once

#include "Timer.h"

//#ifdef _DEBUG
#if 1
#define _TMR_(...)  Timer tmr(__VA_ARGS__)
#else
#define _TMR_(...)
#endif


#define TEST_MY_TRAIT(traitValue) TEST_METHOD_ATTRIBUTE(L"GlsTrait", traitValue)

//BEGIN_TEST_METHOD_ATTRIBUTE(Method1)
//	TEST_OWNER(L"OwnerName")
//	TEST_PRIORITY(1)
//	TEST_MY_TRAIT(L"thisTraitValue")
//END_TEST_METHOD_ATTRIBUTE()



namespace UnitTest_glsCV
{

	// calc ULPS (Units in the Last Place)
	template <typename T> static int ULPs(T a, T b){
		return abs((int)a - (int)b);
	}

	template <> static int ULPs(float a, float b){
		int aInt = *(int*)&a;
		// Make aInt lexicographically ordered as a twos-complement int
		if (aInt < 0)
			aInt = 0x80000000 - aInt;
		// Make bInt lexicographically ordered as a twos-complement int
		int bInt = *(int*)&b;
		if (bInt < 0)
			bInt = 0x80000000 - bInt;
		return abs(aInt - bInt);
	}



	bool AlmostEqualUlpsAbsEps(float A, float B, int maxUlps, float maxDiff = 1e-3);


	template <typename T> static
	bool AreEqual(T val0, T val1, int maxUlps){
		return val0 == val1;
	}

	template<> static 
	bool AreEqual<float>(float val0, float val1, int maxUlps){
		return AlmostEqualUlpsAbsEps(val0, val1, maxUlps, FLT_MIN);
	}

	template<> static
		bool AreEqual<Scalar>(Scalar val0, Scalar val1, int maxUlps){
		return AlmostEqualUlpsAbsEps((float)val0[0], (float)val1[0], maxUlps, FLT_MIN)
			&& AlmostEqualUlpsAbsEps((float)val0[1], (float)val1[1], maxUlps, FLT_MIN)
			&& AlmostEqualUlpsAbsEps((float)val0[2], (float)val1[2], maxUlps, FLT_MIN)
			&& AlmostEqualUlpsAbsEps((float)val0[3], (float)val1[3], maxUlps, FLT_MIN);
	}

	template <typename T> static
	bool AreEqual(Mat& mat0, Mat& mat1, int maxUlpsSpec = 0, float maxDiffSpec = 1e-6){
		if (mat0.size() != mat1.size()){
			cerr << "mat0.size() != mat1.size()" << endl;
			return false;
		}

		if (mat0.type() != mat1.type()){
			cerr << "mat0.type() != mat1.type()" << endl;
			return false;
		}

		int errNum = 0;
		int ulpsMax = 0;
		int ulpsMaxPos[3] = { -1, -1, -1 };
		T   ulpsMaxRef;
		T   ulpsMaxDst;
		for (int y = 0; y < mat0.rows; y++){
			for (int x = 0; x < mat0.cols; x++){
				T* pRef = mat0.ptr<T>(y, x);
				T* pDst = mat1.ptr<T>(y, x);
				for (int ch = 0; ch < mat0.channels(); ch++){
					T ref = *pRef++;
					T dst = *pDst++;
					int ulps = ULPs<T>(ref,dst);
					if (ulps > ulpsMax){
						ulpsMax = ulps;
						ulpsMaxPos[0] = y;
						ulpsMaxPos[1] = x;
						ulpsMaxPos[2] = ch;
						ulpsMaxRef = ref;
						ulpsMaxDst = dst;
					}

					if (ulps > maxUlpsSpec && fabs((float)ref - (float)dst)>maxDiffSpec){
						errNum++;
						if (errNum<100){
							cout << cv::format("(%4d,%4d,%4d)\t", y, x, ch);
							cout << ref << "\t";
							cout << dst << "\t";
							cout << ulps << "\t";
							cout << endl;
						}
					}
				}
			}
		}

		cout << "ULPS spec:" << maxUlpsSpec << endl;
		if (ulpsMax > 0){
			cout << cv::format("(%4d,%4d,%4d)\t", ulpsMaxPos[0], ulpsMaxPos[1], ulpsMaxPos[2]);
			cout << "ULPS:" << ulpsMax << "\t";
			cout << "Ref:" << ulpsMaxRef << "\t";
			cout << "Dst:" << ulpsMaxDst << "\t";
			cout << endl;
		}

		cout << "ErrNum:" << errNum << endl;

		if (errNum == 0) return true;
		return false;
	}


	template<typename T> static
	void FillRandU(Mat &mat){
		for (int y = 0; y < mat.rows; y++){
			for (int x = 0; x < mat.cols; x++){
				T* pSrc = mat.ptr<T>(y, x);
				for (int ch = 0; ch < mat.channels(); ch++){
					*pSrc++ = randu<T>();
				}
			}
		}
	}


}