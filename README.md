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

## 実装済み
- 要素毎の操作
 - add,sub,mul,div,....
- 行列の縮約
 - reduce
 - minMaxLoc
 - normalize
- 演算
 - FFT
- 画像処理
 - sepFilter2D
 - boxFilter
 - gaussianFilter
 - sobel
 - adaptiveThreshold / threshold

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

 
 




