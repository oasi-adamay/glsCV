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
	int test_glsWarpAffine(int cvtype, int interpolation){
		int ulps = 4;
		float eps = 0.01f;
		Size size(32, 24);
		//Size dsize(32, 24);
		Size dsize(64, 48);
		Point2f center((float)size.width / 2, (float)size.height / 2);
		double angle = 13.0;
		double scale = 1.2;
		Mat M = cv::getRotationMatrix2D(center, angle, scale);
//		M.convertTo(M, CV_32F);

		cout << "Size:" << size << endl;

#if 0
		Mat imgSrc(size, cvtype);
		FillRandU<T>(imgSrc);
#else
		Mat imgSrc = Mat::zeros(size, cvtype);
		Mat roi = Mat(imgSrc, Rect(size.width / 4, size.height / 4, size.width / 2, size.height / 2));
		FillRandU<T>(roi);
#endif

		Mat imgRef;
		Mat imgDst;
		GlsMat glsSrc(imgSrc);
		GlsMat glsDst;

#if 1
		cv::warpAffine(imgSrc, imgRef, M, dsize, interpolation);
#else
		Mat _M;
		cv::invertAffineTransform(M, _M);
		_M.convertTo(_M, CV_32F);

		Mat mapx(dsize, CV_32F);
		Mat mapy(dsize, CV_32F);
		float *m = (float *)_M.data;

		for (int y = 0; y < dsize.height; y++){
			for (int x = 0; x < dsize.width; x++){
				mapx.at<float>(y, x) = (float)(x*m[0] + y*m[1] + m[2]);
				mapy.at<float>(y, x) = (float)(x*m[3] + y*m[4] + m[5]);
			}
		}
		cv::remap(imgSrc, imgRef, mapx, mapy, interpolation);


#endif
		gls::warpAffine(glsSrc, glsDst, M, dsize, interpolation);

		imgDst = (Mat)glsDst;

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst, ulps, eps)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsWarpAffine)
	{
	public:
		TEST_METHOD(glsWarpAffine_CV_32FC1_INTER_NEAREST)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsWarpAffine<float>(CV_32FC1, INTER_NEAREST);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(glsWarpAffine_CV_32FC1_INTER_LINEAR)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsWarpAffine<float>(CV_32FC1, INTER_LINEAR);
			Assert::AreEqual(0, errNum);
		}

		//TEST_METHOD(glsWarpAffine_CV_32FC1_INTER_CUBIC)
		//{
		//	cout << __FUNCTION__ << endl;
		//	int errNum = test_glsWarpAffine<float>(CV_32FC1, INTER_CUBIC);
		//	Assert::AreEqual(0, errNum);
		//}


	};

}