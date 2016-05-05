#pragma once

namespace UnitTest_glsCV
{

	bool AlmostEqualUlpsAbsEps(float A, float B, int maxUlps, float maxDiff = 1e-3);

	bool AreEqual(Mat& mat0, Mat& mat1);

	template<typename T> void FillRandU(Mat &mat){
		RNG rng(0xFFFFFFFF);
		for (int y = 0; y < mat.rows; y++){
			for (int x = 0; x < mat.cols; x++){
				T* pSrc = mat.ptr<T>(y, x);
				for (int ch = 0; ch < mat.channels(); ch++){
					*pSrc++ = randu<T>();
				}
			}
		}
	}



}