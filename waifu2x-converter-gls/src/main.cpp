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

/*
 * main.cpp
 *   (ここにファイルの簡易説明を記入)
 *
 *  Created on: 2015/05/24
 *      Author: wlamigo
 *		changed : oasi-adamay
 *
 *   (ここにファイルの説明を記入)
 */


#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>
#include "picojson.h"
#include "tclap/CmdLine.h"

#include "modelHandler.hpp"
#include "convertRoutine.hpp"

#include "glsCV.h"
//Lib 
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glsCV.lib")

//#ifdef _DEBUG
#if 1
#include "Timer.h"
#define _TMR_(...)  Timer tmr(__VA_ARGS__)
#else
#define _TMR_(...)
#endif


#define MODEL_BIN

int main(int argc, char** argv) {

	glsCvInit();


	// definition of command line arguments
	TCLAP::CmdLine cmd("waifu2x reimplementation using OpenCV", ' ', "1.0.0");

	TCLAP::ValueArg<std::string> cmdInputFile("i", "input_file",
			"path to input image file (you should input full path)", true, "",
			"string", cmd);

	TCLAP::ValueArg<std::string> cmdOutputFile("o", "output_file",
			"path to output image file (you should input full path)", false,
			"(auto)", "string", cmd);

	std::vector<std::string> cmdModeConstraintV;
	cmdModeConstraintV.push_back("noise");
	cmdModeConstraintV.push_back("scale");
	cmdModeConstraintV.push_back("noise_scale");
	TCLAP::ValuesConstraint<std::string> cmdModeConstraint(cmdModeConstraintV);
	TCLAP::ValueArg<std::string> cmdMode("m", "mode", "image processing mode",
			false, "noise_scale", &cmdModeConstraint, cmd);

	std::vector<int> cmdNRLConstraintV;
	cmdNRLConstraintV.push_back(1);
	cmdNRLConstraintV.push_back(2);
	TCLAP::ValuesConstraint<int> cmdNRLConstraint(cmdNRLConstraintV);
	TCLAP::ValueArg<int> cmdNRLevel("", "noise_level", "noise reduction level",
			false, 1, &cmdNRLConstraint, cmd);

	TCLAP::ValueArg<double> cmdScaleRatio("", "scale_ratio",
			"custom scale ratio", false, 2.0, "double", cmd);

	TCLAP::ValueArg<std::string> cmdModelPath("", "model_dir",
			"path to custom model directory (don't append last / )", false,
			"models", "string", cmd);

	TCLAP::ValueArg<int> cmdNumberOfJobs("j", "jobs",
			"number of threads launching at the same time", false, 1, "integer",
			cmd);

	// definition of command line argument : end

	// parse command line arguments
	try {
		cmd.parse(argc, argv);
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "Error : cmd.parse() threw exception" << std::endl;
		std::exit(-1);
	}

	// load image file
	std::cout << "Load image file:" << cmdInputFile.getValue() << std::endl;
	cv::Mat image = cv::imread(cmdInputFile.getValue(), cv::IMREAD_COLOR);
	if (image.empty()){
		std::cerr << "Error : cannot open image " << cmdInputFile.getValue() << std::endl;
		std::exit(-1);
	}

	cv::Mat srcImg = image.clone();

	image.convertTo(image, CV_32F, 1.0 / 255.0);
	cv::cvtColor(image, image, cv::COLOR_RGB2YUV);

	// set number of jobs for processing models
	w2xc::modelUtility::getInstance().setNumberOfJobs(cmdNumberOfJobs.getValue());

	string strModelExt;
#if !defined(MODEL_BIN)
	strModelExt = ".json";
#else
	strModelExt = ".bin";
#endif
	 
	// ===== Noise Reduction Phase =====
	if (cmdMode.getValue() == "noise" || cmdMode.getValue() == "noise_scale") {
		_TMR_("Noise Reduction Phase:");
		std::cout << "Noise Reduction Phase." << std::endl;

		std::string modelFileName(cmdModelPath.getValue());
		modelFileName = modelFileName + "/noise"
			+ std::to_string(cmdNRLevel.getValue()) + "_model" + strModelExt;
		std::vector<std::unique_ptr<w2xc::Model> > models;

		std::cout << "Load model file:" << modelFileName << std::endl;
#if !defined(MODEL_BIN)
		if (!w2xc::modelUtility::generateModelFromJSON(modelFileName, models))
#else
		if (!w2xc::modelUtility::generateModelFromBin(modelFileName, models))
#endif
			std::exit(-1);

		std::vector<cv::Mat> imageSplit;
		cv::Mat imageY;
		cv::split(image, imageSplit);
		imageSplit[0].copyTo(imageY);

		w2xc::convertWithModels(imageY, imageSplit[0], models);

		cv::merge(imageSplit, image);
		
	} // noise reduction phase : end

	// ===== scaling phase =====

	if (cmdMode.getValue() == "scale" || cmdMode.getValue() == "noise_scale") {
		_TMR_("scaling Phase:");
		std::cout << "scaling phase." << std::endl;

		// calculate iteration times of 2x scaling and shrink ratio which will use at last
		int iterTimesTwiceScaling = static_cast<int>(std::ceil(
				std::log2(cmdScaleRatio.getValue())));
		double shrinkRatio = 0.0;
		if (static_cast<int>(cmdScaleRatio.getValue())
				!= std::pow(2, iterTimesTwiceScaling)) {
			shrinkRatio = cmdScaleRatio.getValue()
					/ std::pow(2.0, static_cast<double>(iterTimesTwiceScaling));
		}

		std::string modelFileName(cmdModelPath.getValue());
		modelFileName = modelFileName + "/scale2.0x_model" +strModelExt;

		std::vector<std::unique_ptr<w2xc::Model> > models;

		std::cout << "Load model file:" << modelFileName << std::endl;
#if !defined(MODEL_BIN)
		if (!w2xc::modelUtility::generateModelFromJSON(modelFileName, models))
#else
		if (!w2xc::modelUtility::generateModelFromBin(modelFileName, models))
#endif
			std::exit(-1);

		std::cout << "start scaling" << std::endl;
		// 2x scaling
		for (int nIteration = 0; nIteration < iterTimesTwiceScaling;
				nIteration++) {

			std::cout << "#" << std::to_string(nIteration + 1)
					<< " 2x scaling..." << std::endl;

			cv::Size imageSize = image.size();
			imageSize.width *= 2;
			imageSize.height *= 2;
			cv::Mat image2xNearest;
			cv::resize(image, image2xNearest, imageSize, 0, 0, cv::INTER_NEAREST);
			std::vector<cv::Mat> imageSplit;
			cv::Mat imageY;
			cv::split(image2xNearest, imageSplit);
			imageSplit[0].copyTo(imageY);

			// generate bicubic scaled image and split
			imageSplit.clear();
			cv::Mat image2xBicubic;
			cv::resize(image,image2xBicubic,imageSize,0,0,cv::INTER_CUBIC);
			cv::split(image2xBicubic, imageSplit);


			if(!w2xc::convertWithModels(imageY, imageSplit[0], models)){
				std::cerr << "w2xc::convertWithModels : something error has occured.\n"
						"stop." << std::endl;
				std::exit(1);
			};

			cv::merge(imageSplit, image);

		} // 2x scaling : end

		if (shrinkRatio != 0.0) {
			cv::Size lastImageSize = image.size();
			lastImageSize.width =
					static_cast<int>(static_cast<double>(lastImageSize.width
							* shrinkRatio));
			lastImageSize.height =
					static_cast<int>(static_cast<double>(lastImageSize.height
							* shrinkRatio));
			cv::resize(image, image, lastImageSize, 0, 0, cv::INTER_LINEAR);
		}

	}

	cv::cvtColor(image, image, cv::COLOR_YUV2RGB);
	image.convertTo(image, CV_8U, 255.0);
	std::string outputFileName = cmdOutputFile.getValue();
	if (outputFileName == "(auto)") {
		outputFileName = cmdInputFile.getValue();
		int tailDot = (int)outputFileName.find_last_of('.');
		outputFileName.erase(tailDot, outputFileName.length());
		outputFileName = outputFileName + "(" + cmdMode.getValue() + ")";
		std::string &mode = cmdMode.getValue();
		if(mode.find("noise") != mode.npos){
			outputFileName = outputFileName + "(Level" + std::to_string(cmdNRLevel.getValue())
			+ ")";
		}
		if(mode.find("scale") != mode.npos){
			outputFileName = outputFileName + "(x" + std::to_string(cmdScaleRatio.getValue())
			+ ")";
		}
		outputFileName += ".png";
	}
	std::cout << "Save image file:" << outputFileName << std::endl;
	cv::imwrite(outputFileName, image);

	std::cout << "process successfully done!" << std::endl;

	cv::Mat dstImg = image.clone();

	cv::Mat cubicImg;
	cv::resize(srcImg, cubicImg, Size(0,0), 2.0, 2.0, cv::INTER_CUBIC);

#if 0
	cv::imshow("src",srcImg);
	cv::imshow("dst", dstImg);
	cv::imshow("cubic", cubicImg);
#endif

	glsCvTerminate();

	std::cin.get();
	return 0;
}
