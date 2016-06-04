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
	int test_glsCartToPolar(int cvtype, bool angleInDegrees){
		Size size(32, 24);
		cout << "Size:" << size << endl;
		int ulps_mag = 4;
		int ulps_angle = 3000;

		Mat imgSrcX(size, cvtype);
		Mat imgSrcY(size, cvtype);
		FillRandU<T>(imgSrcX);
		FillRandU<T>(imgSrcY);
		imgSrcX -= 0.5;
		imgSrcY -= 0.5;

		Mat imgMag;
		Mat imgAngle;


		cv::cartToPolar(imgSrcX, imgSrcY, imgMag, imgAngle, angleInDegrees);

		GlsMat glsSrcX(imgSrcX);
		GlsMat glsSrcY(imgSrcY);
		GlsMat glsMag;
		GlsMat glsAngle;

		gls::cartToPolar(glsSrcX, glsSrcY, glsMag, glsAngle, angleInDegrees);

		Mat imgMag_ = (Mat)glsMag;
		Mat imgAngle_ = (Mat)glsAngle;


		int errNum = 0;
		if (!AreEqual<T>(imgMag, imgMag_, ulps_mag)) errNum -= 1;
		if (!AreEqual<T>(imgAngle, imgAngle_, ulps_angle)) errNum -= 1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsCartToPolar)
	{
	public:
		//glsCartToPolar
		TEST_METHOD(glsCartToPolar_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCartToPolar<float>(CV_32FC1, false);
			Assert::AreEqual(0, errNum);
		}

		//glsCartToPolar
		TEST_METHOD(glsCartToPolar_CV_32FC1_degree)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCartToPolar<float>(CV_32FC1, true);
			Assert::AreEqual(0, errNum);
		}

	};
}