#include "CurrentTime.h"

CurrentTime::CurrentTime()
{
}

const uint64_t CurrentTime::getMilliSeconds()
{
	return (std::chrono::duration_cast<std::chrono::milliseconds>
		(m_clock.now().time_since_epoch()).count());
}

const uint64_t CurrentTime::getMicroSeconds()
{
	return std::chrono::duration_cast<std::chrono::microseconds>
		(m_clock.now().time_since_epoch()).count();
}

const uint64_t CurrentTime::getNanoSeconds()
{
	
	return std::chrono::duration_cast<std::chrono::nanoseconds>
		(m_clock.now().time_since_epoch()).count();
}
