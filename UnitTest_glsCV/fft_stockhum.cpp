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


#include <complex>
#include <cmath>

#define _USE_MATH_DEFINES
#include <math.h>


typedef std::complex<double> complex_t;

void fft0_type0_dif(int n, int s, bool eo, complex_t* x, complex_t* y)
{
	const int m = n / 2;
	const double theta0 = 2 * M_PI / n;

	if (n == 1) { if (eo) for (int q = 0; q < s; q++) y[q] = x[q]; }
	else {
		cout << "---------------" << endl;


		for (int p = 0; p < m; p++) {
			const complex_t wp = complex_t(cos(p*theta0), -sin(p*theta0));
			for (int q = 0; q < s; q++) {
				int ix0 = q + s*(p + 0);
				int ix1 = q + s*(p + m);
				int iy0 = q + s*(2 * p + 0);
				int iy1 = q + s*(2 * p + 1);
				cout << "src(" << ix0 << "," << ix1 << ")\t";
				cout << "dst(" << iy0 << "," << iy1 << ")\t";
				cout << endl;



				const complex_t a = x[q + s*(p + 0)];
				const complex_t b = x[q + s*(p + m)];
				y[q + s*(2 * p + 0)] = a + b;
				y[q + s*(2 * p + 1)] = (a - b) * wp;
			}
		}
		fft0_type0_dif(n / 2, 2 * s, !eo, y, x);
	}
}

void fft_type0_dif(int n, complex_t* x)
{
	complex_t* y = new complex_t[n];
	fft0_type0_dif(n, 1, 0, x, y);
	delete[] y;
	for (int k = 0; k < n; k++) x[k] /= n;
}

void ifft_type0_dif(int n, complex_t* x)
{
	for (int p = 0; p < n; p++) x[p] = conj(x[p]);
	complex_t* y = new complex_t[n];
	fft0_type0_dif(n, 1, 0, x, y);
	delete[] y;
	for (int k = 0; k < n; k++) x[k] = conj(x[k]);
}

//-------------------------------------------------------------------
void fft0_type0_dit(int n, int s, bool eo, complex_t* x, complex_t* y)
{
	const int m = n / 2;
	const double theta0 = 2 * M_PI / n;

	if (n == 1) { if (eo) for (int q = 0; q < s; q++) x[q] = y[q]; }
	else {

		fft0_type0_dit(n / 2, 2 * s, !eo, y, x);

		cout << "---------------" << endl;
		for (int p = 0; p < m; p++) {
			const complex_t wp = complex_t(cos(p*theta0), -sin(p*theta0));
			cout << "n:" << n <<"\t";
			cout << "p:" << p << "\t";
			cout << endl;

			for (int q = 0; q < s; q++) {
				int ix0 = q + s*(2 * p + 0);
				int ix1 = q + s*(2 * p + 1);
				int iy0 = q + s*(p + 0);
				int iy1 = q + s*(p + m);
				cout << "src(" << ix0 << "," << ix1 << ")\t";
				cout << "dst(" << iy0 << "," << iy1 << ")\t";
				cout << endl;


				const complex_t a = y[q + s*(2 * p + 0)];
				const complex_t b = y[q + s*(2 * p + 1)] * wp;
				x[q + s*(p + 0)] = a + b;
				x[q + s*(p + m)] = a - b;
			}
		}
	}
}

void fft_type0_dit(int n, complex_t* x)
{
	complex_t* y = new complex_t[n];
	fft0_type0_dit(n, 1, 0, x, y);
	delete[] y;
	for (int k = 0; k < n; k++) x[k] /= n;
}

void ifft_type0_dit(int n, complex_t* x)
{
	for (int p = 0; p < n; p++) x[p] = conj(x[p]);
	complex_t* y = new complex_t[n];
	fft0_type0_dit(n, 1, 0, x, y);
	delete[] y;
	for (int k = 0; k < n; k++) x[k] = conj(x[k]);
}

//-------------------------------------------------------
void fft0_type1_dif(int n, int s, bool eo, complex_t* x, complex_t* y)
{
	const int m = n / 2;
	const double theta0 = M_PI / s;

	if (n == 1) { if (eo) for (int q = 0; q < s; q++) y[q] = x[q]; }
	else {
		cout << "---------------" << endl;
		for (int p = 0; p < m; p++) {
			for (int q = 0; q < s; q++) {
				int ix0 = q + s*(p + 0);
				int ix1 = q + s*(p + m);
				int iy0 = q + s*(2 * p + 0);
				int iy1 = q + s*(2 * p + 1);
				cout << "src(" << ix0 << "," << ix1 << ")\t";
				cout << "dst(" << iy0 << "," << iy1 << ")\t";
				cout << endl;


				const complex_t wq = complex_t(cos(q*theta0), -sin(q*theta0));
				const complex_t a = x[q + s*(p + 0)];
				const complex_t b = x[q + s*(p + m)] * wq;
				y[q + s*(2 * p + 0)] = a + b;
				y[q + s*(2 * p + 1)] = a - b;
			}
		}
		fft0_type1_dif(n / 2, 2 * s, !eo, y, x);
	}
}

void fft_type1_dif(int n, complex_t* x)
{
	complex_t* y = new complex_t[n];
	fft0_type1_dif(n, 1, 0, x, y);
	delete[] y;
	for (int k = 0; k < n; k++) x[k] /= n;
}

void ifft_type1_dif(int n, complex_t* x)
{
	for (int p = 0; p < n; p++) x[p] = conj(x[p]);
	complex_t* y = new complex_t[n];
	fft0_type1_dif(n, 1, 0, x, y);
	delete[] y;
	for (int k = 0; k < n; k++) x[k] = conj(x[k]);
}


//------------------------------------------------------------------------
void fft0_type1_dit(int n, int s, bool eo, complex_t* x, complex_t* y)
{
	const int m = n / 2;
	const double theta0 = M_PI / s;

	if (n == 1) { if (eo) for (int q = 0; q < s; q++) x[q] = y[q]; }
	else {
		fft0_type1_dit(n / 2, 2 * s, !eo, y, x);
		cout << "---------------" << endl;
		for (int p = 0; p < m; p++) {
			for (int q = 0; q < s; q++) {
				int ix0 = q + s*(2 * p + 0);
				int ix1 = q + s*(2 * p + 1);
				int iy0 = q + s*(p + 0);
				int iy1 = q + s*(p + m);
				cout << "src(" << ix0 << "," << ix1 << ")\t";
				cout << "dst(" << iy0 << "," << iy1 << ")\t";
				cout << endl;

				const complex_t wq = complex_t(cos(q*theta0), -sin(q*theta0));
				const complex_t a = y[q + s*(2 * p + 0)];
				const complex_t b = y[q + s*(2 * p + 1)];
				x[q + s*(p + 0)] = a + b;
				x[q + s*(p + m)] = (a - b)*wq;
			}
		}
	}
}

void fft_type1_dit(int n, complex_t* x)
{
	complex_t* y = new complex_t[n];
	fft0_type1_dit(n, 1, 0, x, y);
	delete[] y;
	for (int k = 0; k < n; k++) x[k] /= n;
}

void ifft_type1_dit(int n, complex_t* x)
{
	for (int p = 0; p < n; p++) x[p] = conj(x[p]);
	complex_t* y = new complex_t[n];
	fft0_type1_dit(n, 1, 0, x, y);
	delete[] y;
	for (int k = 0; k < n; k++) x[k] = conj(x[k]);
}


//-----------------------------------------------------------------------


//---------------------------------------------------------------------------
//value is power of 2
static bool IsPow2(unsigned int x){
	return (((x)&(x - 1)) == 0);
}



static unsigned int insertZeroBits(
	const unsigned int src,		//src
	const int idx,				//‘}“ü‚·‚éˆÊ’u
	const int num				//‘}“ü‚·‚é”
	)
{
	unsigned int ret = src << num;
	ret &= ~((1 << (idx + num)) - 1);
	ret |= src & ((1 << (idx)) - 1);
	return ret;
}

//http://graphics.stanford.edu/~seander/bithacks.html#SwappingBitsXOR
 
static unsigned int bitSwap(
	const unsigned int b,		 // bits to swap reside in b
	const int i,	// positions of bit sequences to swap
	const int j,	//
	const int n		// number of consecutive bits in each sequence
	)
{
	unsigned int x = ((b >> i) ^ (b >> j)) & ((1U << n) - 1); // XOR temporary
	return  b ^ ((x << i) | (x << j));
}

static unsigned int modq(unsigned int x, unsigned int q) { return  x& ((1 << q) - 1); }
static unsigned int divq(unsigned int x, unsigned int q) { return  x >> q; }



// FFT Stockham,DIT
void fft_dit_Stockham_radix2_type0(const Mat& src, Mat &dst){
	CV_Assert(src.type() == CV_32FC2);
//	CV_Assert(src.cols == src.rows);
	int N;
	if (src.cols == src.rows){
		N = src.cols;
	}
	else if (src.cols == 1){
		N = src.rows;
	}
	else if (src.rows == 1){
		N = src.cols;
	}
	else{
		CV_Assert(0);
	}

	CV_Assert(IsPow2(N));

	enum { _re_ = 0, _im_ = 1 };


	Mat buf[2];
	buf[0] = src.clone();
	buf[1] = Mat(src.size(), src.type());


	vector<Vec2f> w(N / 2);


	// --- twidle ----
	for (int n = 0; n < N / 2; n++){
		float jw = (float)(-2 * M_PI * n / N);
		w[n][_re_] = cos(jw);
		w[n][_im_] = sin(jw);
	}


	int Q = 0;
	while ((1 << Q) < N){ Q++; }

	/* FFT‚ÌŒvŽZ */

	int bank = 0;
	// --- FFT rows ----
	if (src.cols != 1){
#ifdef _OPENMP
//#pragma omp parallel for
#endif
		for (int row = 0; row < src.rows; row++){
			//int bank = 0;

			for (int p = 0, q = Q - 1; q >= 0; p++, q--, bank = bank ^ 1) {
				Vec2f* x = (Vec2f*)buf[bank].ptr<Vec2f>(row);
				Vec2f* y = (Vec2f*)buf[bank ^ 1].ptr<Vec2f>(row);

				int a = 1 << p;
				int b = 1 << q;

				//cout << "------------------------" << endl;
				//cout << "p:" << p << "\t";
				//cout << "q:" << q << "\t";
				//cout << "a:" << a << "\t";
				//cout << "b:" << b << "\t";
				//cout << endl;

				for (int n = 0; n < N / 2; n = n++) {
					int iw = modq(n, p)*b;
					int ix0 = n;
					int ix1 = ix0 + N / 2;
					int iy0 = insertZeroBits(n, p, 1);
					int iy1 = iy0 + (1 << p);

					//cout << "w(" << iw << ")\t";
					//cout << "src(" << ix0 << "," << ix1 << ")\t";
					//cout << "dst(" << iy0 << "," << iy1 << ")\t";
					//cout << endl;

					Vec2f tmp;
					tmp[_re_] = x[ix1][_re_] * w[iw][_re_] - x[ix1][_im_] * w[iw][_im_];
					tmp[_im_] = x[ix1][_re_] * w[iw][_im_] + x[ix1][_im_] * w[iw][_re_];


					y[iy0] = x[ix0] + tmp;
					y[iy1] = x[ix0] - tmp;
				}
			}
		}
	}
	// --- FFT cols ----
	if (src.rows != 1){

#ifdef _OPENMP
//#pragma omp parallel for
#endif
		for (int col = 0; col < src.cols; col++){
			//int bank = (Q & 1);	//FFT rows ‚Ì‰ñ”‚ªŠï”‚È‚ç‚ÎŠï”bank‚©‚ç

			for (int p = 0, q = Q - 1; q >= 0; p++, q--, bank = bank ^ 1) {
				Vec2f* x = (Vec2f*)buf[bank].ptr<Vec2f>(0, col);
				Vec2f* y = (Vec2f*)buf[bank ^ 1].ptr<Vec2f>(0, col);

				int a = 1 << p;
				int b = 1 << q;

				//cout << "------------------------" << endl;
				//cout << "p:" << p << "\t";
				//cout << "q:" << q << "\t";
				//cout << "a:" << a << "\t";
				//cout << "b:" << b << "\t";
				//cout << endl;

				for (int n = 0; n < N / 2; n = n++) {
					int iw = modq(n, p)*b;
					int ix0 = n;
					int ix1 = ix0 + N / 2;
					int iy0 = insertZeroBits(n, p, 1);
					int iy1 = iy0 + (1 << p);
					ix0 *= src.cols;
					ix1 *= src.cols;
					iy0 *= src.cols;
					iy1 *= src.cols;

					//cout << "w(" << iw << ")\t";
					//cout << "src(" << ix0 << "," << ix1 << ")\t";
					//cout << "dst(" << iy0 << "," << iy1 << ")\t";
					//cout << endl;

					Vec2f tmp;
					tmp[_re_] = x[ix1][_re_] * w[iw][_re_] - x[ix1][_im_] * w[iw][_im_];
					tmp[_im_] = x[ix1][_re_] * w[iw][_im_] + x[ix1][_im_] * w[iw][_re_];


					y[iy0] = x[ix0] + tmp;
					y[iy1] = x[ix0] - tmp;
				}
			}

		}
	}
//	dst = buf[0];

	dst = buf[bank];

}



// FFT Stockham,DIT
void fft_dit_Stockham_radix2_type1(const Mat& src, Mat &dst){
	CV_Assert(src.type() == CV_32FC2);
	CV_Assert(src.cols == src.rows);
	int N = src.cols;
	CV_Assert(IsPow2(N));

	enum { _re_ = 0, _im_ = 1 };

	Mat buf[2];
	buf[0] = src.clone();
	buf[1] = Mat(src.size(), src.type());

	vector<Vec2f> w(N / 2);

	// --- twidle ----
	for (int n = 0; n < N / 2; n++){
		float jw = (float)(-2 * M_PI * n / N);
		w[n][_re_] = cos(jw);
		w[n][_im_] = sin(jw);
	}

	int Q = 0;
	while ((1 << Q) < N){ Q++; }

	// --- FFT rows ----
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int row = 0; row < src.rows; row++){
		int bank = 0;

		for (int p = 0, q = Q - 1; q >= 0; p++, q--, bank = bank ^ 1) {

			Vec2f* x = (Vec2f*)buf[bank].ptr<Vec2f>(row);
			Vec2f* y = (Vec2f*)buf[bank ^ 1].ptr<Vec2f>(row);

			//cout << "------------------------" << endl;
			//cout << "p:" << p << "\t";
			//cout << "q:" << q << "\t";
			//cout << endl;

			for (int n = 0; n<N / 2; n = n++) {
				int iw = (n >> q) << q;
				int ix0 = insertZeroBits(n, q, 1);
				int ix1 = ix0 + (1 << q);
				int iy0 = n;
				int iy1 = iy0 + N / 2;

				//cout << "w(" << iw << ")\t";
				//cout << "src(" << ix0 << "," << ix1 << ")\t";
				//cout << "dst(" << iy0 << "," << iy1 << ")\t";
				//cout << endl;

				Vec2f tmp;
				tmp[_re_] = x[ix1][_re_] * w[iw][_re_] - x[ix1][_im_] * w[iw][_im_];
				tmp[_im_] = x[ix1][_re_] * w[iw][_im_] + x[ix1][_im_] * w[iw][_re_];

				y[iy0] = x[ix0] + tmp;
				y[iy1] = x[ix0] - tmp;

			}
		}
	}
#if 1
	// --- FFT cols ----
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int col = 0; col < src.cols; col++){

		int bank = (Q & 1);	//FFT rows ‚Ì‰ñ”‚ªŠï”‚È‚ç‚ÎŠï”bank‚©‚ç

		for (int p = 0, q = Q - 1; q >= 0; p++, q--, bank = bank ^ 1) {
			Vec2f* x = (Vec2f*)buf[bank].ptr<Vec2f>(0, col);
			Vec2f* y = (Vec2f*)buf[bank ^ 1].ptr<Vec2f>(0, col);

			for (int n = 0; n<N / 2; n = n++) {
				int iw = (n >> q) << q;
				int ix0 = insertZeroBits(n, q, 1);
				int ix1 = ix0 + (1 << q);
				int iy0 = n;
				int iy1 = iy0 + N / 2;

				ix0 *= src.cols;
				ix1 *= src.cols;
				iy0 *= src.cols;
				iy1 *= src.cols;

				Vec2f tmp;
				tmp[_re_] = x[ix1][_re_] * w[iw][_re_] - x[ix1][_im_] * w[iw][_im_];
				tmp[_im_] = x[ix1][_re_] * w[iw][_im_] + x[ix1][_im_] * w[iw][_re_];

				y[iy0] = x[ix0] + tmp;
				y[iy1] = x[ix0] - tmp;

			}
		}
	}
#endif

	dst = buf[0];

}



//-----------------------------------------
// FFT Stockham,DIF radix 4 

static Vec2f mul(Vec2f a, Vec2f b){
	enum { _re_ = 0, _im_ = 1 };
	Vec2f ans;
	ans[_re_] = a[_re_] * b[_re_] - a[_im_] * b[_im_];
	ans[_im_] = a[_re_] * b[_im_] + a[_im_] * b[_re_];
	return ans;
	
}

void _fft_dif_Stockham_radix4_type0(int n, int s, bool eo, Vec2f* x, Vec2f* y)
{
	static const Vec2f j = Vec2f(0, 1);
	const int n0 = 0;
	const int n1 = n / 4;
	const int n2 = n / 2;
	const int n3 = n1 + n2;
	const double theta0 = 2 * M_PI / n;

	if (n == 1) { if (eo) for (int q = 0; q < s; q++) y[q] = x[q]; }
	else if (n == 2) {
		for (int q = 0; q < s; q++) {
			const Vec2f a = x[q + 0];
			const Vec2f b = x[q + s];
			y[q + 0] = a + b;
			y[q + s] = a - b;
		}
		_fft_dif_Stockham_radix4_type0(1, 2 * s, !eo, y, x);
	}
	else if (n > 2) {
		for (int p = 0; p < n1; p++) {
			const Vec2f w1p = Vec2f((float)cos(p*theta0), (float)-sin(p*theta0));
			const Vec2f w2p = mul(w1p,w1p);
			const Vec2f w3p = mul(w1p,w2p);
			for (int q = 0; q < s; q++) {
				const Vec2f a = x[q + s*(p + n0)];
				const Vec2f b = x[q + s*(p + n1)];
				const Vec2f c = x[q + s*(p + n2)];
				const Vec2f d = x[q + s*(p + n3)];
				const Vec2f  apc = a + c;
				const Vec2f  amc = a - c;
				const Vec2f  bpd = b + d;
				const Vec2f jbmd = mul(j,(b - d));
				y[q + s*(4 * p + 0)] = apc + bpd;
				y[q + s*(4 * p + 1)] = mul(w1p,(amc - jbmd));
				y[q + s*(4 * p + 2)] = mul(w2p,(apc - bpd));
				y[q + s*(4 * p + 3)] = mul(w3p,(amc + jbmd));
			}
		}
		_fft_dif_Stockham_radix4_type0(n / 4, 4 * s, !eo, y, x);
	}
}

void fft_dif_Stockham_radix4_type0(const Mat& src, Mat &dst)
{
	CV_Assert(src.type() == CV_32FC2);
//	CV_Assert(src.cols == src.rows);
	int n = src.cols;
	CV_Assert(IsPow2(n));

	Mat buf[2];
	buf[0] = src.clone();
	buf[1] = Mat(src.size(), src.type());

	//row fft
	for (int i = 0; i < n; i++){
		Vec2f* x = buf[0].ptr<Vec2f>(i);
		Vec2f* tmp = buf[1].ptr<Vec2f>(i);
		_fft_dif_Stockham_radix4_type0(n, 1, 0, x, tmp);
	}

	cv::transpose(buf[0], buf[0]);
	for (int i = 0; i < n; i++){
		Vec2f* x = buf[0].ptr<Vec2f>(i);
		Vec2f* tmp = buf[1].ptr<Vec2f>(i);
		_fft_dif_Stockham_radix4_type0(n, 1, 0, x, tmp);
	}

	cv::transpose(buf[0], buf[0]);


//	for (int k = 0; k < n; k++) x[k] /= n;

	dst = buf[0];
}

//---------------------------------------

#if 0
void _fft_dit_Stockham_radix4_type0(int n, int s, bool eo, Vec2f* x, Vec2f* y)
{
	static const Vec2f j = Vec2f(0, 1);
	const int n0 = 0;
	const int n1 = n / 4;
	const int n2 = n / 2;
	const int n3 = n1 + n2;
	const double theta0 = 2 * M_PI / n;


	if (n == 1) { if (eo) for (int q = 0; q < s; q++) x[q] = y[q]; }
	else if (n == 2) {
		_fft_dit_Stockham_radix4_type0(1, 2 * s, !eo, y, x);

		cout << "--------------- radix2" << endl;
		for (int q = 0; q < s; q++) {
			cout << "n:" << n << "\t";
			cout << "q:" << q << "\t";
			cout << endl;

			int ix0 = q + 0;
			int ix1 = q + s;
			int iy0 = q + 0;
			int iy1 = q + s;

			cout << "src(" << ix0 << "," << ix1 << ")\t";
			cout << "dst(" << iy0 << "," << iy1 << ")\t";
			cout << endl;


			const Vec2f a = y[q + 0];
			const Vec2f b = y[q + s];
			x[q + 0] = a + b;
			x[q + s] = a - b;
		}
	}
	else if (n > 2) {
		_fft_dit_Stockham_radix4_type0(n / 4, 4 * s, !eo, y, x);

		cout << "--------------- radix4" << endl;
		for (int p = 0; p < n1; p++) {
			cout << "n:" << n << "\t";
			cout << "p:" << p << "\t";
			cout << endl;

			const Vec2f w0p = Vec2f(1.0f, 0.0f);
			const Vec2f w1p = Vec2f((float)cos(p*theta0), (float)-sin(p*theta0));
			const Vec2f w2p = mul(w1p, w1p);
			const Vec2f w3p = mul(w1p, w2p);
			for (int q = 0; q < s; q++) {
				int ix0 = q + s*(4 * p + 0);
				int ix1 = q + s*(4 * p + 1);
				int ix2 = q + s*(4 * p + 2);
				int ix3 = q + s*(4 * p + 3);
				int iy0 = q + s*(p + n0);
				int iy1 = q + s*(p + n1);
				int iy2 = q + s*(p + n2);
				int iy3 = q + s*(p + n3);
				cout << "src(" << ix0 << "," << ix1 << "," << ix2 << "," << ix3 << ")\t";
				cout << "dst(" << iy0 << "," << iy1 << "," << iy2 << "," << iy3 << ")\t";
				cout << "w(" << w0p << "," << w1p << "," << w2p << "," << w3p << ")\t";
				cout << endl;

				const Vec2f a = y[ix0];
				const Vec2f b = mul(y[ix1], w1p);
				const Vec2f c = mul(y[ix2], w2p);
				const Vec2f d = mul(y[ix3], w3p);
				const Vec2f  apc = a + c;
				const Vec2f  amc = a - c;
				const Vec2f  bpd = b + d;
				const Vec2f jbmd = mul(j, (b - d));
				x[iy0] = apc + bpd;
				x[iy1] = amc - jbmd;
				x[iy2] = apc - bpd;
				x[iy3] = amc + jbmd;
			}
		}
	}
}

void fft_dit_Stockham_radix4_type0(const Mat& src, Mat &dst)
{
	CV_Assert(src.type() == CV_32FC2);
//	CV_Assert(src.cols == src.rows);
	int N = src.rows;
	int M = src.cols;

	CV_Assert(IsPow2(N));
	CV_Assert(IsPow2(M));

	dst = src.clone();

	{
		cout << "twidle" << endl;

		vector<Vec2f> w(M / 2);
		// --- twidle ---_
		for (int n = 0; n < M / 2; n++){
			float jw = (float)(-2 * M_PI * n / M);
			w[n][0] = cos(jw);
			w[n][1] = sin(jw);

			cout << "[" << n << "]" << w[n] << endl;
		}
	}

	//row fft
	{
		Mat buf = Mat(dst.size(), dst.type());

		for (int i = 0; i < N; i++){
			Vec2f* x = dst.ptr<Vec2f>(i);
			Vec2f* tmp = buf.ptr<Vec2f>(i);
			_fft_dit_Stockham_radix4_type0(M, 1, 0, x, tmp);
		}
	}

	//col fft
	{
		cv::transpose(dst, dst);
		Mat buf = Mat(dst.size(), dst.type());

		for (int i = 0; i < M; i++){
			Vec2f* x = dst.ptr<Vec2f>(i);
			Vec2f* tmp = buf.ptr<Vec2f>(i);
			_fft_dit_Stockham_radix4_type0(N, 1, 0, x, tmp);
		}
		cv::transpose(dst, dst);
	}


	//	for (int k = 0; k < n; k++) x[k] /= n;

}

#else

// FFT Stockham,DIT
void fft_dit_Stockham_radix4_type0(const Mat& src, Mat &dst){
	CV_Assert(src.type() == CV_32FC2);
	//	CV_Assert(src.cols == src.rows);
	int N;
	if (src.cols == src.rows){
		N = src.cols;
	}
	else if (src.cols == 1){
		N = src.rows;
	}
	else if (src.rows == 1){
		N = src.cols;
	}
	else{
		CV_Assert(0);
	}

	CV_Assert(IsPow2(N));

	enum { _re_ = 0, _im_ = 1 };


	Mat buf[2];
	buf[0] = src.clone();
	buf[1] = Mat(src.size(), src.type());


	static const Vec2f j = Vec2f(0, 1);
	const double theta0 = 2 * M_PI / N;


	vector<Vec2f> w(N / 2);


	// --- twidle ----
	for (int n = 0; n < N / 2; n++){
		float jw = (float)(-2 * M_PI * n / N);
		w[n][_re_] = cos(jw);
		w[n][_im_] = sin(jw);
	}



	/* FFT‚ÌŒvŽZ */

	int bank = 0;
	// --- FFT rows ----
	if (src.cols != 1){
		int Q = 0;
		while ((1 << Q) < N){ Q++; }

		if (Q % 2){
			for (int row = 0; row < src.rows; row++){
				Vec2f* x = (Vec2f*)buf[bank].ptr<Vec2f>(row);
				Vec2f* y = (Vec2f*)buf[bank ^ 1].ptr<Vec2f>(row);

				cout << "---radix2(x2)---" << endl;

				for (int n = 0; n < N / 4; n = n++) {

					int ix = n;
					int ix0 = ix + (N / 4) * 0;
					int ix2 = ix + (N / 4) * 1;
					int ix1 = ix + (N / 4) * 2;
					int ix3 = ix + (N / 4) * 3;

					int iy = n;
					int iy0 = iy + (N / 4) * 0;
					int iy2 = iy + (N / 4) * 1;
					int iy1 = iy + (N / 4) * 2;
					int iy3 = iy + (N / 4) * 3;

					cout << "src(" << ix0 << "," << ix1 << "," << ix2 << "," << ix3 << ")\t";
					cout << "dst(" << iy0 << "," << iy1 << "," << iy2 << "," << iy3 << ")\t";
					cout << endl;

					const Vec2f a0 = x[ix0];
					const Vec2f b0 = x[ix1];
					const Vec2f a1 = x[ix2];
					const Vec2f b1 = x[ix3];

					y[iy0] = a0 + b0;
					y[iy1] = a0 - b0;
					y[iy2] = a1 + b1;
					y[iy3] = a1 - b1;

				}
			}
			Q -= 1;
			bank ^= 1;
		}

		for (int row = 0; row < src.rows; row++){
			for (int p = 0, q = Q - 2; q >= 0; p += 2, q -= 2, bank = bank ^ 1) {
				Vec2f* x = (Vec2f*)buf[bank].ptr<Vec2f>(row);
				Vec2f* y = (Vec2f*)buf[bank ^ 1].ptr<Vec2f>(row);

				cout << "---radix4(x4)---" << endl;
				cout << "p:" << p << "\t";
				cout << "q:" << q << "\t";
				cout << endl;

				for (int n = 0; n < N / 4; n = n++) {
					int iw = (n >> q) << q;
					int ix = insertZeroBits(n, q, 2);
					int ix0 = ix + (1 << q) * 0;
					int ix1 = ix + (1 << q) * 1;
					int ix2 = ix + (1 << q) * 2;
					int ix3 = ix + (1 << q) * 3;

					int iy = n;
					int iy0 = iy + (N / 4) * 0;
					int iy1 = iy + (N / 4) * 1;
					int iy2 = iy + (N / 4) * 2;
					int iy3 = iy + (N / 4) * 3;

					const Vec2f w0 = Vec2f(1.0f, 0.0f);
					const Vec2f w1 = w[iw];
					const Vec2f w2 = mul(w1, w1);
					const Vec2f w3 = mul(w1, w2);

					//cout << "w(" << iw << ")\t";
					cout << "src(" << ix0 << "," << ix1 << "," << ix2 << "," << ix3 << ")\t";
					cout << "dst(" << iy0 << "," << iy1 << "," << iy2 << "," << iy3 << ")\t";
					cout << "w(" << w0 << "," << w1 << "," << w2 << "," << w3 << ")\t";
					cout << endl;

					const Vec2f a = x[ix0];
					const Vec2f b = mul(x[ix1], w1);
					const Vec2f c = mul(x[ix2], w2);
					const Vec2f d = mul(x[ix3], w3);
					const Vec2f  apc = a + c;
					const Vec2f  amc = a - c;
					const Vec2f  bpd = b + d;
					const Vec2f jbmd = mul(j, (b - d));

					y[iy0] = apc + bpd;
					y[iy1] = amc - jbmd;
					y[iy2] = apc - bpd;
					y[iy3] = amc + jbmd;
				}
			}
		}
	}
	// --- FFT cols ----
	if (src.rows != 1){
		int Q = 0;
		while ((1 << Q) < N){ Q++; }

		if (Q % 2){
			for (int col = 0; col < src.cols; col++){
				Vec2f* x = (Vec2f*)buf[bank].ptr<Vec2f>(0, col);
				Vec2f* y = (Vec2f*)buf[bank ^ 1].ptr<Vec2f>(0, col);
		
				cout << "---radix2(x2)---" << endl;

				for (int n = 0; n < N / 4; n = n++) {

					int ix = n;
					int ix0 = ix + (N / 4) * 0;
					int ix2 = ix + (N / 4) * 1;
					int ix1 = ix + (N / 4) * 2;
					int ix3 = ix + (N / 4) * 3;

					int iy = n;
					int iy0 = iy + (N / 4) * 0;
					int iy2 = iy + (N / 4) * 1;
					int iy1 = iy + (N / 4) * 2;
					int iy3 = iy + (N / 4) * 3;

					ix0 *= src.cols;
					ix1 *= src.cols;
					ix2 *= src.cols;
					ix3 *= src.cols;
					iy0 *= src.cols;
					iy1 *= src.cols;
					iy2 *= src.cols;
					iy3 *= src.cols;


					cout << "src(" << ix0 << "," << ix1 << "," << ix2 << "," << ix3 << ")\t";
					cout << "dst(" << iy0 << "," << iy1 << "," << iy2 << "," << iy3 << ")\t";
					cout << endl;

					const Vec2f a0 = x[ix0];
					const Vec2f b0 = x[ix1];
					const Vec2f a1 = x[ix2];
					const Vec2f b1 = x[ix3];

					y[iy0] = a0 + b0;
					y[iy1] = a0 - b0;
					y[iy2] = a1 + b1;
					y[iy3] = a1 - b1;

				}
			}
			Q -= 1;
			bank ^= 1;

		}

		for (int col = 0; col < src.cols; col++){
			for (int p = 0, q = Q - 2; q >= 0; p += 2, q -= 2, bank = bank ^ 1) {
				Vec2f* x = (Vec2f*)buf[bank].ptr<Vec2f>(0, col);
				Vec2f* y = (Vec2f*)buf[bank ^ 1].ptr<Vec2f>(0, col);

				cout << "------------------------" << endl;
				cout << "p:" << p << "\t";
				cout << "q:" << q << "\t";
				cout << endl;

				for (int n = 0; n < N / 4; n = n++) {

					int iw = (n >> q) << q;
					int ix = insertZeroBits(n, q, 2);
					int ix0 = ix + (1 << q) * 0;
					int ix1 = ix + (1 << q) * 1;
					int ix2 = ix + (1 << q) * 2;
					int ix3 = ix + (1 << q) * 3;

					int iy = n;
					int iy0 = iy + (N / 4) * 0;
					int iy1 = iy + (N / 4) * 1;
					int iy2 = iy + (N / 4) * 2;
					int iy3 = iy + (N / 4) * 3;

					const Vec2f w0 = Vec2f(1.0f, 0.0f);
					const Vec2f w1 = w[iw];
					const Vec2f w2 = mul(w1, w1);
					const Vec2f w3 = mul(w1, w2);

					ix0 *= src.cols;
					ix1 *= src.cols;
					ix2 *= src.cols;
					ix3 *= src.cols;
					iy0 *= src.cols;
					iy1 *= src.cols;
					iy2 *= src.cols;
					iy3 *= src.cols;

					const Vec2f a = x[ix0];
					const Vec2f b = mul(x[ix1], w1);
					const Vec2f c = mul(x[ix2], w2);
					const Vec2f d = mul(x[ix3], w3);
					const Vec2f  apc = a + c;
					const Vec2f  amc = a - c;
					const Vec2f  bpd = b + d;
					const Vec2f jbmd = mul(j, (b - d));

					y[iy0] = apc + bpd;
					y[iy1] = amc - jbmd;
					y[iy2] = apc - bpd;
					y[iy3] = amc + jbmd;
				}
			}

		}
	}
	//	dst = buf[0];

	dst = buf[bank];

}


#endif