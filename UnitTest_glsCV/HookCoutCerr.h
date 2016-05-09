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

#ifndef _HookCoutCerr_H_
#define _HookCoutCerr_H_

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

class MyStreambuf : public std::streambuf {
private:
	string buf;
public:
	virtual int_type overflow(int_type c = EOF) {
		if (c != EOF)
		{
			buf += c;
			if (c=='\n'){
				Logger::WriteMessage(buf.c_str());
				buf.clear();
			}
//			char buf[] = { c, '\0' };
//			Logger::WriteMessage(buf);


		}
		return c;
	}
};

class HookCoutCerr{
	MyStreambuf my_stream_cout;
	MyStreambuf my_stream_cerr;
	std::streambuf *default_stream_cout;
	std::streambuf *default_stream_cerr;
public:
	HookCoutCerr(void)  {
		default_stream_cout = std::cout.rdbuf(&my_stream_cout);
		default_stream_cerr = std::cerr.rdbuf(&my_stream_cerr);
	}
	~HookCoutCerr(void) {
		std::cout.rdbuf(default_stream_cout);
		std::cerr.rdbuf(default_stream_cerr);
	}
};


#endif