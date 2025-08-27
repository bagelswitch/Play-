#include "FrameLimiter.h"
#include <cassert>
#include <thread>

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
	m_frameStarted = true;
}

void CFrameLimiter::EndFrame()
{
	assert(m_frameStarted);

	auto currentFrameTime = std::chrono::high_resolution_clock::now();
	auto frameDuration = std::chrono::duration_cast<std::chrono::microseconds>(currentFrameTime - m_lastFrameTime);

	auto delay = m_minFrameDuration - frameDuration;
	if(delay > std::chrono::microseconds(1000))
	{
		LARGE_INTEGER ft = {};
		ft.QuadPart = -static_cast<int64>(delay.count() * 10);
		SetWaitableTimer(m_timer, &ft, 0, NULL, NULL, 0);
		WaitForSingleObject(m_timer, INFINITE);
	}

	m_frameStarted = false;
}

void CFrameLimiter::SetFrameRate(uint32 fps)
{
	if(fps == 0)
	{
		m_minFrameDuration = std::chrono::microseconds(0);
	}
	else
	{
		m_minFrameDuration = std::chrono::microseconds(1000000 / fps);
	}
}
