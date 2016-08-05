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
	int test_glsMinMaxLoc(int cvtype, bool minmaxonly = false,Size size = Size(32, 24)){
		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);

		cout << "Size:" << imgSrc.size() << endl;

		double maxValRef;
		double minValRef;
		double maxVal;
		double minVal;
		Point minLocRef;
		Point maxLocRef;
		Point minLoc;
		Point maxLoc;

		GlsMat glsSrc(imgSrc);

		int loop = 1;
		if (size.width >= 256) loop = 10;
		if (minmaxonly){
			for (int i = 0; i < loop; i++){
				_TMR_("cv::minMaxLoc:");
				cv::minMaxLoc(imgSrc, &minValRef, &maxValRef);
			}
			for (int i = 0; i < loop; i++){
				_TMR_("gls::minMaxLoc:");
				gls::minMaxLoc(glsSrc, &minVal, &maxVal);
			}
		}
		else{
			for (int i = 0; i < loop; i++){
				_TMR_("cv::minMaxLoc:");
				cv::minMaxLoc(imgSrc, &minValRef, &maxValRef, &minLocRef, &maxLocRef);
			}
			for (int i = 0; i < loop; i++){
				_TMR_("gls::minMaxLoc:");
				gls::minMaxLoc(glsSrc, &minVal, &maxVal, &minLoc, &maxLoc);
			}
		}



		cout << maxValRef << "," << maxVal << endl;
		cout << minValRef << "," << minVal << endl;

		if (!minmaxonly){
			cout << minLocRef << "," << minLoc << endl;
			cout << maxLocRef << "," << maxLoc << endl;
		}

		int errNum = 0;
		if (!AreEqual<float>((float)maxValRef, (float)maxVal, 0))errNum++;
		if (!AreEqual<float>((float)minValRef, (float)minVal, 0))errNum++;
		if (!minmaxonly){
			if (!AreEqual<int>(minLocRef.x, minLoc.x, 0))errNum++;
			if (!AreEqual<int>(minLocRef.y, minLoc.y, 0))errNum++;
			if (!AreEqual<int>(maxLocRef.x, maxLoc.x, 0))errNum++;
			if (!AreEqual<int>(maxLocRef.y, maxLoc.y, 0))errNum++;
		}

		cout << "errNum:" << errNum << endl;

		return errNum;
	}

	TEST_CLASS(UnitTest_glsMinMaxLoc)
	{
	public:

		TEST_METHOD(glsMinMaxLoc_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMinMaxLoc<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsMinMaxLoc_CV_32FC1)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()


		TEST_METHOD(glsMinMax_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMinMaxLoc<float>(CV_32FC1,true);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(glsMinMaxLoc_CV_32FC1_1024x1024)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMinMaxLoc<float>(CV_32FC1 , false , Size(1024,1024));
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsMinMaxLoc_CV_32FC1_1024x1024)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(glsMinMax_CV_32FC1_1024x1024)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMinMaxLoc<float>(CV_32FC1, true, Size(1024, 1024));
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsMinMax_CV_32FC1_1024x1024)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()

	};

}