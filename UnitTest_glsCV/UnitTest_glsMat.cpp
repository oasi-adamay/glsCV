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
#include "UnitTest_Common.h"

//Lib 
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "glfw3dll.lib")

#pragma comment (lib, "glsCV.lib")


namespace UnitTest_glsCV
{

	template <typename T>
	int test_GlsMat_transfer(const int cvtype, const Size size = Size(32, 24)){
		Mat imgSrc = Mat(size, cvtype);
		Mat imgDst = Mat::zeros(imgSrc.size(), imgSrc.type());
//		Mat imgDst;

		cout << "Size:" << imgSrc.size() << endl;

		//---------------------------------
		//init Src image
		FillRandU<T>(imgSrc);

		GlsMat glsSrc(imgSrc.size(), imgSrc.type());		//create GlsMat
		int loop = 1;
		if (size.width >= 256)loop = 10;
		for (int i = 0; i < loop;i++){
			{
				_TMR_("GlsMat::upload:\t");
				glsSrc.upload(imgSrc);	//upload
			}
			{
				_TMR_("GlsMat::download:\t");
				glsSrc.download(imgDst);		// download
			}
		}


		//verify
		int errNum = 0;
		if (!AreEqual<T>(imgSrc, imgDst)) errNum = -1;

		//if (errNum){
		//	cout << imgSrc << endl;
		//	cout << imgDst << endl;
		//}

		return errNum;
	}

	template <typename T>
	int test_GlsMat_transfer3D(const int cvtype, const int width = 32, const int height = 24, const int depth = 8){
		int sizes[3];
		sizes[0] = depth;
		sizes[1] = height;
		sizes[2] = width;
		
		Mat imgSrc = Mat(3,sizes, cvtype);
		Mat imgDst = Mat::zeros(3, sizes, imgSrc.type());

		cout << "Size[0]:" << imgSrc.size[0] << endl;
		cout << "Size[1]:" << imgSrc.size[1] << endl;
		cout << "Size[2]:" << imgSrc.size[2] << endl;

		//---------------------------------
		//init Src image
		FillRandU<T>(imgSrc);

		GlsMat glsSrc(imgSrc.dims, imgSrc.size, imgSrc.type());		//create GlsMat
		{
			_TMR_("GlsMat::upload:\t");
			glsSrc.upload(imgSrc);	//upload
		}
		{
			_TMR_("GlsMat::download:\t");
			glsSrc.download(imgDst);		// download
		}


		//verify
		int errNum = 0;
		if (!AreEqual<T>(imgSrc, imgDst)) errNum = -1;


		return errNum;
	}


	TEST_CLASS(UnitTest_GlsMat)
	{
	public:
		TEST_METHOD(GlsMat_transfer_CV_8UC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<uchar>(CV_8UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_8UC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<uchar>(CV_8UC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_8UC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<uchar>(CV_8UC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_8UC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<uchar>(CV_8UC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_8SC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<schar>(CV_8SC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_8SC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<schar>(CV_8SC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_8SC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<schar>(CV_8SC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_8SC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<schar>(CV_8SC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_16SC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<short>(CV_16SC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_16SC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<short>(CV_16SC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_16SC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_transfer<short>(CV_16SC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_16SC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_transfer<short>(CV_16SC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_16UC1)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_transfer<ushort>(CV_16UC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_16UC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_transfer<ushort>(CV_16UC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_16UC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_transfer<ushort>(CV_16UC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_16UC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_transfer<ushort>(CV_16UC4);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_32SC1)
		{
			cout << __FUNCTION__ << endl;
			const int width  = 32;
			const int height = 24;
			int errNum = test_GlsMat_transfer<int>(CV_32SC1);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_32SC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_transfer<int>(CV_32SC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_32SC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_transfer<int>(CV_32SC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_32SC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_transfer<int>(CV_32SC4);
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(GlsMat_transfer_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_transfer<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(GlsMat_transfer_CV_32FC1)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()



		TEST_METHOD(GlsMat_transfer_CV_32FC2)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_transfer<float>(CV_32FC2);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_32FC3)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_transfer<float>(CV_32FC3);
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_32FC4)
		{
			cout << __FUNCTION__ << endl;
			const int width = 32;
			const int height = 24;
			int errNum = test_GlsMat_transfer<float>(CV_32FC4);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(GlsMat_transfer_CV_8UC1_4x8)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<uchar>(CV_8UC1, Size(4, 8));
			Assert::AreEqual(0, errNum);
		}
		TEST_METHOD(GlsMat_transfer_CV_8UC1_4x3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<uchar>(CV_8UC1, Size(4, 3));
			Assert::AreEqual(0, errNum);
		}



		TEST_METHOD(GlsMat_transfer_CV_32FC1_5x5)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<float>(CV_32FC1, Size(5, 5));
			Assert::AreEqual(0, errNum);
		}


		TEST_METHOD(GlsMat_transfer_CV_32FC1_1024x1024)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<float>(CV_32FC1, Size(1024, 1024));
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(GlsMat_transfer_CV_32FC1_1024x1024)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"benchmark")
		END_TEST_METHOD_ATTRIBUTE()


		TEST_METHOD(GlsMat_transfer_CV_32FC1_1x1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<float>(CV_32FC1, Size(1, 1));
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(GlsMat_transfer_CV_32FC1_3x4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<float>(CV_32FC1, Size(3, 4));
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(GlsMat_transfer_CV_32SC1_1x1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<int>(CV_32SC1, Size(1, 1));
			Assert::AreEqual(0, errNum);
		}


		//@@@ TODO issue #6
		TEST_METHOD(GlsMat_transfer_CV_8UC1_3x4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<uchar>(CV_8UC1, Size(3, 4));
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(GlsMat_transfer_CV_8UC1_3x4)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"TODO")
			TEST_IGNORE()
		END_TEST_METHOD_ATTRIBUTE()

		TEST_METHOD(GlsMat_transfer_CV_16UC1_3x4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer<ushort>(CV_16UC1, Size(3, 4));
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(GlsMat_transfer_CV_16UC1_3x4)
			//TEST_OWNER(L"OwnerName")
			//TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"TODO")
			TEST_IGNORE()
		END_TEST_METHOD_ATTRIBUTE()



		TEST_METHOD(GlsMat_transfer3D_CV_32FC1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer3D<float>(CV_32FC1);
			Assert::AreEqual(0, errNum);
		}
		BEGIN_TEST_METHOD_ATTRIBUTE(GlsMat_transfer3D_CV_32FC1)
			//TEST_OWNER(L"OwnerName")
			TEST_PRIORITY(1)
			TEST_MY_TRAIT(L"basic")
		END_TEST_METHOD_ATTRIBUTE()


		TEST_METHOD(GlsMat_transfer3D_CV_32FC2)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer3D<float>(CV_32FC2);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(GlsMat_transfer3D_CV_32FC3)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer3D<float>(CV_32FC3);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(GlsMat_transfer3D_CV_32FC4)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer3D<float>(CV_32FC4);
			Assert::AreEqual(0, errNum);
		}

		TEST_METHOD(GlsMat_transfer3D_CV_32FC1_32x24x1)
		{
			cout << __FUNCTION__ << endl;
			int errNum = test_GlsMat_transfer3D<float>(CV_32FC1,32,24,1);
			Assert::AreEqual(0, errNum);
		}



	};


}