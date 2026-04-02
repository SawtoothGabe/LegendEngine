#include <LE/IO/Logger.hpp>

#include <LE/IO/LoggerSinks.hpp>
#include <LE/Application.hpp>

#include <chrono>
#include <ctime>
#include <format>

namespace le
{
	namespace LogColor
	{
		static const std::string BLUE = "\x1b[38;2;0;120;220m";
		static const std::string GOLD = "\x1b[38;2;255;213;0m";
		static const std::string GREEN = "\x1b[38;2;0;150;60m";
		static const std::string RED = "\x1b[38;2;255;0;0m";
		static const std::string LIGHT_GRAY = "\x1b[38;2;118;118;118m";
		static const std::string DARK_GRAY = "\x1b[38;2;148;148;148m";
		static const std::string RESET = "\x1b[0m";
	}

	class GlobalLogger : public Logger
	{
	public:
		explicit GlobalLogger(const std::string_view name)
			:
			Logger(name)
		{}

		~GlobalLogger()
		{
			Application::Destroy();
		}
	};

	Logger::Logger(const std::string_view name, bool addStdoutSink)
		:
		m_Name(name)
	{
		if (addStdoutSink)
			m_Sinks.push_back(std::make_shared<StdoutSink>());
	}

	void Logger::Log(const Level level, std::string_view message, const std::optional<std::source_location> location)
	{
		std::string severity;
		std::string color;

		switch (level)
		{
			case Level::INFO:
			{
				severity = "INFO";
				color = LogColor::BLUE;
			}
			break;

			case Level::WARN:
			{
				severity = "WARN";
				color = LogColor::GOLD;
			}
			break;

			case Level::DEBUG:
			{
				severity = "DEBUG";
				color = LogColor::GREEN;
			}
			break;

			case Level::ERROR:
			{
				severity = "ERROR";
				color = LogColor::RED;
			}
			break;
		}

		std::string time = GetFormattedTime();

		std::string colorized = std::format(
			"{0}[{1}{2}{0}] [{1}{3}{0}] {4}: {5}[{6}]",
			LogColor::LIGHT_GRAY,
			LogColor::DARK_GRAY,
			time,
			m_Name,
			LogColor::RESET,
			color,
			severity
		);

		std::string plain = std::format(
			"[{}] [{}] [{}]",
			time,
			m_Name,
			severity,
			message
		);

		if (location.has_value())
		{
			const std::string formattedSource = GetFormattedSource(location.value());
			colorized += formattedSource;
			plain += formattedSource;
		}

		colorized += std::format(" => {}{}", message, LogColor::RESET);
		plain += std::format(" => {}", message);

		for (auto& sink : m_Sinks)
			sink->Log(colorized, plain);
	}

	Logger& Logger::GetGlobalLogger(const std::string_view name)
	{
		static GlobalLogger logger(name);
		return logger;
	}

	std::string Logger::GetFormattedTime()
	{
		const auto now = std::chrono::system_clock::now();

		const std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
		const std::tm* pTime = std::localtime(&currentTime);

		// strftime adds the null terminator, so initializing isn't strictly necessary
		char time[32];
		std::strftime(time, sizeof(time), "%H:%M:%S", pTime);

		return time;
	}

	std::string Logger::GetFormattedSource(const std::source_location& location)
	{
		return std::format(" {}:{} ({}) ", location.file_name(), location.line(), location.function_name());
	}
}
