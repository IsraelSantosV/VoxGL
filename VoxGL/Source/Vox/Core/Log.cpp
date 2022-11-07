#include "VoxPch.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/sinks/basic_file_sink.h>
#include "Vox/Editor/Console/EditorMessageSync.h"

namespace Vox
{
	std::shared_ptr<spdlog::logger> Log::m_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::m_ClientLogger;
	std::shared_ptr<spdlog::logger> Log::m_ConsoleLogger;

	void Log::Init()
	{
		std::string logsDirectory = "logs";
		if (!std::filesystem::exists(logsDirectory))
		{
			std::filesystem::create_directories(logsDirectory);
		}

		std::vector<spdlog::sink_ptr> voxSinks =
		{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/VoxGL.log", true),
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
		};

		std::vector<spdlog::sink_ptr> appSinks =
		{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/App.log", true),
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
		};

		std::vector<spdlog::sink_ptr> editorConsoleSinks =
		{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/Console.log", true),
			std::make_shared<EditorConsoleSync>(1)
		};

		voxSinks[0]->set_pattern("[%T] [%l] %n: %v");
		appSinks[0]->set_pattern("[%T] [%l] %n: %v");

		voxSinks[1]->set_pattern("%^[%T] %n: %v%$");
		appSinks[1]->set_pattern("%^[%T] %n: %v%$");
		for (auto sink : editorConsoleSinks)
		{
			sink->set_pattern("%^%v%$");
		}

		m_CoreLogger = std::make_shared<spdlog::logger>("VoxGL", voxSinks.begin(), voxSinks.end());
		m_CoreLogger->set_level(spdlog::level::trace);

		m_ClientLogger = std::make_shared<spdlog::logger>("App", appSinks.begin(), appSinks.end());
		m_ClientLogger->set_level(spdlog::level::trace);

		m_ConsoleLogger = std::make_shared<spdlog::logger>("Console", editorConsoleSinks.begin(), editorConsoleSinks.end());
		m_ConsoleLogger->set_level(spdlog::level::trace);
	}
}
