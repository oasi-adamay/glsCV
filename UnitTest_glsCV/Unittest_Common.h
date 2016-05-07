#pragma once

namespace UnitTest_glsCV
{

	// calc ULPS (Units in the Last Place)
	template <typename T> static int ULPs(T a, T b){
		return abs((int)a - (int)b);
	}

	template <> static int ULPs(float a, float b){
		int aInt = *(int*)&a;
		// Make aInt lexicographically ordered as a twos-complement int
		if (aInt < 0)
			aInt = 0x80000000 - aInt;
		// Make bInt lexicographically ordered as a twos-complement int
		int bInt = *(int*)&b;
		if (bInt < 0)
			bInt = 0x80000000 - bInt;
		return abs(aInt - bInt);
	}



	bool AlmostEqualUlpsAbsEps(float A, float B, int maxUlps, float maxDiff = 1e-3);


	template <typename T> static
	bool AreEqual(T val0, T val1, int maxUlps){
		return val0 == val1;
	}

	template<> static 
	bool AreEqual<float>(float val0, float val1, int maxUlps){
		return AlmostEqualUlpsAbsEps(val0, val1, maxUlps, FLT_MIN);
	}

	template <typename T> static
	bool AreEqual(Mat& mat0, Mat& mat1, int maxUlpsSpec = 0, float maxDiffSpec = 1e-6){
		if (mat0.size() != mat1.size()){
			cerr << "mat0.size() != mat1.size()" << endl;
			return false;
		}

		if (mat0.type() != mat1.type()){
			cerr << "mat0.type() != mat1.type()" << endl;
			return false;
		}

		int errNum = 0;
		int ulpsMax = 0;
		int ulpsMaxPos[3] = { -1, -1, -1 };
		T   ulpsMaxRef;
		T   ulpsMaxDst;
		for (int y = 0; y < mat0.rows; y++){
			for (int x = 0; x < mat0.cols; x++){
				T* pRef = mat0.ptr<T>(y, x);
				T* pDst = mat1.ptr<T>(y, x);
				for (int ch = 0; ch < mat0.channels(); ch++){
					T ref = *pRef++;
					T dst = *pDst++;
					int ulps = ULPs<T>(ref,dst);
					if (ulps > ulpsMax){
						ulpsMax = ulps;
						ulpsMaxPos[0] = y;
						ulpsMaxPos[1] = x;
						ulpsMaxPos[2] = ch;
						ulpsMaxRef = ref;
						ulpsMaxDst = dst;
					}

					if (ulps > maxUlpsSpec && fabs((float)ref - (float)dst)>maxDiffSpec){
						errNum++;
						if (errNum<100){
							cout << cv::format("(%4d,%4d,%4d)\t", y, x, ch);
							cout << ref << "\t";
							cout << dst << "\t";
							cout << ulps << "\t";
							cout << endl;
						}
					}
				}
			}
		}

		cout << "ULPS spec:" << maxUlpsSpec << endl;
		if (ulpsMax > 0){
			cout << cv::format("(%4d,%4d,%4d)\t", ulpsMaxPos[0], ulpsMaxPos[1], ulpsMaxPos[2]);
			cout << "ULPS:" << ulpsMax << "\t";
			cout << "Ref:" << ulpsMaxRef << "\t";
			cout << "Dst:" << ulpsMaxDst << "\t";
			cout << endl;
		}

		cout << "ErrNum:" << errNum << endl;

		if (errNum == 0) return true;
		return false;
	}


	template<typename T> static
	void FillRandU(Mat &mat){
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