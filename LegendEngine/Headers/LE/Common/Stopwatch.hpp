#pragma once

#include <chrono>

namespace le
{
	class Stopwatch 
	{
	public:
		Stopwatch();

		/**
		 * @brief Sets the clock to the current time.
		 *  In other words, this function resets the timer.
		 */
		void Set();

		/**
		 * @returns The time (in milliseconds) since the last time Set was called.
		 */
		[[nodiscard]] float GetElapsedMillis() const;
	private:
		std::chrono::high_resolution_clock::time_point m_StartTime;
	};
}
