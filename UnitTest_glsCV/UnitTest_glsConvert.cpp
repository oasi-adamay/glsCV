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

#include "HookCoutCerr.h"
#include "Timer.h"

#include "glsCV.h"
#include "UnitTest_Common.h"



namespace UnitTest_glsCV
{
	template <typename Ts, typename Td>
	int test_glsConvert(const int typeS, const int typeD, const float scale = 1.0f){
		const int width = 32;
		const int height = 24;
		Mat imgSrc = Mat(Size(width, height), typeS);
		Mat imgRef; 

		cout << "Size:" << imgSrc.size() << endl;
		//---------------------------------
		//init Src image
		FillRandU<Ts>(imgSrc);

		imgSrc.convertTo(imgRef, typeD, scale);
		Mat imgDst = Mat::zeros(imgRef.size(), imgRef.type());

		GlsMat glsSrc(imgSrc);		//create GlsMat and  upload
//		GlsMat glsDst(imgRef, false); //create GlsMat not upload!
		GlsMat glsDst;

		gls::convert(glsSrc, glsDst, scale);
		glsDst.CopyTo(imgDst);		// download

		if (imgDst.size() != imgRef.size()){
			cout << "imgDst.size() != imgRef.size()" << endl;
			return -1;
		}
		if (imgDst.type() != imgRef.type()){
			cout << "imgDst.type() != imgRef.type()" << endl;
			return -1;
		}


		//verify
		int errNum = 0;
		if (!AreEqual<Td>(imgRef, imgDst, 0)) errNum = -1;
		cout << "errNum:" << errNum << endl;

		return errNum;
	}

	TEST_CLASS(UnitTest_glsConvert)
	{
	public:
		TEST_METHOD(glsConvert_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<uchar, float>(CV_8UC1, CV_32FC1,1.0f/256.0f);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvert_CV_8UC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<uchar, float>(CV_8UC2, CV_32FC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvert_CV_8UC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<uchar, float>(CV_8UC3, CV_32FC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvert_CV_8UC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<uchar, float>(CV_8UC4, CV_32FC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvert_CV_16UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<ushort, float>(CV_16UC1, CV_32FC1,  1.0f / 65536.0f);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsConvert_CV_32SC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsConvert<int, float>(CV_32SC1, CV_32FC1);
			Assert::AreEqual(0, errNum);
		}


	};




	template <typename Ts, typename Td>
	int test_glsCvtColor(const int typeS, const int code, int maxUlps = 2){
		const int width = 32;
		const int height = 24;
		Mat imgSrc = Mat(Size(width, height), typeS);
		Mat imgRef;

		cout << "Size:" << imgSrc.size() << endl;

		//---------------------------------
		//init Src image
		FillRandU<Ts>(imgSrc);

		cv::cvtColor(imgSrc, imgRef, code);

		Mat imgDst = Mat::zeros(imgRef.size(), imgRef.type());

		GlsMat glsSrc(imgSrc);		//create GlsMat and  upload
//		GlsMat glsDst(imgRef, false); //create GlsMat not upload!
		GlsMat glsDst;				 

		gls::cvtColor(glsSrc, glsDst, code);
		glsDst.CopyTo(imgDst);		// download

		if (imgDst.size() != imgRef.size()){
			cout << "imgDst.size() != imgRef.size()" << endl;
			return -1;
		}
		if (imgDst.type() != imgRef.type()){
			cout << "imgDst.type() != imgRef.type()" << endl;
			return -1;
		}


		//verify
		int errNum = 0;
		if (!AreEqual<Td>(imgRef, imgDst, maxUlps)) errNum = -1;

		cout << "errNum:" << errNum << endl;

		return errNum;
	}

	TEST_CLASS(UnitTest_glsCvtColor)
	{
	public:
		TEST_METHOD(glsCvtColor_CV_32FC3_BGR2RGB)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCvtColor<float, float>(CV_32FC3, CV_BGR2RGB);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCvtColor_CV_32FC3_RGB2BGR)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCvtColor<float, float>(CV_32FC3, CV_RGB2BGR);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCvtColor_CV_32FC3_RGB2GARY)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCvtColor<float, float>(CV_32FC3, CV_RGB2GRAY);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsCvtColor_CV_32FC3_BGR2GARY)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCvtColor<float, float>(CV_32FC3, CV_BGR2GRAY);
			Assert::AreEqual(0, errNum);
		}

	};


}