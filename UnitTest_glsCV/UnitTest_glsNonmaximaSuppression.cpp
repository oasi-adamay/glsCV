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

//map encording
enum {
	can_not_belong_to_an_edge = 0,
	might_belong_to_an_edge = 128,
	does_belong_to_an_edge = 255,
};

//do nonmaxima suppression
//angle : radian [0-2PI]
static
void nonmaximaSuppression(const Mat&mag, const Mat&angle, Mat& map, const float highThreshold, const float lowThreshold){
	CV_Assert(mag.type() == CV_32FC1);
	CV_Assert(angle.type() == CV_32FC1);
	CV_Assert(highThreshold >= lowThreshold);

	const double rad_pi_8 = (CV_PI / 8.);
	const double rad_1pi_4 = (CV_PI * 1 / 4);
	const double rad_2pi_4 = (CV_PI * 2 / 4);
	const double rad_3pi_4 = (CV_PI * 3 / 4);

	map = Mat(mag.size(), CV_8UC1);

	const int width = mag.cols;
	const int height = mag.rows;



	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			uchar val;

			float a = mag.at<float>(y, x), b = 0, c = 0;
			int y1 = 0, y2 = 0, x1 = 0, x2 = 0;

			if (a < lowThreshold) {
			}
			else{
				double rad = angle.at<float>(y, x);
				rad += rad_pi_8;

				if (rad > CV_PI) rad -= CV_PI;
				if (rad > CV_PI) rad -= CV_PI;

				if (0 < rad && rad < rad_1pi_4){		// E-W
					y1 = y2 = y; x1 = x + 1; x2 = x - 1;
				}
				else if (rad_1pi_4 <= rad && rad <= rad_2pi_4){	// NE-SW
					y1 = y + 1; y2 = y - 1; x1 = x + 1; x2 = x - 1;
				}
				else if (rad_3pi_4 <= rad || rad <= 0){	// NW-SE
					y1 = y - 1; y2 = y + 1; x1 = x + 1; x2 = x - 1;
				}
				else{
					CV_Assert(rad_2pi_4 < rad && rad < rad_3pi_4);
					x1 = x2 = x; y1 = y + 1; y2 = y - 1;
				}

				if ((unsigned)y1 < (unsigned)height && (unsigned)x1 < (unsigned)width){
					b = (float)fabs(mag.at<float>(y1, x1));
				}

				if ((unsigned)y2 < (unsigned)height && (unsigned)x2 < (unsigned)width){
					c = (float)fabs(mag.at<float>(y2, x2));
				}

				if ((a > b || (a == b && ((x1 == x + 1 && y1 == y) || (x1 == x && y1 == y + 1)))) && a > c){
				}
				else{
					a = 0;
				}
			}

			if (a > highThreshold){
				val = does_belong_to_an_edge;
			}
			else if (a < lowThreshold){
				val = can_not_belong_to_an_edge;
			}
			else{
				val = might_belong_to_an_edge;
			}

			map.at<uchar>(y, x) = val;
		}
	}
}




	template <typename T>
	int test_glsNonmaximaSuppression(int cvtype){
		const float highThreshold = 0.5f;
		const float lowThreshold = 0.3f;

		Size size(32, 24);
		cout << "Size:" << size << endl;


		int ulps = 4;

		Mat imgSrcX(size, cvtype);
		Mat imgSrcY(size, cvtype);
		FillRandU<T>(imgSrcX);
		FillRandU<T>(imgSrcY);
		imgSrcX -= 0.5;
		imgSrcY -= 0.5;

		Mat imgMag;
		Mat imgAngle;
		cv::cartToPolar(imgSrcX, imgSrcY, imgMag, imgAngle, false);

		Mat imgDst;
		nonmaximaSuppression(imgMag, imgAngle, imgDst, highThreshold, lowThreshold);

		GlsMat glsMag(imgMag);
		GlsMat glsAngle(imgAngle);
		GlsMat glsDst;

		gls::nonmaximaSuppression(glsMag, glsAngle, glsDst, highThreshold, lowThreshold);

		Mat imgDst_ = (Mat)glsDst;

		int errNum = 0;
		if (!AreEqual<uchar>(imgDst, imgDst_, ulps)) errNum -= 1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsNonmaximaSuppression)
	{
	public:
		//glsNonmaximaSuppression

		TEST_METHOD(glsNonmaximaSuppression_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsNonmaximaSuppression<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(glsNonmaximaSuppression_CV_32FC1)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()



	};
}