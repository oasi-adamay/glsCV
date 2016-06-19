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

#ifndef _GLS_SHADER_H_
#define _GLS_SHADER_H_

namespace gls
{


class GlsMat;
class AtomicCounter;

/*!
glsShader Base Class

 - load from shader source files and compile,link.
 - load from shader code and compile,link.
 - load from shader binary files and link.
*/
class glsShaderBase
{
private:
	glsShaderBase(const glsShaderBase&);              ///  Uncopyable
	glsShaderBase& operator=(const glsShaderBase&);   ///  Uncopyable

	shared_ptr<GLuint> _program;	//!< program idを保存する、shared_ptr
	vector<GLuint> uniformLocArray;	//!<  uniform location cache

	/*!
	setup info 
	*/
	struct  SetupInfo{
		int argnum;			//Execute()の引数の数
		int texSrcNum;		//src texture数
		Size texSrcSize;	//src textureのsize
		int texDstNum;		//dst texture数
		Size texDstSize;	//dst textureのsize
		GLuint fbo;			//fbo
	};

	//! setup (uniform / bind / ...)
	template<class T>	void setup(SetupInfo* info, const T& t);
	template<>	void setup<int>(SetupInfo* info, const int& t);			//!< uniform int
	template<>	void setup<float>(SetupInfo* info, const float& t);		//!< uniform float
	template<>	void setup<double>(SetupInfo* info, const double& t);	//!< uniform float
	template<>	void setup<Scalar>(SetupInfo* info, const Scalar& t);	//!< uniform vec4
	template<>	void setup<Vec2f>(SetupInfo* info, const Vec2f& t);		//!< uniform vec2
	template<>	void setup<Vec3f>(SetupInfo* info, const Vec3f& t);		//!< uniform vec3
	template<>	void setup<Vec4f>(SetupInfo* info, const Vec4f& t);		//!< uniform vec
	template<>	void setup<Vec2i>(SetupInfo* info, const Vec2i& t);		//!< uniform ivec2
	template<>	void setup<Vec3i>(SetupInfo* info, const Vec3i& t);		//!< uniform ivec3
	template<>	void setup<Vec4i>(SetupInfo* info, const Vec4i& t);		//!< uniform ivec4
	template<>	void setup<Matx22f>(SetupInfo* info, const Matx22f& t);	//!< uniform mat2(transpose)
	template<>	void setup<Matx33f>(SetupInfo* info, const Matx33f& t);	//!< uniform mat3(transpose)
	template<>	void setup<Matx44f>(SetupInfo* info, const Matx44f& t);	//!< uniform mat4(transpose)
	template<>	void setup<Matx23f>(SetupInfo* info, const Matx23f& t);	//!< uniform mat3x2(transpose)
	template<>	void setup<Matx32f>(SetupInfo* info, const Matx32f& t);	//!< uniform mat2x3(transpose)
	template<>	void setup<Matx34f>(SetupInfo* info, const Matx34f& t);	//!< uniform mat4x3(transpose)
	template<>	void setup<Matx43f>(SetupInfo* info, const Matx43f& t);	//!< uniform mat3x4(transpose)
	template<>	void setup<GlsMat>(SetupInfo* info, const GlsMat& t);	//!<
	template<>	void setup<AtomicCounter>(SetupInfo* info, const AtomicCounter& t);	//!<

	template<class First, class... Rest>
	void setup(SetupInfo* info, const First& first, const Rest&... rest);


protected:
	const string name;		//Shader name;

	virtual string VertexShaderCode(void);
	virtual string FragmentShaderCode(void){ return string(""); }
	virtual list<string> UniformNameList(void){ return list<string>(0,""); }


	//gloabl list
//	static list<glsShaderBase*> ShaderList;

public:
//	glsShaderBase(const string& _name) : name(_name){ ShaderList.push_back(this); }
	glsShaderBase(const string& _name) : name(_name){ ; }

	~glsShaderBase(void){ if (_program.use_count() == 1) glDeleteProgram(*_program); }


	/*!
	load shader filse(string) , compile and link.

	@param VertexShaderCode vertex shader file path.
	@param FragmentShaderCode fragment shader file path.
	@param shaderbin_file_path  option. if it is not empty, shader binary file will be created.
	*/
	void LoadShadersFile(const std::string& vertex_file_path, const std::string& fragment_file_path, const std::string& shaderbin_file_path = "");

	/*!
	load shader code(string) , compile and link.

	@param VertexShaderCode vertex shader code strings.
	@param FragmentShaderCode fragment shader code strings.
	@param shaderbin_file_path  option. if it is not empty, shader binary file will be created.
	*/
	void LoadShadersCode(const std::string& VertexShaderCode, const std::string& FragmentShaderCode, const std::string& shaderbin_file_path = "");

	/*!
	load shader binary.

	@param shaderbin_file_path.
	*/
	bool LoadShadersBinary(const std::string& shaderbin_file_path);

	/*!
	get shader binary filename.

	@param classname  __FUNCTION__
	*/
	string shaderBinName(const std::string funcname);

	/*!
	get program id.

	if program is not loaded yet, it will be  compiled and loaded.
	*/
	GLuint program(void) const;

	/*!
	initalize porgram

	*/
	void Init(void);

	void InitAll(void);

	/*!
	Shaderの実行
	*/
	template<class First, class... Rest>
	void Execute(const First& first, const Rest&... rest);


	/*!
	OpenGL API wrap
	*/

	//! glDrawBuffers wrap
	void DrawBuffers(const int attachment_num);

	//! uniform設定
	void setUniform(const string& name, const int & val) const{
		glUniform1i(glGetUniformLocation(program(),name.c_str()), val);
	}

	void setUniform(const string& name, const float & val)const{
		glUniform1f(glGetUniformLocation(program(), name.c_str()), val);
	}

	void setUniform(const string& name, const Scalar & val)const{
		float _val[4] = { (float)val[0], (float)val[1], (float)val[2], (float)val[3] };
		glUniform4fv(glGetUniformLocation(program(), name.c_str()), 1, _val);
	}


};



/*!
Initialize all shader
*/
void ShaderInitAll(void);

}//namespace gls


/*
Template include
*/
#include "glsShader_impl.h"


#endif