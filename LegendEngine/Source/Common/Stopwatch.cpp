#include <LE/Common/Stopwatch.hpp>

namespace le
{
	Stopwatch::Stopwatch()
		:
		m_StartTime(std::chrono::high_resolution_clock::now())
	{}

	void Stopwatch::Set()
	{
		this->m_StartTime = std::chrono::high_resolution_clock::now();
	}

	float Stopwatch::GetElapsedMillis() const
	{
		const std::chrono::duration<float, std::milli> timeSpan =
			std::chrono::high_resolution_clock::now() - m_StartTime;
		return timeSpan.count();
	}
}
