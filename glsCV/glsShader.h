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

#include "glsVAO.h"
#include "glsFBO.h"

#ifdef _DEBUG
//#define _DEBUG_SHADER
#endif

namespace gls
{


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

	setup関数内で更新される。
	*/
	struct  SetupInfo{
		int argnum;			//Execute()の引数の数
		int texSrcNum;		//src texture数
		int texDstNum;		//dst texture数
		Size texDstSize;	//dst textureのsize
	};

	// param が1個のために必要
	void setup(SetupInfo* info) //const
	{
		const int i = info->argnum++;
#ifdef _DEBUG_SHADER
		std::cout << "i:" << i << "\t";
		std::cout << "end!!" << std::endl;
#endif
	}

	template<class T>
	void setup(SetupInfo* info, const T& t)
	{
#ifdef _DEBUG_SHADER
		std::cout << "i:" << i << "\t";
		std::cout << "T:" << t << std::endl;
#endif
	}

	template<>
	void setup<int>(SetupInfo* info, const int& t)
	{
		const int i = info->argnum++;
		GLuint loc = uniformLocArray[i];
		glUniform1i(loc, t);
#ifdef _DEBUG_SHADER
		std::cout << "i:" << i << "\t";
		std::cout << "loc:" << loc << "\t";
		std::cout << "val:" << t << endl;
#endif
	}

	template<>
	void setup<float>(SetupInfo* info, const float& t)
	{
		const int i = info->argnum++;
		GLuint loc = uniformLocArray[i];
		glUniform1f(loc, t);
#ifdef _DEBUG_SHADER
		std::cout << "i:" << i << "\t";
		std::cout << "loc:" << loc << "\t";
		std::cout << "val:" << t << endl;
#endif
	}

	template<>
	void setup<double>(SetupInfo* info, const double& t)
	{
		const int i = info->argnum++;
		GLuint loc = uniformLocArray[i];
		float val = (float)t;
		glUniform1f(loc, val);
#ifdef _DEBUG_SHADER
		std::cout << "i:" << i << "\t";
		std::cout << "loc:" << loc << "\t";
		std::cout << "val:" << t << endl;
#endif
	}

	template<>
	void setup<Scalar>(SetupInfo* info, const Scalar& t)
	{
		const int i = info->argnum++;
		GLuint loc = uniformLocArray[i];
		float val[4] = { (float)t[0], (float)t[1], (float)t[2], (float)t[3] };
		glUniform4fv(loc, 1, val);
#ifdef _DEBUG_SHADER
		std::cout << "i:" << i << "\t";
		std::cout << "loc:" << loc << "\t";
		std::cout << "val:" << t << endl;
#endif
	}


	template<>
	void setup<GlsMat>(SetupInfo* info, const GlsMat& t)
	{
		const int i = info->argnum++;
		if (i<uniformLocArray.size()){	//src texture
			if (!t.empty()){
				GLuint loc = uniformLocArray[i];
				int id = info->texSrcNum++;
				glActiveTexture(GL_TEXTURE0 + id);
				glBindTexture(GL_TEXTURE_2D, t.texid());
				glUniform1i(loc, id);
				
#ifdef _DEBUG_SHADER
				std::cout << "i:" << i << "\t";
				std::cout << "loc:" << loc << "\t";
				std::cout << "texunit:" << id << "\t";
				std::cout << "texSrc:" << t.texid() << endl;
#endif
			}

		}
		else{	//dst texture
			if (!t.empty()){
				int id = info->texDstNum++;
				info->texDstSize = t.size();
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + id, GL_TEXTURE_2D, t.texid(), 0);
				//GLS_Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
#ifdef _DEBUG_SHADER
				std::cout << "i:" << i << "\t";
				std::cout << "attach:" << id << "\t";
				std::cout << "texdst:" << t.texid() << endl;
#endif
			}

		}
	}

	template<class First, class... Rest>
	void setup(SetupInfo* info, const First& first, const Rest&... rest)
	{
		setup(info, first);
		setup(info, rest...);
	}

	void DrawBuffers(const int attachment_num);


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
	uniform設定

	*/

	void setUniform(const string& name , const int & val) const{
		glUniform1i(glGetUniformLocation(program(),name.c_str()), val);
	}

	void setUniform(const string& name, const float & val)const{
		glUniform1f(glGetUniformLocation(program(), name.c_str()), val);
	}

	void setUniform(const string& name, const Scalar & val)const{
		float _val[4] = { (float)val[0], (float)val[1], (float)val[2], (float)val[3] };
		glUniform4fv(glGetUniformLocation(program(), name.c_str()), 1, _val);
	}


	template<class First, class... Rest>
	void Execute(const First& first, const Rest&... rest);


};


template<class First, class... Rest>
void glsShaderBase::Execute(const First& first, const Rest&... rest)
{
	glsVAO vao(glGetAttribLocation(program(), "position"));
	glsFBO fbo(1);

	//program
	{
		glUseProgram(program());
	}

	//setup
	SetupInfo info = { 0 };

	{
		setup(&info, first);
		setup(&info, rest...);
	}
#ifdef _DEBUG_SHADER
	cout << "argnum:" << info.argnum << endl;
	cout << "texSrcNum:" << info.texSrcNum << endl;
	cout << "texDstNum:" << info.texDstNum << endl;
	cout << "texDstSize:" << info.texDstSize << endl;

#endif

	DrawBuffers(info.texDstNum);

	//Viewport
	glViewport(0, 0, info.texDstSize.width, info.texDstSize.height);

	//Render!!
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glFlush();
	}

//	GL_CHECK_ERROR();

}



/*!
Initialize all shader
*/
void ShaderInitAll(void);




}//namespace gls


#endif