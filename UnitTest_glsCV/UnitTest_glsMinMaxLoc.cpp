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
	int test_glsMinMaxLoc(int cvtype, bool _min = true, bool _max = true, bool _minLoc = true, bool _maxLoc = true ,Size size = Size(32, 24)){
		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);

		cout << "Size:" << imgSrc.size() << endl;

		double minValRef = 0;
		double maxValRef = 0;
		double minVal = 0;
		double maxVal = 0;
		Point minLocRef(0, 0);
		Point maxLocRef(0, 0);
		Point minLoc(0, 0);
		Point maxLoc(0, 0);

		double* pminValRef = _min ? &minValRef : 0;
		double* pmaxValRef = _max ? &maxValRef : 0;
		double* pminVal = _min ? &minVal : 0;
		double* pmaxVal = _max ? &maxVal : 0;
		Point* pminLocRef= _minLoc? &minLocRef : 0 ;
		Point* pmaxLocRef= _maxLoc? &maxLocRef : 0 ;
		Point* pminLoc = _minLoc? &minLoc : 0 ;
		Point* pmaxLoc = _maxLoc ? &maxLoc : 0 ;

		GlsMat glsSrc(imgSrc);

		int loop = 1;
		if (size.width >= 256) loop = 10;
		for (int i = 0; i < loop; i++){
			_TMR_("cv::minMaxLoc:");
			cv::minMaxLoc(imgSrc, pminValRef, pmaxValRef, pminLocRef, pmaxLocRef);
		}
		for (int i = 0; i < loop; i++){
			_TMR_("gls::minMaxLoc:");
			gls::minMaxLoc(glsSrc, pminVal, pmaxVal, pminLoc, pmaxLoc);
		}

		if (_min)cout << "minVal:" << minValRef << ",\t" << minVal << endl;
		if (_max)cout << "maxVal:" << maxValRef << ",\t" << maxVal << endl;
		if (_minLoc)cout << "minLoc:" << minLocRef << ",\t" << minLoc << endl;
		if (_maxLoc)cout << "maxLoc:" << maxLocRef << ",\t" << maxLoc << endl;

		int errNum = 0;
		if (_min) { if (!AreEqual<float>((float)minValRef, (float)minVal, 0))errNum++; }
		if (_max) { if (!AreEqual<float>((float)maxValRef, (float)maxVal, 0))errNum++; }
		if (_minLoc){ if (!AreEqual<int>(minLocRef.x, minLoc.x, 0))errNum++; }
		if (_minLoc){ if (!AreEqual<int>(minLocRef.y, minLoc.y, 0))errNum++; }
		if (_maxLoc){ if (!AreEqual<int>(maxLocRef.x, maxLoc.x, 0))errNum++; }
		if (_maxLoc){ if (!AreEqual<int>(maxLocRef.y, maxLoc.y, 0))errNum++; }
	
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


		TEST_METHOD(glsMinMaxLoc_CV_32FC1_min)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMinMaxLoc<float>(CV_32FC1,true,false,false,false);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsMinMaxLoc_CV_32FC1_max)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMinMaxLoc<float>(CV_32FC1, false, true, false, false);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsMinMaxLoc_CV_32FC1_minLoc)
		{			cout << __FUNCTION__ << endl;
			int errNum = test_glsMinMaxLoc<float>(CV_32FC1, false, false, true, false);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsMinMaxLoc_CV_32FC1_maxLoc)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMinMaxLoc<float>(CV_32FC1, false, false, false, true);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsMinMaxLoc_CV_32FC1_1024x1024)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMinMaxLoc<float>(CV_32FC1, true,true,true,true, Size(1024, 1024));
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsMinMaxLoc_CV_32FC1_1024x1024)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()

	};

}