# glsCV
OpenGLSLを使用したCVライブラリ  

## LICENSE
BSD-3clause license  

## 概要
- OpenGL/OpenGLSLで記述した、OpenCV　Likeなライブラリ
- cv::gpu(CUDA)と同じように、I/FはOpenCV互換
- NVIDIA非搭載のNotePCでも実行可能
- OpenGL　3.3 core profileを使用
- Frament shader　のみで頑張ってみる。　

## 機能
- 配列操作
 - split, merge, tiled, untiled, copy, 
- 要素毎の演算
 - add, subtract, multiply, divide, min, max, mulSpectrums, log, exp, pow
 - convert, cvtColor
- 行列の縮約
 - reduce, norm
- 演算
 - FFT, normalize, minMaxLoc
- フィルタ
 - sepFilter2D, filter2D, gaussianFilter, boxFilter, Sobel, Laplacian
 - threshold, adaptiveThreshold

## 開発環境
 * Windows8
 * VC2013
  * NupenGL 0.1.0.1  (glew,glfw)
  * glm 0.9.7.1
  * OpenCV 2.1.10
  
## build
 1. glsCV.sln をVCで開く  
 2. Nugetでパッケージのインストール  
 2.1. "ツール->NuGet パッケージマネージャ->ソリューションのNugetパッケージの管理"より、以下のパッケージをインストール  
  * NupenGL 0.1.0.1  (glew,glfw)
  * glm 0.9.7.1
  * OpenCV 2.1.10
 3. ソリューションのbuild  

## glsCam
サンプルアプリケーション  
カメラからキャプチャーした画像をリアルタイム画像処理  
- Gray,　Edge(Sobel,Laplacian), 2値化(threshold,adaptiveThreshold) , FFT

 
 




