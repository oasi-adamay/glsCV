# glsCV
OpenGLSLを使用したCVライブラリ  

リアルタイムカメラ、waifu2x　等のアプリケーション

## glsCV
OpenGLSLを使用したCVライブラリ  
- OpenGL/OpenGLSLで記述した、OpenCV　Likeなライブラリ
- cv::gpu(CUDA)と同じように、I/FはOpenCV互換
- NVIDIA非搭載のNotePCでも実行可能
- OpenGL　3.3 core profileを使用

[more] (glsCV/README.md)

## glsCam
カメラからキャプチャーした画像をリアルタイム画像処理  
- FFT, threshold, bilateral-filter , edge image ,　CANNY
- pre-filter (temporary noise reduction)
- resize (bicubic)

[more] (glsCam/README.md)  

## waifu2x-converter-gls
waifu2x のglsCVによる最適化  
[more] (waifu2x-converter-gls/README.md)  

## LICENSE
BSD-3clause license  

## 開発環境
 * Windows8 / Windows10
 * VC2013
  * NupenGL 0.1.0.1  (glew,glfw)
  * OpenCV 2.4.10

## build
 1. glsCV.sln をVCで開く  
 2. Nugetでパッケージのインストール  
 2.1. "ツール->NuGet パッケージマネージャ->ソリューションのNugetパッケージの管理"より、以下のパッケージをインストール  
  * NupenGL 0.1.0.1  (glew,glfw)
  * glm 0.9.7.1
  * OpenCV 2.4.10
 3. ソリューションのbuild  
