#include <chrono>
#include <cstdint>

class CurrentTime {
public:
	CurrentTime();
	const uint64_t getMilliSeconds();
	const uint64_t getMicroSeconds();
	const uint64_t getNanoSeconds();

private:
	std::chrono::high_resolution_clock m_clock;
	
};