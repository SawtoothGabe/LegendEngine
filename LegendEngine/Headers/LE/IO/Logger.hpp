#pragma once

#include <format>
#include <memory>
#include <optional>
#include <source_location>
#include <string>
#include <vector>

#ifndef NDEBUG
#define LE_INFO(...) le::Logger::GetGlobalLogger().Info(__VA_ARGS__)
#define LE_WARN(...) le::Logger::GetGlobalLogger().Warn(__VA_ARGS__)
#define LE_ERROR(...) le::Logger::GetGlobalLogger().Error(__VA_ARGS__)
#define LE_DEBUG(...) le::Logger::GetGlobalLogger().Debug(__VA_ARGS__)
#define LE_INFO_TRACE(...) le::Logger::GetGlobalLogger().InfoTrace(std::source_location::current(), __VA_ARGS__)
#define LE_WARN_TRACE(...) le::Logger::GetGlobalLogger().WarnTrace(std::source_location::current(), __VA_ARGS__)
#define LE_ERROR_TRACE(...) le::Logger::GetGlobalLogger().ErrorTrace(std::source_location::current(), __VA_ARGS__)
#define LE_DEBUG_TRACE(...) le::Logger::GetGlobalLogger().DebugTrace(std::source_location::current(), __VA_ARGS__)
#else
#define LE_INFO(...) ;
#define LE_WARN(...) ;
#define LE_ERROR(...) ;
#define LE_DEBUG(...) ;
#define LE_INFO_TRACE(...) ;
#define LE_WARN_TRACE(...) ;
#define LE_ERROR_TRACE(...) ;
#define LE_DEBUG_TRACE(...) ;
#endif

namespace le
{
	class LoggerSink;
	
	class Logger
	{
	public:
		enum class Level
		{
			DEBUG,
			INFO,
			WARN,
			ERROR
		};

		explicit Logger(std::string_view name, bool addStdoutSink = true);

		void Log(Level level, std::string_view message, std::optional<std::source_location> location = std::nullopt);

		template <typename... Args>
		void Info(std::format_string<Args...> format, Args&&... args) {
			Log(Level::INFO, std::format(format, std::forward<Args>(args)...));
		}
		
		template <typename... Args>
		void Warn(std::format_string<Args...> format, Args&&... args) {
			Log(Level::WARN, std::format(format, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void Error(std::format_string<Args...> format, Args&&... args) {
			Log(Level::ERROR, std::format(format, std::forward<Args>(args)...));
		}

		template <typename... Args>
		void Debug(std::format_string<Args...> format, Args&&... args) {
			Log(Level::DEBUG, std::format(format, std::forward<Args>(args)...));
		}
		
		template <typename... Args>
		void InfoTrace(const std::source_location& location, std::format_string<Args...> format, Args&&... args) {
			Log(Level::INFO, std::format(format, std::forward<Args>(args)...), location);
		}
		
		template <typename... Args>
		void WarnTrace(const std::source_location& location, std::format_string<Args...> format, Args&&... args) {
			Log(Level::WARN, std::format(format, std::forward<Args>(args)...), location);
		}

		template <typename... Args>
		void ErrorTrace(const std::source_location& location, std::format_string<Args...> format, Args&&... args) {
			Log(Level::ERROR, std::format(format, std::forward<Args>(args)...), location);
		}

		template <typename... Args>
		void DebugTrace(const std::source_location& location, std::format_string<Args...> format, Args&&... args) {
			Log(Level::DEBUG, std::format(format, std::forward<Args>(args)...), location);
		}

		static Logger& GetGlobalLogger(std::string_view name = "LegendEngine");
	private:
		static std::string GetFormattedTime();
		static std::string GetFormattedSource(const std::source_location& location);

		std::string m_Name;
		std::vector<std::shared_ptr<LoggerSink>> m_Sinks;
	};
}