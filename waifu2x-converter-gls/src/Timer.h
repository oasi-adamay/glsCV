#ifndef _TIMER_H_
#define _TIMER_H_

//-----------------------------------------------------------------------------
class Timer{
private:
	cv::TickMeter meter;
	std::string msg;
public:
	Timer(std::string _msg){ msg = _msg;  meter.start(); }
	~Timer(void){ meter.stop(); std::cout << msg << meter.getTimeMilli() << "[ms]" << std::endl; }
};

#endif