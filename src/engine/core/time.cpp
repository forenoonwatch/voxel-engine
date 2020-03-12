#include "engine/core/time.hpp"

#if defined(OPERATING_SYSTEM_LINUX)

#include <unistd.h>
#include <ctime>

Time::Time() {
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	start = ts.tv_sec + static_cast<double>(ts.tv_nsec) / 1.0e9;
}

void Time::sleep(double time) noexcept {
	usleep(static_cast<int>(time * 1.0e6));
}

double Time::getTimeInternal() const noexcept {
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	double current = ts.tv_sec + static_cast<double>(ts.tv_nsec) / 1.0e9;

	return current - start;
}

#elif defined(OPERATING_SYSTEM_WINDOWS)

#define NOMINMAX
#include <windows.h>

static LARGE_INTEGER timerFrequency;
static LARGE_INTEGER timerStart;

Time::Time() {
	QueryPerformanceFrequency(&timerFrequency);
	QueryPerformanceCounter(&timerStart);
}

double Time::getTimeInternal() const noexcept {
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	return static_cast<double>(now.QuadPart - timerStart.QuadPart)
			/ static_cast<double>(timerFrequency.QuadPart);
}

void Time::sleep(double time) noexcept {
	const int millis = time * 1000;
	Sleep(millis);
}

#else
	#error "TODO: implement timing for this OS"
#endif

