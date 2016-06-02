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

static void
edgeTracer_recursive(int x, int y, const Mat& src, Mat& dst)
{
	CV_Assert(src.type() == CV_8UC1);
	CV_Assert(dst.type() == CV_8UC1);
	CV_Assert(dst.size() == src.size());

	static const int ofs[][2] = { { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } };
	int i;

	dst.at<uchar>(y, x) = (uchar)does_belong_to_an_edge;

	for (i = 0; i < 8; i++)
	{
		int x1 = x + ofs[i][0];
		int y1 = y + ofs[i][1];
		if ((unsigned)x1 < (unsigned)src.cols &&
			(unsigned)y1 < (unsigned)src.rows &&
			src.at<uchar>(y1, x1) == might_belong_to_an_edge &&
			dst.at<uchar>(y1, x1) == can_not_belong_to_an_edge){
			edgeTracer_recursive(x1, y1, src, dst);
		}

	}
}

static void edgeTracer(const Mat& src, Mat& dst)
{
	dst = Mat::zeros(src.size(), src.type());
	const int width = src.cols;
	const int height = src.rows;

	//edgeTracer top level
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++){
			if (src.at<uchar>(y, x) == does_belong_to_an_edge &&
				dst.at<uchar>(y, x) == can_not_belong_to_an_edge){
				edgeTracer_recursive(x, y, src, dst);
			}
		}
	}
}



	template <typename T>
	int test_glsEdgeTracer(int cvtype){

		Size size(256, 192);
		cout << "Size:" << size << endl;
		int ulps = 0;

		Point center = Point(size.width / 2, size.height / 2);
		int radius = size.height / 4;
			 
		Mat src = Mat::zeros(size, cvtype);
		cv::circle(src, center, radius, Scalar(255));
		cv::line(src, Point(0, size.height / 2), Point(size.width, size.height / 2), Scalar(255));

		const float th = 0.3f;
		for (int y = 0; y < size.height; y++){
			for (int x = 0; x < size.width; x++){
				if (cv::randu<float>() < th){
					src.at<T>(y, x) = 128;
				}
			}
		}

		Mat dst;

		{
			edgeTracer(src,dst);
		}

		GlsMat glsSrc(src);
		GlsMat glsDst;

		gls::edgeTracer(glsSrc, glsDst);

		Mat dst_ = (Mat)glsDst;

		int errNum = 0;
		if (!AreEqual<T>(dst, dst_, ulps)) errNum -= 1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsEdgeTracer)
	{
	public:
		//glsEdgeTracer
		TEST_METHOD(glsEdgeTracer_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsEdgeTracer<uchar>(CV_8UC1);
			Assert::AreEqual(0, errNum);
		}


	};
}