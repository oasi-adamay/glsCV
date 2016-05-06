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
};

void controls(GLFWwindow* window, int& mode){

	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) mode = E_CAM_MODE::NORMAL;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) mode = E_CAM_MODE::GRAY;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) mode = E_CAM_MODE::FFT;
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


	int camMode = E_CAM_MODE::FFT;
	do{
		cv::Mat frame;
		camera >> frame;						// キャプチャ映像から画像を切り出す
		imshow("[OCV]", frame.clone());

		cv::flip(frame, frame, 0);				// 上下反転

		glsMat glsFrame(frame);
		glsConvert(glsFrame, glsFrame, 1.0f / 256.0f);
		switch (camMode){
		case(E_CAM_MODE::FFT) :
			{

				int x = (glsFrame.size().width - sizeFft.width) / 2;
				int y = (glsFrame.size().height - sizeFft.height) / 2;
				Rect rect( x, y, sizeFft.width, sizeFft.height);
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
#elif 0
				glsMat glsZero(sizeFft, CV_32FC1,Size(2,2));
				glsCopyRect(glsFrame, glsFrame, rect , Size(2,2));
				glsCvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
				glsMultiply(glsFftWin, glsFrame, glsFrame);
				vector<glsMat> plnGls(2);
				plnGls[0] = glsFrame;
				plnGls[1] = glsZero;
				glsMat glsComplx;
				glsMerge(plnGls, glsComplx);
				glsFft(glsComplx, glsComplx, GLS_FFT_SHIFT);
				glsMagSpectrums(glsComplx, glsFrame);
				glsAdd(vec4(1.0), glsFrame, glsFrame);
				glsLog(glsFrame, glsFrame);
				double min, max;
				glsMinMaxLoc(glsFrame, &min, &max);
				glsAdd(vec4((float)-min), glsFrame, glsFrame);
				glsMultiply(vec4(1.0f / (float)(max - min)), glsFrame, glsFrame);
#else
				glsMat glsZero(sizeFft, CV_32FC1, Size(2, 2));
				glsCopyRect(glsFrame, glsFrame, rect, Size(2, 2));
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
			}
			break;
		case(E_CAM_MODE::GRAY) :
			glsCvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			break;
		case(E_CAM_MODE::NORMAL):
		default:
			glsCvtColor(glsFrame, glsFrame, CV_BGR2RGB);
			break;
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

