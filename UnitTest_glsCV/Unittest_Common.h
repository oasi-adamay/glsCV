#pragma once

namespace UnitTest_glsCV
{

	bool AlmostEqualUlpsAbsEps(float A, float B, int maxUlps, float maxDiff = 1e-3);


	template <typename T>
	static bool AreEqual(T val0, T val1, int maxUlps){
		return val0 == val1;
	}

	template<> static bool AreEqual<float>(float val0, float val1, int maxUlps){
		return AlmostEqualUlpsAbsEps(val0, val1, maxUlps, FLT_MIN);
	}

	template <typename T>
	bool AreEqual(Mat& mat0, Mat& mat1, int maxUlps = 0){
		if (mat0.size() != mat1.size()){
			cerr << "mat0.size() != mat1.size()" << endl;
			return false;
		}

		if (mat0.type() != mat1.type()){
			cerr << "mat0.type() != mat1.type()" << endl;
			return false;
		}

		int errNum = 0;
		for (int y = 0; y < mat0.rows; y++){
			for (int x = 0; x < mat0.cols; x++){
				T* pRef = mat0.ptr<T>(y, x);
				T* pDst = mat1.ptr<T>(y, x);
				for (int ch = 0; ch < mat0.channels(); ch++){
					T ref = *pRef++;
					T dst = *pDst++;
					if (!AreEqual<T>(ref, dst, maxUlps)){
						errNum++;
						if (errNum<100){
							cout << cv::format("(%4d,%4d,%4d)\t", y, x, ch);
							cout << ref << "\t";
							cout << dst << "\t";
							cout << endl;
						}
					}
				}
			}
		}
		if (errNum == 0) return true;
		return false;
	}


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