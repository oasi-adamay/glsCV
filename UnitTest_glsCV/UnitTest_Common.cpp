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
		gls::ShaderInitAll();
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