#include "FrameLimiter.h"
#include <cassert>
#include <thread>

CFrameLimiter::CFrameLimiter()
{
}

CFrameLimiter::~CFrameLimiter()
{
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

	auto delay = ((m_minFrameDuration - frameDuration) / 1000) * 1000;

	if(delay > std::chrono::microseconds(0))
	{
		std::this_thread::sleep_for(delay);
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
