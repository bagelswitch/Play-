#include "FrameLimiter.h"
#include <cassert>
#include <thread>

LARGE_INTEGER ft = {};

CFrameLimiter::CFrameLimiter()
{
	timeBeginPeriod(1);
	m_timer = CreateWaitableTimer(NULL, TRUE, NULL);
}

CFrameLimiter::~CFrameLimiter()
{
	CloseHandle(m_timer);
	timeEndPeriod(1);
}

void CFrameLimiter::BeginFrame()
{
	assert(!m_frameStarted);
	m_lastFrameTime = std::chrono::high_resolution_clock::now();
	if(m_delay && m_enabled)
	{
		SetWaitableTimer(m_timer, &ft, 0, NULL, NULL, 0);
		m_active = true;
	}
	else {
		m_active = false;
	}
	m_frameStarted = true;
}

void CFrameLimiter::EndFrame()
{
	assert(m_frameStarted);
	m_lastFrameDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_lastFrameTime);
	m_delay = (m_minFrameDuration - m_lastFrameDuration) > m_graceUsec;
	if(m_active && m_delay && m_enabled)
	{
		WaitForSingleObject(m_timer, INFINITE);
	}
	m_frameStarted = false;
}

void CFrameLimiter::SetFrameRate(uint32 fps)
{
	if(fps == 0)
	{
		m_minFrameDuration = std::chrono::microseconds(0);
		m_enabled = false;
	}
	else
	{
		m_minFrameDuration = std::chrono::microseconds(1000000 / fps);
		m_enabled = true;
	}

	ft.QuadPart = -(m_minFrameDuration.count() * 10);
}
