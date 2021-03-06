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
	int test_glsSplit(int cvtype){
		Size size(8, 6);
		cout << "Size:" << size << endl;

		Mat imgSrc = Mat(size, cvtype);
		FillRandU<T>(imgSrc);

		vector<Mat> plnRef;
		cv::split(imgSrc, plnRef);

		//----------------------
		GlsMat glsSrc = (GlsMat)imgSrc;
		vector<GlsMat> plnDst;
		gls::split(glsSrc, plnDst);

		int errNum = 0;
		for (int i = 0; i < (int)plnRef.size();i++){
			Mat imgRef = plnRef[i];
			Mat imgDst = (Mat)plnDst[i];

			if (!AreEqual<T>(imgRef, imgDst)) errNum++;
		}

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}




	TEST_CLASS(UnitTest_glsSplit)
	{
	public:
		//glsSplit
		TEST_METHOD(glsSplit_CV_8UC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsSplit<uchar>(CV_8UC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsSplit_CV_8UC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsSplit<uchar>(CV_8UC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsSplit_CV_8UC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsSplit<uchar>(CV_8UC4);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(glsSplit_CV_32FC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsSplit<float>(CV_32FC4);
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsSplit_CV_32FC4)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()


	};


}