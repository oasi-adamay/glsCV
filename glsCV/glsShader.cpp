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

#include "stdafx.h"

#include "glsCV.h"

#include "glsShader.h"
#include "Timer.h"

namespace gls
{

void glsShaderBase::LoadShadersCode(const std::string& VertexShaderCode, const std::string& FragmentShaderCode, const std::string& shaderbin_file_path){
	
	const bool createBin = !shaderbin_file_path.empty();	//! create shader bin mode

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);


	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	//cout << "Compiling vertex shader" << endl;
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	if (Result != GL_TRUE){
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0){
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			cerr << &VertexShaderErrorMessage[0] << endl;
		}
	}
	GLS_Assert(Result == GL_TRUE);



	// Compile Fragment Shader
	//cout << "Compiling fragment shader" << endl;
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	if (Result != GL_TRUE){
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0){
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			cerr << &FragmentShaderErrorMessage[0] << endl;
		}
	}

	GLS_Assert(Result == GL_TRUE);


	// Link the program
	//cout << "Linking program" << endl;
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	if (createBin) glProgramParameteri(ProgramID, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	if (Result != GL_TRUE){
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0){
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			cerr << &ProgramErrorMessage[0] << endl;
		}
	}
	GLS_Assert(Result == GL_TRUE);

	// save shader bin 
	if(createBin){
		GLint formats = 0;
		glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
		//cout << "formats:" << formats << endl;

		int length;
		glGetProgramiv(ProgramID, GL_PROGRAM_BINARY_LENGTH, &length);
		//cout << "length:" << length << endl;

		if (length > 0 && formats>0){
			std::vector<char> binary(length);
			vector<GLenum> binaryFormats(formats);
			int len;
			glGetProgramBinary(ProgramID, length, &len, binaryFormats.data(), &binary[0]);
			if (length == len){
				cout << "save shader bin to:" << shaderbin_file_path << endl;
				ofstream ofs;
				ofs.open(shaderbin_file_path, ios_base::out | ios_base::trunc | ios_base::binary);
				ofs.write(binary.data(), binary.size());
				ofs.close();
			}
		}
		else{
			cout << "shader binary might be not supported." << endl;
		}
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	program = ProgramID;

}


void glsShaderBase::LoadShadersFile(const std::string& vertex_file_path, const std::string& fragment_file_path, const std::string& shaderbin_file_path){


	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()){
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else{
		cout << "Impossible to open:" << vertex_file_path << endl;
		GLS_Assert(0);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()){
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else{
		cout << "Impossible to open:" << fragment_file_path << endl;
		GLS_Assert(0);
	}

	LoadShadersCode(VertexShaderCode, FragmentShaderCode);

}


bool glsShaderBase::LoadShadersBinary(const std::string& shaderbin_file_path){
	GLuint progId = 0;
	std::vector<char> binary;
	{
		std::ifstream ifs(shaderbin_file_path, std::ios::in | ios_base::binary);
		if (!ifs)return false;
		size_t length = (size_t)ifs.seekg(0, std::ios::end).tellg();
		ifs.seekg(0, std::ios::beg);
		binary.resize(length);
		ifs.read(binary.data(), length);
		ifs.close();
	}

	{
		GLint formats = 0;
		glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
		//cout << "formats:" << formats << endl;
		if (formats == 0) return false;

		vector<GLenum> binaryFormats(formats);
		glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, (GLint*)binaryFormats.data());

		progId = glCreateProgram();
		glProgramBinary(progId, binaryFormats[0], binary.data(), (GLsizei)binary.size());

		GLint success;
		int InfoLogLength;
		glGetProgramiv(progId, GL_LINK_STATUS, &success);
		if (success != GL_TRUE){
			glGetProgramiv(progId, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if (InfoLogLength > 0){
				std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
				glGetProgramInfoLog(progId, InfoLogLength, NULL, &ProgramErrorMessage[0]);
				//		printf("%s\n", &ProgramErrorMessage[0]);
				cout << &ProgramErrorMessage[0] << endl;
			}
		}
		assert(success == GL_TRUE);

	}

	program = progId;

	return true;
}

string glsShaderBase::shaderBinName(const std::string funcname){
	string binname;
	size_t pos1 = funcname.rfind(':');
	if (pos1 != string::npos){
		binname = funcname.substr(pos1 + 1, funcname.size() - pos1);
	}
	else{
		GLS_Assert(0);
	}

	return binname + ".bin";
}

string glsShaderBase::VertexShaderCode(void){
	static const char vertexShaderCode[] =
"#version 330 core\n"
"layout (location = 0)in  vec2 position;\n"
"void main(void)\n"
"{\n"
"   gl_Position  = vec4(position,0.0,1.0);\n"
"}\n"
;

	return vertexShaderCode;
}



}//namespace gls
//-----------------------------------------------------------------------------

