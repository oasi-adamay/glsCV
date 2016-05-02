// glsCam.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
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
	
	
	// OpenCV �ɂ��r�f�I�L���v�`��������������
	cv::VideoCapture camera(CV_CAP_ANY);
	if (!camera.isOpened())
	{
		std::cerr << "cannot open input" << std::endl;
		exit(-1);
	}

	// �J�����̏����ݒ�
	camera.grab();
	const GLsizei capture_width(frameWidth);
	const GLsizei capture_height(frameHeight);
	camera.set(CV_CAP_PROP_FRAME_WIDTH, double(capture_width));
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, double(capture_height));

#if 0
	{
		cv::Mat frame;
		camera >> frame;						// �L���v�`���f������摜��؂�o��

		glsMat glsFrame0(frame);
		glsMat glsFrame1(glsFrame0);
		glsMat glsFrame2 = glsFrame0;
		glsMat glsFrame3 = glsFrame0;
	}
#endif

	do{
		cv::Mat frame;
		camera >> frame;						// �L���v�`���f������摜��؂�o��
		imshow("[OCV]", frame.clone());

		cv::flip(frame, frame, 0);				// �㉺���]
//		cvtColor(frame, frame, CV_BGR2RGB);		//channel order

		glsMat glsFrame(frame);
		glsConvert(glsFrame, glsFrame, 1.0f / 256.0f);
		glsCvtColor(glsFrame, glsFrame, CV_BGR2RGB);

		glsDraw(glsFrame);

		glfwSwapBuffers(window);  // Swap buffers
		glfwPollEvents();
	}
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);


	glsCvTerminate();

	return 0;
}

