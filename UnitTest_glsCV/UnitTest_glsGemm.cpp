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
	int test_glsGemm(int cvtype, int ulps = 64){
#if 0
		const int cols = 24;
		const int rows = 32;
#else
		const int cols = 1024;
		const int rows = 1024;
#endif
		const float alpha = 1.0f;
		const float beta = 1.0f;


		Mat matSrc1(Size(cols, rows), cvtype);
		Mat matSrc2(Size(rows, cols), cvtype);
		Mat matSrc3(Size(rows, rows), cvtype);


		FillRandU<T>(matSrc1);
		FillRandU<T>(matSrc2);
		FillRandU<T>(matSrc3);

		Mat matRef;
		Mat matDst;

		cout << "Size1:" << matSrc1.size() << endl;
		cout << "Size2:" << matSrc2.size() << endl;
		cout << "Size3:" << matSrc3.size() << endl;

		{
			Timer tmr("cv::gemm:   \t");
			cv::gemm(matSrc1, matSrc2, alpha, matSrc3, beta, matRef);
		}

		GlsMat glsSrc1(matSrc1);
		GlsMat glsSrc2(matSrc2);
		GlsMat glsSrc3(matSrc3);
		GlsMat glsDst;

		{
			Timer tmr("gls::gemm:   \t");
			gls::gemm(glsSrc1, glsSrc2, alpha, glsSrc3, beta, glsDst);
			glFinish();
		}

		glsDst.download(matDst);		// download

		int errNum = 0;
		if (!AreEqual<T>(matRef, matDst, ulps)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsGemm)
	{
	public:

		TEST_METHOD(glsGemm_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsGemm<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsGemm_CV_32FC1)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()



	};

}