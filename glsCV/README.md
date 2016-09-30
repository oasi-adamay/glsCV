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
 - split, merge, tiled, untiled, copy, flip, cartToPolar
- 要素毎の演算
 - add, subtract, multiply, divide, min, max, mulSpectrums, log, exp, pow
 - convert, cvtColor
- 行列の縮約
 - reduce, norm, mean, meanStdDev, minMaxLoc
- 演算
 - FFT, normalize
- フィルタ
 - sepFilter2D, filter2D, gaussianFilter, boxFilter, Sobel, Laplacian, bilateralFilter, Canny, accumulateWeighted
 - threshold, adaptiveThreshold
- 幾何変換
 - remap ,resize, warpAffine

## 開発環境
 * Windows8 / 10
 * VC2013
  * NupenGL 0.1.0.1  (glew,glfw)
  * OpenCV 2.4.10

## build
 1. glsCV.sln をVCで開く  
 2. Nugetでパッケージのインストール  
 2.1. "ツール->NuGet パッケージマネージャ->ソリューションのNugetパッケージの管理"より、以下のパッケージをインストール  
  * NupenGL 0.1.0.1  (glew,glfw)
  * glm 0.9.7.1
  * OpenCV 2.1.10
 3. ソリューションのbuild  

# glsCV manual
## gls::GlsMat
glsCVでは、`cv::Mat`の代わりに、`gls::GlsMat`を使用する。
gls::GlsMatは、OpenGL textureのwrapperクラスであり、以下の機能を持つ。
- OpenGL textureの生成、管理
  生成するtextureは、２次元配列では、`GL_TEXURE_2D`を使用し、３次元配列、`GL_TEXURE_2D_ARRAY`を使用する。
  (4次元以上の配列は未サポート)

  生成するtextureのinternal fomratは、Sized formatを使用する。
  詳細は、[OpenCVとOpenGLのデータ型対応](#opencv_opengl_type)参照

  textureは、shader_ptrで管理し、参照先がなくなると、自動的に廃棄する。

- cv：：Mat　との相互変換
  データ型の変換 cv::Mat m.type() ⇔ GL sized format
  データのupload,download

### コンストラクタ
OpenCVのcv::Matと同様のコンストラクタ。（全ての形式をサポートしているわけではない）
画像のサイズ(`cv::Size`)、データ型はOpenCVの定義(`CV_32FC1,CV_8UC3...`)と互換性がある。
以下は、代表的コンストラクタの例。
``` cpp
  //空のMat　textureは生成されない。
  gls::GlsMat m0;

  //640x480 の float 1ch のtexture生成
  gls::GlsMat m1(cv::Size(640,480),CV_32FC1);

  //cv::Matからの変換
  //cvmatと同じsize、同じtypeのtextureを生成。
  //cvmatのデータをGPUにアップロードする。
  cv::Mat cvmat(cv::Size(640,480),CV_32FC1);
  gls::GlsMat m2(cvmat);
```
### 代入
cv::Mat 同様、代入は浅いコピー（shallow copy）であり、texture自体はコピーされない。
管理情報のみコピーされるため、コピー元とコピー先とで同じtextureを共有することになる。
この場合，texture自身が実際にはコピーされないので、コピー動作は、サイズによらず一定で高速に行われる。
一方、コピー先のtextureを書き換えると、コピー元のtextureも書き換わるので注意が必要。
コピー元とコピー先を別データとしてコピーするには，`clone()`メソッドを利用して，深いコピー（deep copy）を行う。
 `clone()`メソッド内部では，新たなgls::GlsMatが作成され，そこに`copyTo()`メソッドでtexture全体がコピーされる。
``` cpp
  //cvmatと同じsize、同じtypeのtextureを生成。
  //cvmatのデータをGPUにアップロードする。
  cv::Mat cvmat(cv::Size(640,480),CV_32FC1);
  gls::GlsMat m1(cvmat);
  gls::GlsMat m2;

  //浅いコピー　データのコピーは発生しない。
  // m1とm2は、同一のtextureを共有する。
  m2 = m1;
```
### キャスト
cv::Mat <-> gls::GlsMat の相互変換
変換は実質的に、upload、downloadを伴うため、暗黙のキャストは禁止している。
明示的にキャストする必要がある。
アーキテクチャによるが、upload,downloadは意外と処理時間がかかる場合がある。
なるべくCPU<->GPU間のデータのやり取りが少なくなるようにする。
``` cpp
  //　ｃｖ::Matから、gls::GlsMatへのキャスト
  // cvmatと同一のサイズ、タイプのtexuteを生成し、データをupload
  cv::mat cvmat;
  gls::GlsMat glmat = (gls::GlsMat)cvmat;

  // glmatに対する、何かの処理・・・

  //gls::GlsMatから、ｃｖ::Matへのキャスト
  // textureからcvmatへdownload
  cvmat = (cv::Mat)glmat;
```
### GlsMatでサポートされていない機能
以下の機能は、GlsMatではサポートされていない。
- 演算子のオーバーロード
　関数形式で表現できるので。
- 生ポインタの取得、変更
　GPU管理のtextureは、直接触れないため。
  (PBOに転送して、Mapすればできる)
- reshape(),row(),col(),cv::Rectを使用した部分配列。
- 5ch以上のマルチチャンネル配列
  GPUは、4chまで扱えるため。
- 4次元以上の配列
　textureの上限が３次元
- その他、未実装機能あり。

## gls::glsShaderBase
shaderを管理する基底クラス。様々な機能を実装する各shaderは、このクラスを継承している。
`gls::glsShaderBase`は、以下の機能を提供する。
- glslで記述された、vertex shader、fragment shaderのload,compile,link
- コンパイル済みバイナリのload
- uniform location,atribute loacationのキャッシュ
- textute,VAO,FBOなどのBinding,Renderなどの一連のシーケンスの抽象化

### FragmentShaderCode()
`gls::glsShaderBase`を継承したクラスは、fragment shaderのコードを取得する仮想関数`virtual string FragmentShaderCode(void)`をオーバーライドすることで、特有の機能を実装する。
このメソッドは、shaderのプログラムIDを取得する`gls::glsShaderBase::program()`が呼ばれた際に、自動的に呼ばれる。

### UniformNameList()
プログラムから、UniformLocationを決定するために、このメソッドをオーバライドする。
FragmentShaderCodeに含まれる、uniform変数の文字列を定義する。
FragmentShaderのロード後に、このメソッドに登録された文字列に対して、`glGetUniformLocation（）`をコールし、各Uniform変数のUniformLocationを取得する。

### Execute(・・・)
programの使用、VAO,FBO,textureのbinding、viewpointの設定、draw発行などの一連のシーケンスを実現するためのメソッド。
c++11の可変長テンプレートを使用して実装している。
``` cpp
  // ２つのsrc texture,２つのparam, １つのdst textureの例
  GlsMat src1,src2;
  int param1;
  Vec2f param2;
  GlsMat dst1,dst2;
  shader.Execute(src1,src2,param1,param2,dst1,dst2);
```
この例では、以下のシーケンスが実行される。
1. プログラムの使用
　glUseProgram(program());
  この時、プログラムがまだロードされていなければ、ロードする。
2. VAO、VBOの生成とバインド、アトリビュートの有効化
  { -1.0f, -1.0f },{ 1.0f, -1.0f },{ 1.0f, 1.0f },{ -1.0f, 1.0f }　の頂点情報の転送
3. src1,src2のtextureのbindと、uniform locationの設定
  uniform location は、`UniformNameList`に登録された順番で決定する。
4. param1,param2をuniform変数設定
  変数の型に応じて、適切なOpenGL APIでuniform変数を設定する。
  int -> glUniform1i();
  Vec2f -> glUniform2fv();
  uniform location は、`UniformNameList`に登録された順番で決定する。
5. FBOの生成
6. dst1,dst2のtextureをFrameBufferにアタッチ
7. viewpointの設定
　最後のdst textureのサイズから決定する。
8. Draw発行
  glDrawArrays();
  glFlush();

1～2 , 7～8のステップが共通部であり、Executeメソッドの引数の順番、内容に応じて3～6のステップが変わることになる。

## <a name="opencv_opengl_type"> OpenCVとOpenGLのデータ型対応
|OpenCV  |GlsMat     |
|:-------|:----------|
|CV_32FC1|GL_R32F    |
|CV_32FC2|GL_RG32F   |
|CV_32FC3|GL_RGB32F  |
|CV_32FC4|GL_RGBA32F |
|CV_8UC1 |GL_R8UI    |
|CV_8UC2 |GL_RG8UI   |
|CV_8UC3 |GL_RGB8UI  |
|CV_8UC4 |GL_RGBA8UI |
|CV_8SC1 |GL_R8I     |
|CV_8SC2 |GL_RG8I    |
|CV_8SC3 |GL_RGB8I   |
|CV_8SC4 |GL_RGBA8I  |
|CV_16UC1|GL_R16UI   |
|CV_16UC2|GL_RG16UI  |
|CV_16UC3|GL_RGB16UI |
|CV_16UC4|GL_RGBA16UI|
|CV_16SC1|GL_R16I    |
|CV_16SC2|GL_RG16I   |
|CV_16SC3|GL_RGB16I  |
|CV_16SC4|GL_RGBA16I |
|CV_32SC1|GL_R32I    |
|CV_32SC2|GL_RG32I   |
|CV_32SC3|GL_RGB32I  |
|CV_32SC4|GL_RGBA32I |
