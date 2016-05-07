#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "HookCoutCerr.h"
#include "Timer.h"

#include "glsCV.h"

//Lib 
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "glfw3dll.lib")

#pragma comment (lib, "glsCV.lib")


namespace UnitTest_glsCV
{
	HookCoutCerr hook;
#if 1
	TEST_MODULE_INITIALIZE(test_module_initialize) {
		cout << __FUNCTION__ << endl;
		glsCvInit();

	}
	TEST_MODULE_CLEANUP(test_module_cleanup)  {
		cout << __FUNCTION__ << endl;
		glsCvTerminate();
		Logger::WriteMessage("TEST_MODULE_CLEANUP end");
	}
#endif

	bool AlmostEqualUlpsAbsEps(float A, float B, int maxUlps, float maxDiff)
	{
		// Check if the numbers are really close -- needed
		// when comparing numbers near zero.
		float absDiff = fabs(A - B);
		if (absDiff <= maxDiff)
			return true;

		// Make sure maxUlps is non-negative and small enough that the
		// default NAN won't compare as equal to anything.
		GLS_Assert(maxUlps >= 0 && maxUlps < 4 * 1024 * 1024);
		int aInt = *(int*)&A;
		// Make aInt lexicographically ordered as a twos-complement int
		if (aInt < 0)
			aInt = 0x80000000 - aInt;
		// Make bInt lexicographically ordered as a twos-complement int
		int bInt = *(int*)&B;
		if (bInt < 0)
			bInt = 0x80000000 - bInt;
		int intDiff = abs(aInt - bInt);
		if (intDiff <= maxUlps)
			return true;


		return false;
	}




#if 0
	bool AreEqual(Mat& mat0, Mat& mat1){
		if (mat0.size() != mat1.size()){
			cerr << "mat0.size() != mat1.size()" << endl;
			return false;
		}

		if (mat0.type() != mat1.type()){
			cerr << "mat0.type() != mat1.type()" << endl;
			return false;
		}

		//verify
		int errNum = 0;
		{
			Mat cmp_ne;
			cv::compare(mat0, mat1, cmp_ne, CMP_NE);
			vector<Mat> pln_ne;
			cv::split(cmp_ne, pln_ne);
			for (int i = 0; i< pln_ne.size(); i++){
				errNum += cv::countNonZero(pln_ne[i]);
			}
		}
		cout << "errNum:" << errNum << endl;

		if (errNum == 0) return true;
#if 1
		{
			cout << mat0 << endl;
			cout << mat1 << endl;
		}
#endif

		return false;
	}
#endif



}