#include "Core/Logger.h"

HANDLE Logger::m_ConsoleHandle;

void Logger::display_log(int log_type, std::string msg)
{
	switch (log_type) {
	case 1:
		SetConsoleTextAttribute(Logger::m_ConsoleHandle, _INFO_COLOR);
		std::cout << "[INFO] ";
		break;
	case 2:
		SetConsoleTextAttribute(Logger::m_ConsoleHandle, _WARN_COLOR);
		std::cout << "[WARN] ";
		break;
	case 3:
		SetConsoleTextAttribute(Logger::m_ConsoleHandle, _ERROR_COLOR);
		std::cout << "[ERROR] ";
		break;
	default:
		SetConsoleTextAttribute(Logger::m_ConsoleHandle, _LOG_COLOR);
		std::cout << "[LOG] ";
		break;
	}

	SetConsoleTextAttribute(Logger::m_ConsoleHandle, 7);
	std::cout << msg << std::endl;
}

void Logger::init()
{
	Logger::m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

std::string Logger::to_string()
{
	return std::string();
}