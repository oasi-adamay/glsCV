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

#ifndef _TIMER_H_
#define _TIMER_H_

//-----------------------------------------------------------------------------
#if 0
class Timer{
private:
	cv::TickMeter meter;
	string msg;
public:
	Timer(string _msg){ msg = _msg;  meter.start(); }
	~Timer(void){ meter.stop(); std::cout << msg << meter.getTimeMilli() << "[ms]" << std::endl; }
};
#else
#include <time.h>
class Timer {
private:
	clock_t start, end;
	std::string msg;
	void Start(void) { start = clock(); }
	void Stop(void) { end = clock(); }
	double GetTimeMilli(void) { return 1000 * (end - start) / CLOCKS_PER_SEC; }
public:
	Timer(std::string _msg) { msg = _msg; Start(); }
	~Timer(void) { Stop(); std::cout << msg << GetTimeMilli() << "[ms]" << std::endl; }
};
#endif

#endif