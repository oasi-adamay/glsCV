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

	template <typename T>
	int test_glsMerge(int cvtype, int cn,int flag = 0){
//		const int width = 32;
//		const int height = 24;
		const int width = 8;
		const int height = 6;
		Size blkNum;
		if (flag == 0)blkNum = Size(1, 1);
		else blkNum = Size(2, 2);

		vector<Mat> plnSrc(cn);
		for (int c = 0; c < cn; c++){
			plnSrc[c] = Mat(Size(width, height), cvtype);
			//init Src image
			FillRandU<T>(plnSrc[c]);
		}

		Mat imgRef;
		Mat imgDst;

		cout << "Size:" << plnSrc[0].size() << endl;

		cv::merge(plnSrc,imgRef);

		//----------------------
		vector<glsMat> plnGlsSrc(cn);
		for (int c = 0; c < cn; c++){
			plnGlsSrc[c] = glsMat(plnSrc[c]);
		}

		glsMat glsDst;
		glsMerge(plnGlsSrc, glsDst);	//copy texture

		glsDst.CopyTo(imgDst);		// download

		int errNum = 0;
		if (!AreEqual<T>(imgRef, imgDst)) errNum = -1;

		//cout << imgRef << endl;
		//cout << imgDst << endl;
		//cout << imgDst - imgRef << endl;


		return errNum;
	}




	TEST_CLASS(UnitTest_glsMerge)
	{
	public:
		//glsMerge
		TEST_METHOD(glsMerge_CV_8UC1_2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMerge<uchar>(CV_8UC1, 2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMerge_CV_8UC1_3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMerge<uchar>(CV_8UC1, 3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMerge_CV_8UC1_4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMerge<uchar>(CV_8UC1, 4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMerge_CV_32FC1_2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMerge<float>(CV_32FC1,2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(glsMerge_CV_32FC1_2_TILED)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_glsMerge<float>(CV_32FC1, 2 , 1);
			Assert::AreEqual(0, errNum);
		}

	};


}