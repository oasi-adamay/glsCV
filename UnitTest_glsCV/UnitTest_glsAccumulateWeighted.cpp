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


#include "glsCV.h"
#include "UnitTest_Common.h"


namespace UnitTest_glsCV
{

	template <typename T>
	int test_glsAccumulateWeighted(int cvtype, double alpha){
		Size size(32, 24);
		cout << "Size:" << size << endl;
		int ulps = 0;

		Mat imgSrc(size, cvtype);
		Mat imgDst(size, cvtype);
		FillRandU<T>(imgSrc);
		FillRandU<T>(imgDst);
		GlsMat glsSrc(imgSrc);
		GlsMat glsDst(imgDst);


		cv::accumulateWeighted(imgSrc, imgDst, alpha);
		gls::accumulateWeighted(glsSrc, glsDst, alpha);

		Mat imgDst_ = (Mat)glsDst;

		int errNum = 0;
		if (!AreEqual<T>(imgDst, imgDst_, ulps)) errNum -= 1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsAccumulateWeighted)
	{
	public:
		//glsAccumulateWeighted
		TEST_METHOD(glsAccumulateWeighted_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsAccumulateWeighted<float>(CV_32FC1, 0.25);
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsAccumulateWeighted_CV_32FC1)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()



	};
}