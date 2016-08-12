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
	GAUSS,
	BILATERAL,
	SOBEL_H,
	SOBEL_V,
	EDGE,
	LAPLACIAN,
	THRESH,
	ADAPTIVE_THRESH,
	CANNY,
	FFT,
	FFT_RECT,
	MODE_MASK = 0x0000FFFF,
	CTRL_MASK = 0xFFFF0000,
	ENABLE_PRE_FILTER = 1 << 16,
	RESET_ACC = 1 << 17,
};

enum E_CAM_ZOOM {
	ZOOMxMIN,
	ZOOMx0500,
	ZOOMx0707,
	ZOOMx1000,
	ZOOMx1414,
	ZOOMx2000,
	ZOOMxMAX,
};

static void set_mode(int& mode, const int new_mode){
	mode = (mode & E_CAM_MODE::CTRL_MASK) | new_mode;
}

static void controls(GLFWwindow* window, int& mode, int& zoom){

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) set_mode(mode, E_CAM_MODE::NORMAL);
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) set_mode(mode, E_CAM_MODE::GRAY);
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) set_mode(mode, E_CAM_MODE::BILATERAL);
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) set_mode(mode, E_CAM_MODE::SOBEL_H);
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) set_mode(mode, E_CAM_MODE::SOBEL_V);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) set_mode(mode, E_CAM_MODE::LAPLACIAN);
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) set_mode(mode, E_CAM_MODE::THRESH);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) set_mode(mode, E_CAM_MODE::ADAPTIVE_THRESH);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) set_mode(mode, E_CAM_MODE::FFT);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) set_mode(mode, E_CAM_MODE::FFT_RECT);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) set_mode(mode, E_CAM_MODE::CANNY);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) set_mode(mode, E_CAM_MODE::EDGE);

	static bool key_p_invalid = false;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) key_p_invalid = false;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		if (!key_p_invalid) {
			key_p_invalid = true;
			mode ^= ENABLE_PRE_FILTER;
			if (mode & ENABLE_PRE_FILTER) mode |= RESET_ACC;
		}
	}

	static bool key_page_up_invalid = false;
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_RELEASE) key_page_up_invalid = false;
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS){
		if (!key_page_up_invalid) {
			key_page_up_invalid = true;
			zoom++;
			if (zoom >= E_CAM_ZOOM::ZOOMxMAX) zoom--;
		}
	}

	static bool key_page_down_invalid = false;
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_RELEASE)key_page_down_invalid = false;
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS){
		if (!key_page_down_invalid) {
			key_page_down_invalid = true;
			zoom--;
			if (zoom <= E_CAM_ZOOM::ZOOMxMIN) zoom++;
		}
	}

}



int _tmain(int argc, _TCHAR* argv[])
{
	const GLsizei captureWidth(640);
	const GLsizei captureHeight(480);
//	const GLsizei captureWidth(1280);
//	const GLsizei captureHeight(720);


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

	Size sizeFft(256, 256);
//	Size sizeFft(512, 512);
	Mat fftwin;
	cv::createHanningWindow(fftwin, sizeFft, CV_32F);
	GlsMat glsFftWin(fftwin);
	GlsMat glsZero(Mat::zeros(sizeFft, CV_32FC1));

	Rect rectFft((captureWidth - sizeFft.width) / 2, (captureHeight - sizeFft.height) / 2 , sizeFft.width, sizeFft.height);

	int camMode = E_CAM_MODE::NORMAL;
	int camZoom = E_CAM_ZOOM::ZOOMx1000;

	GlsMat glsFrameAcc;

	cv::TickMeter timer;
	timer.reset();

	do{
		timer.reset();
		timer.start();

		cv::Mat frame;
		camera >> frame;						// キャプチャ映像から画像を切り出す

		GlsMat glsFrame;

		{	// common pre-process
			glsFrame = (GlsMat)frame;							//upload 
			gls::flip(glsFrame, glsFrame, 0);					//flip up to down
			gls::convert(glsFrame, glsFrame, CV_32F,1.0f / 256.0f);	//convert to float
			if (camMode & ENABLE_PRE_FILTER){					//pre-filter
				if (glsFrameAcc.empty() || (camMode & RESET_ACC)){
					gls::copy(glsFrame, glsFrameAcc);
					camMode &= ~RESET_ACC;	//clear bit
				}
				gls::accumulateWeighted(glsFrame, glsFrameAcc, 0.25);
				glsFrame = glsFrameAcc;
			}
		}

		switch (camMode&E_CAM_MODE::MODE_MASK){
		case(E_CAM_MODE::FFT) :	{
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			gls::copyRect(glsFrame, glsFrame, rectFft);
			gls::multiply(glsFftWin, glsFrame, glsFrame);
			gls::fft(glsFrame, glsFrame, gls::GLS_FFT_SHIFT);
			gls::logMagSpectrums(glsFrame, glsFrame, 1.0);
			gls::normalize(glsFrame, glsFrame, 0, 1, NORM_MINMAX);
		}break;
		case(E_CAM_MODE::FFT_RECT) : {
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			gls::copyRect(glsFrame, glsFrame, rectFft);
			gls::multiply(glsFftWin, glsFrame, glsFrame);
		} break;

		case(E_CAM_MODE::ADAPTIVE_THRESH) : {
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			gls::adaptiveThreshold(glsFrame, glsFrame, 1.0, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY,15,0.01);
		}break;
		case(E_CAM_MODE::THRESH) : {
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			Scalar mean = gls::mean(glsFrame);
			gls::threshold(glsFrame, glsFrame, mean[0], 1.0, THRESH_BINARY);
		}break;
		case(E_CAM_MODE::SOBEL_V) : {
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			gls::Sobel(glsFrame, glsFrame, -1, 0, 1, 3, 1.0, 0.5);
		}break;
		case(E_CAM_MODE::SOBEL_H) : {
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			gls::Sobel(glsFrame, glsFrame, -1, 1, 0, 3, 1.0, 0.5);
		}break;
		case(E_CAM_MODE::EDGE) : {
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			GlsMat dx;
			GlsMat dy;
			gls::Sobel(glsFrame, dx, -1, 1, 0, 3, 1.0, 0.0);
			gls::Sobel(glsFrame, dy, -1, 0, 1, 3, 1.0, 0.0);
			vector<GlsMat> mag_rad(2);
			gls::cartToPolar(dx, dy, mag_rad[0], mag_rad[1], false);
			gls::merge(mag_rad, glsFrame);
		}break;
		case(E_CAM_MODE::LAPLACIAN) : {
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			gls::Laplacian(glsFrame, glsFrame, -1, 3, 1.0, 0.5);
		}break;
		case(E_CAM_MODE::GAUSS) : {
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2RGB);
			gls::GaussianBlur(glsFrame, glsFrame, Size(9, 9), 0, 0);
		}break;
		case(E_CAM_MODE::BILATERAL) : {
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2RGB);
			//! tuning param
			const int ksize = 9;				//trade off IQ vs process time
			const double sigmaColor = 0.07;		//measurement noise sigma / trade off image detail vs noise
			const double sigmaSpace = 0.3*(ksize / 2 - 1) + 0.8;
			gls::bilateralFilter(glsFrame, glsFrame, ksize, sigmaColor, sigmaSpace);
		}break;
		case(E_CAM_MODE::CANNY) : {
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			const double high = 0.20;
			const double low = high /2.0;
			gls::Canny(glsFrame, glsFrame, high,low,3,true);
		}break;
		case(E_CAM_MODE::GRAY) : {
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
		}break;
		case(E_CAM_MODE::NORMAL):
		default:{
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2RGB);
		}break;
		}

		switch (camZoom){
		case(E_CAM_ZOOM::ZOOMx1000) : {
		}break;
		case(E_CAM_ZOOM::ZOOMx1414) : {
			gls::resize(glsFrame, glsFrame, Size(0, 0), sqrt(2.0), sqrt(2.0), INTER_CUBIC);
		}break;
		case(E_CAM_ZOOM::ZOOMx2000) : {
			gls::resize(glsFrame, glsFrame, Size(0, 0), 2.0, 2.0, INTER_CUBIC);
		}break;
		case(E_CAM_ZOOM::ZOOMx0707) : {
			gls::resize(glsFrame, glsFrame, Size(0, 0), 1.0 / sqrt(2.0), 1.0 / sqrt(2.0));
		}break;
		case(E_CAM_ZOOM::ZOOMx0500) : {
			gls::resize(glsFrame, glsFrame, Size(0, 0), 0.5, 0.5);
		}break;
		}


		glfwSetWindowSize(window, glsFrame.size().width, glsFrame.size().height);
		gls::draw(glsFrame);

		glfwSwapBuffers(window);  // Swap buffers
		glfwPollEvents();
		controls(window, camMode, camZoom); // key check

		timer.stop();
		double fps = 1.0 / timer.getTimeSec();
		glfwSetWindowTitle(window, ("glsCam [" + to_string(fps) + "]").c_str());

	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);


	glsCvTerminate();

	return 0;
}

