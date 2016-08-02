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
 * modelHandler.hpp
 *   (ここにファイルの簡易説明を記入)
 *
 *  Created on: 2015/05/24
 *      Author: wlamigo
 *		changed : oasi-adamay
 *
 *   (ここにファイルの説明を記入)
 */

#ifndef MODEL_HANDLER_HPP_
#define MODEL_HANDLER_HPP_

#include <opencv2/opencv.hpp>
#include <opencv2/ocl/ocl.hpp>
#include "picojson.h"
#include <iostream>
#include <memory>
#include <cstdint>
#include <cstdlib>

#include "glsCV.h"

//#define USE_GLS
#define USE_GLS_NEW

namespace w2xc {


class Model {

private:
	int nInputPlanes;
	int nOutputPlanes;
	cv::Mat weights;
	std::vector<double> biases;
	int kernelSize;

	Model() {
	}
	; // cannot use no-argument constructor

	// class inside operation function
	bool loadModelFromJSONObject(picojson::object& jsonObj);
	bool loadModelFromBin(std::istream& binFile);



public:
	// ctor and dtor
	Model(picojson::object &jsonObj) {
		// preload nInputPlanes,nOutputPlanes, and preserve required size vector
		nInputPlanes = static_cast<int>(jsonObj["nInputPlane"].get<double>());
		nOutputPlanes = static_cast<int>(jsonObj["nOutputPlane"].get<double>());
		if ((kernelSize = static_cast<int>(jsonObj["kW"].get<double>()))
				!= static_cast<int>(jsonObj["kH"].get<double>())) {
			std::cerr << "Error : Model-Constructor : \n"
					"kernel in model is not square.\n"
					"stop." << std::endl;
			std::exit(-1);
		} // kH == kW

		int _size[3] = { nInputPlanes * nOutputPlanes, kernelSize, kernelSize };
		weights = Mat(3, _size, CV_32FC1);
		biases = std::vector<double>(nOutputPlanes, 0.0);

		if (!loadModelFromJSONObject(jsonObj)) {
			std::cerr
					<< "Error : Model-Constructor : \n"
							"something error has been occured in loading model from JSON-Object.\n"
							"stop." << std::endl;
			std::exit(-1);
		}
	}
	Model(std::istream& binFile);

	;
	~Model() {
	}

	// for debugging
	void printWeightMatrix();
	void printBiases();

	// getter function
	int getNInputPlanes();
	int getNOutputPlanes();

	// setter function

	// public operation function
#ifdef USE_GLS
	bool Model::filter(
		std::vector<gls::GlsMat> &inputPlanes,
		std::vector<gls::GlsMat> &outputPlanes
		);
#elif	defined(USE_GLS_NEW)
	bool Model::filter(
		gls::GlsMat &inputPlanes,
		gls::GlsMat &outputPlanes);
#else
	bool Model::filter(
		cv::Mat &inputPlanes,
		cv::Mat &outputPlanes);
#endif

	bool saveModelToBin(std::ostream& binFile);


};

class modelUtility {

private:
	static modelUtility* instance;
	int nJob;
	cv::Size blockSplittingSize;
	modelUtility() :
			nJob(4), blockSplittingSize(512,512) {
	}
	;

public:
	static bool generateModelFromJSON(const std::string &fileName,
			std::vector<std::unique_ptr<Model> > &models);
	static bool generateModelFromBin(const std::string &fileName,
		std::vector<std::unique_ptr<Model> > &models);
	static bool saveModelToBin(const std::string &fileName,
		std::vector<std::unique_ptr<Model> > &models);

	static modelUtility& getInstance();
	bool setNumberOfJobs(int setNJob);
	int getNumberOfJobs();
	bool setBlockSize(cv::Size size);
	bool setBlockSizeExp2Square(int exp);
	cv::Size getBlockSize();

};

}

#endif /* MODEL_HANDLER_HPP_ */
