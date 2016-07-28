// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"

// TODO: プログラムに必要な追加ヘッダーをここで参照してください。

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <memory>
using namespace std;

//---------------------
// OpenCV
#include <opencv2/opencv.hpp>
using namespace cv;

//---------------------
// GL
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
