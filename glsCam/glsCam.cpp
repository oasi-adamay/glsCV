// glsCam.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include "glsCV.h"



//Lib 
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "glfw3dll.lib")

#pragma comment (lib, "glsCV.lib")


int _tmain(int argc, _TCHAR* argv[])
{
	const int frameWidth = 640;
	const int frameHeight = 480;
	GLFWwindow* window = glsCvInit(frameWidth, frameHeight);
	
	
	// OpenCV によるビデオキャプチャを初期化する
	cv::VideoCapture camera(CV_CAP_ANY);
	if (!camera.isOpened())
	{
		std::cerr << "cannot open input" << std::endl;
		exit(-1);
	}

	// カメラの初期設定
	camera.grab();
	const GLsizei capture_width(frameWidth);
	const GLsizei capture_height(frameHeight);
	camera.set(CV_CAP_PROP_FRAME_WIDTH, double(capture_width));
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, double(capture_height));

	do{
		cv::Mat frame;
		camera >> frame;						// キャプチャ映像から画像を切り出す
		imshow("[OCV]", frame.clone());

		cv::flip(frame, frame, 0);				// 上下反転
//		cvtColor(frame, frame, CV_BGR2RGB);		//channel order
		cvtColor(frame, frame, CV_BGR2GRAY);	//to gray

		glsMat glsFrame(frame);
//		glsMat glsFrameFlt(glsFrame.width, glsFrame.height,GL_RGB32F);
//		glsConvert(glsFrame, glsFrameFlt, 1.0f / 256.0f);

		glsDraw(glsFrame);
//		glsDraw(glsFrameFlt);

		glfwSwapBuffers(window);  // Swap buffers
		glfwPollEvents();
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);


	glsCvTerminate();

	return 0;
}

