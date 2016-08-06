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

/*-----------------------------------------------------------------------------
include
*/
#include "glsMacro.h"
#include "GlsMat.h"
#include "glsShader.h"

#include "glsPhaseCorrelate.h"
#include "glsBasicOperation.h"
#include "glsMinMaxLoc.h"
#include "glsCopy.h"
#include "glsSplit.h"
#include "glsFft.h"

namespace gls
{


static Point2d weightedCentroid(Mat src, cv::Point peakLocation, cv::Size weightBoxSize, double* response)
{

	int type = src.type();
	CV_Assert(type == CV_32FC1 || type == CV_64FC1);

	int minr = peakLocation.y - (weightBoxSize.height >> 1);
	int maxr = peakLocation.y + (weightBoxSize.height >> 1);
	int minc = peakLocation.x - (weightBoxSize.width >> 1);
	int maxc = peakLocation.x + (weightBoxSize.width >> 1);

	Point2d centroid;
	double sumIntensity = 0.0;

	// clamp the values to min and max if needed.
	if (minr < 0)
	{
		minr = 0;
	}

	if (minc < 0)
	{
		minc = 0;
	}

	if (maxr > src.rows - 1)
	{
		maxr = src.rows - 1;
	}

	if (maxc > src.cols - 1)
	{
		maxc = src.cols - 1;
	}

	if (type == CV_32FC1)
	{
		const float* dataIn = (const float*)src.data;
		dataIn += minr*src.cols;
		for (int y = minr; y <= maxr; y++)
		{
			for (int x = minc; x <= maxc; x++)
			{
				centroid.x += (double)x*dataIn[x];
				centroid.y += (double)y*dataIn[x];
				sumIntensity += (double)dataIn[x];
			}

			dataIn += src.cols;
		}
	}
	else
	{
		const double* dataIn = (const double*)src.data;
		dataIn += minr*src.cols;
		for (int y = minr; y <= maxr; y++)
		{
			for (int x = minc; x <= maxc; x++)
			{
				centroid.x += (double)x*dataIn[x];
				centroid.y += (double)y*dataIn[x];
				sumIntensity += dataIn[x];
			}

			dataIn += src.cols;
		}
	}

	if (response)
		*response = sumIntensity;

	sumIntensity += DBL_EPSILON; // prevent div0 problems...

	centroid.x /= sumIntensity;
	centroid.y /= sumIntensity;

	return centroid;
}




Point2d phaseCorrelateRes(const GlsMat& src1, const GlsMat& src2, GlsMat& window, double* response){
	CV_Assert(src1.type() == src2.type());
	CV_Assert(src1.type() == CV_32FC1);
	CV_Assert(src1.size() == src2.size());
	if (!window.empty()){
		CV_Assert(src1.type() == window.type());
		CV_Assert(src1.size() == window.size());
	}

	int M = cv::getOptimalDFTSize(src1.rows);
	int N = cv::getOptimalDFTSize(src1.cols);

	GlsMat padded1, padded2, paddedWin;

	if (M != src1.rows || N != src1.cols)
	{
		GLS_Assert( 0 && "not supported");
		//copyMakeBorder(src1, padded1, 0, M - src1.rows, 0, N - src1.cols, BORDER_CONSTANT, Scalar::all(0));
		//copyMakeBorder(src2, padded2, 0, M - src2.rows, 0, N - src2.cols, BORDER_CONSTANT, Scalar::all(0));

		//if (!window.empty())
		//{
		//	copyMakeBorder(window, paddedWin, 0, M - window.rows, 0, N - window.cols, BORDER_CONSTANT, Scalar::all(0));
		//}
	}
	else
	{
		padded1 = src1;
		padded2 = src2;
		paddedWin = window;
	}

	GlsMat FFT1, FFT2, C;

	// perform window multiplication if available
	if (!paddedWin.empty())
	{
		// apply window to both images before proceeding...
		gls::multiply(paddedWin, padded1, padded1);
		gls::multiply(paddedWin, padded2, padded2);
	}

	// execute phase correlation equation
	// Reference: http://en.wikipedia.org/wiki/Phase_correlation

	gls::fft(padded1, FFT1);
	gls::fft(padded2, FFT2);
	gls::mulSpectrumsPhaseOnly(FFT1, FFT2, C);
	gls::fft(C, C, GLS_FFT_INVERSE | GLS_FFT_SHIFT);

//	gls::magSpectrums(C, C);
	vector<GlsMat> cpx(2);
	gls::split(C, cpx);
	C = cpx[0];

	// locate the highest peak
	Point peakLoc;

	gls::minMaxLoc(C, NULL, NULL, NULL, &peakLoc);
//	gls::minMaxLoc(C, NULL, response, NULL, &peakLoc);
	
	// get the phase shift with sub-pixel accuracy, 5x5 window seems about right here...
	Point2d t;
	t = weightedCentroid((Mat)C, peakLoc, Size(5, 5), response);
//	t = Point2d(peakLoc);
	
	// max response is M*N (not exactly, might be slightly larger due to rounding errors)
	if (response)
		*response /= M*N;

	// adjust shift relative to image center...
	Point2d center((double)padded1.cols / 2.0, (double)padded1.rows / 2.0);

	return (center - t);
}


Point2d phaseCorrelate(const GlsMat& src1, const GlsMat& src2, GlsMat& window){
	return gls::phaseCorrelateRes(src1,src2,window,0);
}



}//namespace gls

