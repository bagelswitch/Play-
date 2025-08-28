#pragma once

#include <chrono>
#include "Types.h"

#ifdef _WIN32
#include <Windows.h>
#endif

class CFrameLimiter
{
public:
	CFrameLimiter();
	~CFrameLimiter();

	void BeginFrame();
	void EndFrame();

	void SetFrameRate(uint32);

private:
	typedef std::chrono::high_resolution_clock::time_point TimePoint;

	std::chrono::microseconds m_graceUsec = std::chrono::microseconds(2000);
	std::chrono::microseconds m_minFrameDuration = std::chrono::microseconds(0);
	bool m_frameStarted = false;
	TimePoint m_lastFrameTime;
	std::chrono::microseconds m_lastFrameDuration;
	boolean m_delay = false;
	boolean m_enabled = false;
	boolean m_active = false;

#ifdef _WIN32
	HANDLE m_timer = 0;
#endif
};
