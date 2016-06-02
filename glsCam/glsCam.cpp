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


// glsCam.cpp : ÉRÉìÉ\Å[Éã ÉAÉvÉäÉPÅ[ÉVÉáÉìÇÃÉGÉìÉgÉä É|ÉCÉìÉgÇíËã`ÇµÇ‹Ç∑ÅB
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

void controls(GLFWwindow* window, int& mode ,int& zoom, int& ocvwin){
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) ocvwin = 1- ocvwin;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) mode = E_CAM_MODE::NORMAL;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) mode = E_CAM_MODE::GRAY;
//	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) mode = E_CAM_MODE::GAUSS;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) mode = E_CAM_MODE::BILATERAL;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) mode = E_CAM_MODE::SOBEL_H;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) mode = E_CAM_MODE::SOBEL_V;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) mode = E_CAM_MODE::LAPLACIAN;
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) mode = E_CAM_MODE::THRESH;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) mode = E_CAM_MODE::ADAPTIVE_THRESH;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) mode = E_CAM_MODE::FFT;
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) mode = E_CAM_MODE::FFT_RECT;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) mode = E_CAM_MODE::CANNY;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) mode = E_CAM_MODE::EDGE;


	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS){
		zoom++;
		if (zoom >= E_CAM_ZOOM::ZOOMxMAX) zoom--;
	}
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS){
		zoom--;
		if (zoom <= E_CAM_ZOOM::ZOOMxMIN) zoom++;
	}

}



int _tmain(int argc, _TCHAR* argv[])
{
	const GLsizei captureWidth(640);
	const GLsizei captureHeight(480);
//	const GLsizei captureWidth(1280);
//	const GLsizei captureHeight(720);


	GLFWwindow* window = glsCvInit(captureWidth, captureHeight);
	
	
	// OpenCV Ç…ÇÊÇÈÉrÉfÉIÉLÉÉÉvÉ`ÉÉÇèâä˙âªÇ∑ÇÈ
	cv::VideoCapture camera(CV_CAP_ANY);
	if (!camera.isOpened())
	{
		std::cerr << "cannot open input" << std::endl;
		exit(-1);
	}

	// ÉJÉÅÉâÇÃèâä˙ê›íË
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

	int camMode = E_CAM_MODE::CANNY;
	int camZoom = E_CAM_ZOOM::ZOOMx1000;
	int ocvwin = 0;

	do{
		cv::Mat frame;
		camera >> frame;						// ÉLÉÉÉvÉ`ÉÉâfëúÇ©ÇÁâÊëúÇêÿÇËèoÇ∑
		if (ocvwin)	cv::imshow("[OCV]", frame.clone());
		else cv::destroyWindow("[OCV]");

//		cv::flip(frame, frame, 0);				// è„â∫îΩì]

		GlsMat glsFrame;

		switch (camMode){
		case(E_CAM_MODE::FFT) :	{
			Mat roi = Mat(frame, rectFft);
			glsFrame = (GlsMat)roi;
			gls::flip(glsFrame, glsFrame, 0);				// è„â∫îΩì]
			gls::convert(glsFrame, glsFrame, 1.0f / 256.0f);
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			gls::multiply(glsFftWin, glsFrame, glsFrame);
			vector<GlsMat> plnGls(2);
			plnGls[0] = glsFrame;
			plnGls[1] = glsZero;
			GlsMat glsComplx;
			gls::merge(plnGls, glsComplx);
			gls::fft(glsComplx, glsComplx, GLS_FFT_SHIFT);
			gls::logMagSpectrums(glsComplx, glsFrame, 1.0);
			gls::normalize(glsFrame, glsFrame, 0, 1, NORM_MINMAX);
		}break;
		case(E_CAM_MODE::ADAPTIVE_THRESH) : {
			glsFrame = (GlsMat)frame;
			gls::flip(glsFrame, glsFrame, 0);				// è„â∫îΩì]
			gls::convert(glsFrame, glsFrame, 1.0f / 256.0f);
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			gls::adaptiveThreshold(glsFrame, glsFrame, 1.0, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY,15,0.01);
		}break;
		case(E_CAM_MODE::THRESH) : {
			glsFrame = (GlsMat)frame;
			gls::flip(glsFrame, glsFrame, 0);				// è„â∫îΩì]
			gls::convert(glsFrame, glsFrame, 1.0f / 256.0f);
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			Scalar mean = gls::mean(glsFrame);
			gls::threshold(glsFrame, glsFrame, mean[0], 1.0, THRESH_BINARY);
		}break;
		case(E_CAM_MODE::FFT_RECT) : {
			Mat roi = Mat(frame, rectFft);
			glsFrame = (GlsMat)roi;
			gls::flip(glsFrame, glsFrame, 0);				// è„â∫îΩì]
			gls::convert(glsFrame, glsFrame, 1.0f / 256.0f);
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			gls::multiply(glsFftWin, glsFrame, glsFrame);
		} break;
		case(E_CAM_MODE::SOBEL_V) : {
			glsFrame = (GlsMat)frame;
			gls::flip(glsFrame, glsFrame, 0);				// è„â∫îΩì]
			gls::convert(glsFrame, glsFrame, 1.0f / 256.0f);
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			gls::Sobel(glsFrame, glsFrame, -1, 0, 1, 3, 1.0, 0.5);
		}break;
		case(E_CAM_MODE::SOBEL_H) : {
			glsFrame = (GlsMat)frame;
			gls::flip(glsFrame, glsFrame, 0);				// è„â∫îΩì]
			gls::convert(glsFrame, glsFrame, 1.0f / 256.0f);
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			gls::Sobel(glsFrame, glsFrame, -1, 1, 0, 3, 1.0, 0.5);
		}break;
		case(E_CAM_MODE::EDGE) : {
			glsFrame = (GlsMat)frame;
			gls::flip(glsFrame, glsFrame, 0);				// è„â∫îΩì]
			gls::convert(glsFrame, glsFrame, 1.0f / 256.0f);
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
			glsFrame = (GlsMat)frame;
			gls::flip(glsFrame, glsFrame, 0);				// è„â∫îΩì]
			gls::convert(glsFrame, glsFrame, 1.0f / 256.0f);
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
//			gls::Laplacian(glsFrame, glsFrame, -1, 1, 1.0, 0.5);
			gls::Laplacian(glsFrame, glsFrame, -1, 3, 1.0, 0.5);
		}break;
		case(E_CAM_MODE::GAUSS) : {
			glsFrame = (GlsMat)frame;
			gls::flip(glsFrame, glsFrame, 0);				// è„â∫îΩì]
			gls::convert(glsFrame, glsFrame, 1.0f / 256.0f);
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2RGB);
			gls::GaussianBlur(glsFrame, glsFrame, Size(9, 9), 0, 0);
		}break;
		case(E_CAM_MODE::BILATERAL) : {
			glsFrame = (GlsMat)frame;
			gls::flip(glsFrame, glsFrame, 0);				// è„â∫îΩì]
			gls::convert(glsFrame, glsFrame, 1.0f / 256.0f);
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2RGB);
			//! tuning param
			const int ksize = 9;				//trade off IQ vs process time
			const double sigmaColor = 0.07;		//measurement noise sigma / trade off image detail vs noise
			const double sigmaSpace = 0.3*(ksize / 2 - 1) + 0.8;
			gls::bilateralFilter(glsFrame, glsFrame, ksize, sigmaColor, sigmaSpace);
		}break;
		case(E_CAM_MODE::CANNY) : {
			glsFrame = (GlsMat)frame;
			gls::flip(glsFrame, glsFrame, 0);				// è„â∫îΩì]
			gls::convert(glsFrame, glsFrame, 1.0f / 256.0f);
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			const double high = 0.20;
			const double low = high /2.0;
			gls::Canny(glsFrame, glsFrame, high,low,3,true);
		}break;
		case(E_CAM_MODE::GRAY) : {
			glsFrame = (GlsMat)frame;
			gls::flip(glsFrame, glsFrame, 0);				// è„â∫îΩì]
			gls::convert(glsFrame, glsFrame, 1.0f / 256.0f);
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
		}break;
		case(E_CAM_MODE::NORMAL):
		default:{
			glsFrame = (GlsMat)frame;
			gls::flip(glsFrame, glsFrame, 0);				// è„â∫îΩì]
			gls::convert(glsFrame, glsFrame, 1.0f / 256.0f);
			gls::cvtColor(glsFrame, glsFrame, CV_BGR2RGB);
		}break;
		}

		switch (camZoom){
		case(E_CAM_ZOOM::ZOOMx1000) : {
		}break;
		case(E_CAM_ZOOM::ZOOMx1414) : {
			gls::resize(glsFrame, glsFrame, Size(0, 0), sqrt(2.0), sqrt(2.0));
		}break;
		case(E_CAM_ZOOM::ZOOMx2000) : {
			gls::resize(glsFrame, glsFrame, Size(0, 0), 2.0, 2.0);
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
		controls(window, camMode, camZoom, ocvwin); // key check
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);


	glsCvTerminate();

	return 0;
}

