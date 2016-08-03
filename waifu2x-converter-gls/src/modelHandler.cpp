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
 * modelHandler.cpp
 *   (ここにファイルの簡易説明を記入)
 *
 *  Created on: 2015/05/24
 *      Author: wlamigo
 *		changed : oasi-adamay
 *
 *   (ここにファイルの説明を記入)
 */

#include "modelHandler.hpp"
#include "convolutionalNeuralNetwork.hpp"

// #include <iostream> in modelHandler.hpp
#include <fstream>

#include "glsCV.h"


namespace w2xc {

int Model::getNInputPlanes() {
	return nInputPlanes;
}

int Model::getNOutputPlanes() {
	return nOutputPlanes;
}

#ifdef USE_GLS
bool Model::filter(
	std::vector<gls::GlsMat> &inputPlanes,
	std::vector<gls::GlsMat> &outputPlanes
)
{

	if (inputPlanes.size() != nInputPlanes) {
		std::cerr << "Error : Model-filter : \n"
				"number of input planes mismatch." << std::endl;
		std::cerr << inputPlanes.size() << ","
				<< nInputPlanes << std::endl;
		return false;
	}

	outputPlanes.resize(nOutputPlanes);

	cv::Size ipSize = inputPlanes[0].size();

	// filter processing
	// input : inputPlanes
	// kernel : weightMatrices
	for (int opIndex = 0; opIndex < nOutputPlanes ; opIndex++) {

		int wMatIndex = nInputPlanes * opIndex;
		gls::GlsMat uIntermediatePlane = (GlsMat)cv::Mat::zeros(ipSize, CV_32FC1); // all zero matrix;

		for (int ipIndex = 0; ipIndex < nInputPlanes; ipIndex++) {

			gls::GlsMat filterOutput;
			gls::filter2D(inputPlanes[ipIndex], filterOutput, -1, weights[wMatIndex + ipIndex],
				cv::Point(-1, -1), 0.0 /*, cv::BORDER_REPLICATE*/);
			gls::add(uIntermediatePlane, filterOutput, uIntermediatePlane);

		}

		gls::add(biases[opIndex], uIntermediatePlane, uIntermediatePlane);
		gls::GlsMat moreThanZero;
		gls::GlsMat lessThanZero;
		gls::max(0.0, uIntermediatePlane, moreThanZero);
		gls::min(0.0, uIntermediatePlane, lessThanZero);
		gls::multiply(0.1, lessThanZero, lessThanZero);
		gls::add(lessThanZero, moreThanZero, uIntermediatePlane);
		outputPlanes[opIndex] = uIntermediatePlane;

	} // for index

	return true;



//	filterWorker(inputPlanes, weights, outputPlanes, 0, nOutputPlanes);

	return true;
}
#elif	defined(USE_GLS_NEW)
bool Model::filter(
	gls::GlsMat &inputPlanes,
	gls::GlsMat &outputPlanes)
{

	CV_Assert(inputPlanes.dims == 3);

	if (inputPlanes.size[0] != nInputPlanes) {
		std::cerr << "Error : Model-filter : \n"
			"number of input planes mismatch." << std::endl;
		std::cerr << inputPlanes.size() << ","
			<< nInputPlanes << std::endl;
		return false;
	}

	int _size[3] = { nOutputPlanes, inputPlanes.size[1], inputPlanes.size[2] };
	outputPlanes = gls::GlsMat(3, _size, CV_32FC1);

	gls::convolutionalNeuralNetwork(inputPlanes, outputPlanes, weights, biases);


	return true;
}
#else
bool Model::filter(	cv::Mat &inputPlanes, cv::Mat &outputPlanes) {

	CV_Assert(inputPlanes.dims == 3);

	if (inputPlanes.size[0] != nInputPlanes) {
		std::cerr << "Error : Model-filter : \n"
			"number of input planes mismatch." << std::endl;
		std::cerr << inputPlanes.size() << ","
			<< nInputPlanes << std::endl;
		return false;
	}

	int _size[3] = { nOutputPlanes, inputPlanes.size[1], inputPlanes.size[2] };
	outputPlanes = cv::Mat(3, _size, CV_32FC1);

	CNN::convolutionalNeuralNetwork(inputPlanes, outputPlanes, weights, biases);


	return true;
}

#endif

bool Model::loadModelFromJSONObject(picojson::object &jsonObj) {

	// nInputPlanes,nOutputPlanes,kernelSize have already set.

	int matProgress = 0;
	picojson::array &wOutputPlane = jsonObj["weight"].get<picojson::array>();

	// setting weight matrices
	for (auto&& wInputPlaneV : wOutputPlane) {
		picojson::array &wInputPlane = wInputPlaneV.get<picojson::array>();

		for (auto&& weightMatV : wInputPlane) {
			picojson::array &weightMat = weightMatV.get<picojson::array>();
			cv::Mat writeMatrix = cv::Mat(kernelSize, kernelSize,
				CV_32FC1, weights.ptr<float>(matProgress));

			for (int writingRow = 0; writingRow < kernelSize; writingRow++) {
				auto& weightMatRowV = weightMat.at(writingRow);
				picojson::array &weightMatRow = weightMatRowV.get<
						picojson::array>();

				for (int index = 0; index < kernelSize; index++) {
					writeMatrix.at<float>(writingRow, index) =
							(float)weightMatRow[index].get<double>();
				} // for(weightMatRow) (writing 1 row finished)

			} // for(weightMat) (writing 1 matrix finished)


			matProgress++;
		} // for(wInputPlane) (writing matrices in set of wInputPlane finished)

	} //for(wOutputPlane) (writing all matrices finished)

	// setting biases
	picojson::array biasesData = jsonObj["bias"].get<picojson::array>();
	for (int index = 0; index < nOutputPlanes; index++) {
		biases[index] = biasesData[index].get<double>();
	}

	return true;
}


Model::Model(std::istream& binFile) {
	// preload nInputPlanes,nOutputPlanes, and preserve required size vector

	binFile.read((char*)&nInputPlanes, sizeof(int));
	binFile.read((char*)&nOutputPlanes, sizeof(int));
	binFile.read((char*)&kernelSize, sizeof(int));

	int _size[3] = { nInputPlanes * nOutputPlanes, kernelSize, kernelSize };
	weights = Mat(3, _size, CV_32FC1);
	biases = std::vector<double>(nOutputPlanes, 0.0);

	if (!loadModelFromBin(binFile)) {
		std::cerr << "Error : Model-Constructor : \n"
			"something error has been occured in loading model from Binary File.\n"
			"stop."
			<< std::endl;
		std::exit(-1);
	}

}


bool Model::loadModelFromBin(std::istream& binFile)
{
	// nInputPlanes,nOutputPlanes,kernelSize have already set.
	int matProgress = 0;
	for (int i = 0; i < nOutputPlanes; i++) {
		for (int j = 0; j < nInputPlanes; j++) {
			cv::Mat writeMatrix = cv::Mat(kernelSize, kernelSize,
				CV_32FC1, weights.ptr<float>(matProgress));

			for (int writingRow = 0; writingRow < kernelSize; writingRow++) {
				for (int index = 0; index < kernelSize; index++) {
					float data;
					binFile.read((char*)&data, sizeof(float));
					writeMatrix.at<float>(writingRow, index) = data;
				}
			}
			matProgress++;
		}
	}

	// setting biases
	biases.resize(nOutputPlanes);
	binFile.read((char*)&biases[0], biases.size() * sizeof(double));

	return true;
}

bool Model::saveModelToBin(std::ostream& binFile)
{
	binFile.write((char*)&nInputPlanes, sizeof(int));
	binFile.write((char*)&nOutputPlanes, sizeof(int));
	binFile.write((char*)&kernelSize, sizeof(int));

	int matProgress = 0;
	for (int i = 0; i < nOutputPlanes; i++) {
		for (int j = 0; j < nInputPlanes; j++) {
			const cv::Mat writeMatrix = cv::Mat(kernelSize, kernelSize,
				CV_32FC1, weights.ptr<float>(matProgress));

			for (int writingRow = 0; writingRow < kernelSize; writingRow++) {
				for (int index = 0; index < kernelSize; index++) {
					float data = writeMatrix.at<float>(writingRow, index);
					binFile.write((char*)&data, sizeof(float));
				}
			}

			matProgress++;
		}
	}

	// setting biases
	binFile.write((char*)&biases[0], biases.size() * sizeof(double));

	return true;
}


modelUtility * modelUtility::instance = nullptr;

modelUtility& modelUtility::getInstance(){
	if(instance == nullptr){
		instance = new modelUtility();
	}
	return *instance;
}

bool modelUtility::generateModelFromJSON(const std::string &fileName,
		std::vector<std::unique_ptr<Model> > &models) {

	std::ifstream jsonFile;

	jsonFile.open(fileName);
	if (!jsonFile.is_open()) {
		std::cerr << "Error : couldn't open " << fileName << std::endl;
		return false;
	}

	picojson::value jsonValue;
	jsonFile >> jsonValue;
	std::string errMsg = picojson::get_last_error();
	if (!errMsg.empty()) {
		std::cerr << "Error : PicoJSON Error : " << errMsg << std::endl;
		return false;
	}

	picojson::array& objectArray = jsonValue.get<picojson::array>();
	for (auto&& obj : objectArray) {
		std::unique_ptr<Model> m = std::unique_ptr<Model>(
				new Model(obj.get<picojson::object>()));
		models.push_back(std::move(m));
	}

	return true;
}

bool modelUtility::generateModelFromBin(const std::string &fileName,
	std::vector<std::unique_ptr<Model> > &models) {

	std::ifstream binFile;

	binFile.open(fileName, std::ios::binary);
	if (!binFile.is_open()) {
		std::cerr << "Error : couldn't open " << fileName << std::endl;
		return false;
	}

	int32_t modelCount;
	binFile.read((char*)&modelCount, sizeof(modelCount));
	for (int i = 0; i < modelCount; i++) {
		models.emplace_back(new Model(binFile));
	}

	return true;
}

bool modelUtility::saveModelToBin(const std::string &fileName,
	std::vector<std::unique_ptr<Model> > &models) {

	std::ofstream binFile;

	binFile.open(fileName, std::ios::binary);
	if (!binFile.is_open()) {
		std::cerr << "Error : couldn't open " << fileName << std::endl;
		return false;
	}

	int32_t modelCount = (int32_t)models.size();
	binFile.write((char*)&modelCount, sizeof(modelCount));
	for (auto& model : models) {
		model->saveModelToBin(binFile);
	}

	return true;
}


bool modelUtility::setNumberOfJobs(int setNJob){
	if(setNJob < 1)return false;
	nJob = setNJob;
	return true;
};

int modelUtility::getNumberOfJobs(){
	return nJob;
}

bool modelUtility::setBlockSize(cv::Size size){
	if(size.width < 0 || size.height < 0)return false;
	blockSplittingSize = size;
	return true;
}

bool modelUtility::setBlockSizeExp2Square(int exp){
	if(exp < 0)return false;
	int length = (int)std::pow(2, exp);
	blockSplittingSize = cv::Size(length, length);
	return true;
}

cv::Size modelUtility::getBlockSize(){
	return blockSplittingSize;
}


// for debugging

void Model::printWeightMatrix() {
	for (int i = 0; i < weights.size[0]; i++) {
		cv::Mat weightMatrix(weights.size[1], weights.size[2], weights.type(), weights.data + weights.step[0]);
		std::cout << weightMatrix << std::endl;
	}
}

void Model::printBiases() {
	for (auto&& bias : biases) {
		std::cout << bias << std::endl;
	}
}

}
