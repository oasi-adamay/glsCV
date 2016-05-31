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
map_follow(int x, int y, const Mat& src, Mat& dst)
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
			//			!dst.at<uchar>(y1, x1)){
			map_follow(x1, y1, src, dst);
		}

	}
}

static void map_optimize(Mat& map)
{
	Mat src = map.clone();
	Mat dst = Mat::zeros(map.size(), map.type());
	const int width = map.cols;
	const int height = map.rows;

	// hysteresis threshold
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++){
			if (src.at<uchar>(y, x) == does_belong_to_an_edge &&
				dst.at<uchar>(y, x) == can_not_belong_to_an_edge){
				//				!dst.at<uchar>(y, x)){
				map_follow(x, y, src, dst);
			}
		}
	}

	map = dst;
}



	template <typename T>
	int test_glsCanny(int cvtype){
		const float highThreshold = 1.0f;
		const float lowThreshold = 0.3f;
		int aperture_size = 3;

		Size size(32, 24);
		cout << "Size:" << size << endl;


		int ulps = 4;

		Mat src(size, cvtype);
		FillRandU<T>(src);

		Mat map;
		Mat map_before;
#if 0
		{
			int cn = 1;
			Mat dx(src.size(), CV_32FC(cn));
			Mat dy(src.size(), CV_32FC(cn));
			cv::Sobel(src, dx, CV_32F, 1, 0, aperture_size, 1, 0 /*, cv::BORDER_REPLICATE*/);
			cv::Sobel(src, dy, CV_32F, 0, 1, aperture_size, 1, 0 /*, cv::BORDER_REPLICATE*/);

			Mat mag;
			Mat angle;

			cv::cartToPolar(dx, dy, mag, angle, false);
			nonmaximaSuppression(mag, angle, map, highThreshold, lowThreshold);
			map_before = map.clone();
			map_optimize(map);
		}
#else
		{
			int cn = 1;
			GlsMat dx;
			GlsMat dy;
			GlsMat _src = (GlsMat)src;
			gls::Sobel(_src, dx, CV_32F, 1, 0, aperture_size, 1, 0 /*, cv::BORDER_REPLICATE*/);
			gls::Sobel(_src, dy, CV_32F, 0, 1, aperture_size, 1, 0 /*, cv::BORDER_REPLICATE*/);

			GlsMat mag;
			GlsMat angle;
			GlsMat _map;
			gls::cartToPolar(dx, dy, mag, angle, false);
			gls::nonmaximaSuppression(mag, angle, _map, highThreshold, lowThreshold);
			map = (Mat)_map;
			map_before = map.clone();
			map_optimize(map);
		}
#endif


		GlsMat glsSrc_(src);
		GlsMat glsMap_;

		gls::Canny(glsSrc_, glsMap_, highThreshold, lowThreshold, aperture_size , true);

		Mat map_ = (Mat)glsMap_;

		int errNum = 0;
		if (!AreEqual<T>(map, map_, ulps)) errNum -= 1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}



	TEST_CLASS(UnitTest_glsCanny)
	{
	public:
		//glsCanny
		TEST_METHOD(glsCanny_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsCanny<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}


	};
}