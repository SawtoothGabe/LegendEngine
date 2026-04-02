#pragma once

#include <fstream>
#include <string>
#include <string_view>

namespace le
{
	enum class LogLevel;

	class LoggerSink
	{
	public:
		virtual ~LoggerSink() = default;
		virtual void Log(std::string_view prettyMessage, std::string_view basicMessage) = 0;
	};

	class StdoutSink final : public LoggerSink
	{
	public:
		void Log(std::string_view prettyMessage, std::string_view basicMessage) override;
	};

	class FileSink final : public LoggerSink
	{
	public:
		explicit FileSink(std::string_view filename);

		void Log(std::string_view prettyMessage, std::string_view basicMessage) override;
	private:
		std::string filename;
		std::ofstream file;
	};
}
