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