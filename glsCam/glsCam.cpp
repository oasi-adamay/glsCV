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


// glsCam.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include "glsCV.h"



//Lib 
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "glfw3dll.lib")

#pragma comment (lib, "glsCV.lib")

enum E_CAM_MODE {
	NORMAL,
	GRAY,
	FFT,
	FFT_RECT,
};

void controls(GLFWwindow* window, int& mode){

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) mode = E_CAM_MODE::FFT;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) mode = E_CAM_MODE::FFT_RECT;
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) mode = E_CAM_MODE::GRAY;
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) mode = E_CAM_MODE::NORMAL;
}



int _tmain(int argc, _TCHAR* argv[])
{
//	const GLsizei captureWidth(640);
//	const GLsizei captureHeight(480);
	const GLsizei captureWidth(1280);
	const GLsizei captureHeight(720);


	GLFWwindow* window = glsCvInit(captureWidth, captureHeight);
	
	
	// OpenCV によるビデオキャプチャを初期化する
	cv::VideoCapture camera(CV_CAP_ANY);
	if (!camera.isOpened())
	{
		std::cerr << "cannot open input" << std::endl;
		exit(-1);
	}

	// カメラの初期設定
	camera.grab();
	camera.set(CV_CAP_PROP_FRAME_WIDTH, double(captureWidth));
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, double(captureHeight));

//	Size sizeFft(256, 256);
	Size sizeFft(512, 512);
	Mat fftwin;
	cv::createHanningWindow(fftwin, sizeFft, CV_32F);
	glsMat glsFftWin(fftwin);
	glsMat glsZero(Mat::zeros(sizeFft, CV_32FC1));

	Rect rectFft((captureWidth - sizeFft.width) / 2, (captureHeight - sizeFft.height) / 2 , sizeFft.width, sizeFft.height);


	int camMode = E_CAM_MODE::FFT;
	do{
		cv::Mat frame;
		camera >> frame;						// キャプチャ映像から画像を切り出す
		imshow("[OCV]", frame.clone());

		cv::flip(frame, frame, 0);				// 上下反転

		glsMat glsFrame;

		switch (camMode){
		case(E_CAM_MODE::FFT) :	{
#if 0
			Mat roi = Mat(frame, rect).clone();
			cv::cvtColor(roi,roi,CV_BGR2GRAY);
			roi.convertTo(roi, CV_32FC1,1.0/256.0);
			Mat zero = Mat::zeros(roi.size(),roi.type());
			vector<Mat> pln(2);
			pln[0] = roi;
			pln[1] = zero;
			Mat img;
			cv::merge(pln, img);
			cv::dft(img, img);
			cv::split(img, pln);
			Mat mag;
			cv::magnitude(pln[0], pln[1], mag);
			cv::log(mag+1,mag);
			cv::normalize(mag, mag, 0, 1, CV_MINMAX);
			cv::imshow("[CV MAG]",mag);
#else
			Mat roi = Mat(frame, rectFft);
			glsFrame = roi;
			glsConvert(glsFrame, glsFrame, 1.0f / 256.0f);
			glsCvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			glsMultiply(glsFftWin, glsFrame, glsFrame);
			vector<glsMat> plnGls(2);
			plnGls[0] = glsFrame;
			plnGls[1] = glsZero;
			glsMat glsComplx;
			glsMerge(plnGls, glsComplx);
			glsFft(glsComplx, glsComplx, GLS_FFT_SHIFT);
			glsLogMagSpectrums(glsComplx, glsFrame, 1.0);
			glsNormalize(glsFrame, glsFrame, 0, 1, NORM_MINMAX);
#endif
		}break;
		case(E_CAM_MODE::FFT_RECT) : {
			Mat roi = Mat(frame, rectFft);
			glsFrame = roi;
			glsConvert(glsFrame, glsFrame, 1.0f / 256.0f);
			glsCvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			glsMultiply(glsFftWin, glsFrame, glsFrame);
		} break;
		case(E_CAM_MODE::GRAY) : {
			glsFrame = frame;
			glsConvert(glsFrame, glsFrame, 1.0f / 256.0f);
			glsCvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
		}break;
		case(E_CAM_MODE::NORMAL):
		default:{
			glsFrame = frame;
			glsConvert(glsFrame, glsFrame, 1.0f / 256.0f);
			glsCvtColor(glsFrame, glsFrame, CV_BGR2RGB);
		}break;
		}

		glfwSetWindowSize(window, glsFrame.size().width, glsFrame.size().height);
		glsDraw(glsFrame);

		glfwSwapBuffers(window);  // Swap buffers
		glfwPollEvents();
		controls(window, camMode); // key check
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);


	glsCvTerminate();

	return 0;
}

