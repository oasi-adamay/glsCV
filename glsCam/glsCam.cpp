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
	GRAY
	
};

void controls(GLFWwindow* window, int& mode){

	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) mode = E_CAM_MODE::NORMAL;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) mode = E_CAM_MODE::GRAY;
}



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

#if 0
	{
		cv::Mat frame;
		camera >> frame;						// キャプチャ映像から画像を切り出す

		glsMat glsFrame0(frame);
		glsMat glsFrame1(glsFrame0);
		glsMat glsFrame2 = glsFrame0;
		glsMat glsFrame3 = glsFrame0;
	}
#endif

	int camMode = E_CAM_MODE::NORMAL;
	do{
		cv::Mat frame;
		camera >> frame;						// キャプチャ映像から画像を切り出す
		imshow("[OCV]", frame.clone());

		cv::flip(frame, frame, 0);				// 上下反転

		glsMat glsFrame(frame);
		glsConvert(glsFrame, glsFrame, 1.0f / 256.0f);
		switch (camMode){
		case(E_CAM_MODE::GRAY) :
			glsCvtColor(glsFrame, glsFrame, CV_BGR2GRAY);
			break;
		case(E_CAM_MODE::NORMAL):
		default:
			glsCvtColor(glsFrame, glsFrame, CV_BGR2RGB);
			break;
		}


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

