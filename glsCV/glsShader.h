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

#include "GlsMat.h"
#include "glsFBO.h"
#include "glsVAO.h"

namespace gls
{

class glsShaderBase;
/*!
Shader List
*/
extern list<glsShaderBase*> ShaderList;
/*!
Initialize all shader 
*/
void ShaderInitAll(void);



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

protected:
	const string name;		//Shader name;

	virtual string VertexShaderCode(void);
	virtual string FragmentShaderCode(void){ return string(""); }

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

};


}//namespace gls


#endif