
#include "watchdog.h"

namespace Techless
{
	std::vector<LogMessage> Debug::MessageLog = {};

	std::string GetTimeAsString() {
		const time_t TimeNow = time(nullptr);
		struct tm Time;

		localtime_s(&Time, &TimeNow);

		auto hour = std::to_string(Time.tm_hour);
		auto minute = std::to_string(Time.tm_min);
		auto second = std::to_string(Time.tm_sec);

		if (hour.length() == 1)
			hour = "0" + hour;
		if (minute.length() == 1)
			minute = "0" + minute;
		if (second.length() == 1) 
			second = "0" + second;

		std::string TimeStr = hour + ":" + minute + ":" + second;

		return TimeStr;
	}

	void Debug::Log(const std::string& String, const std::string& Type, const std::string& Header)
	{
		std::string fHeader = "";
		if (Header != "")
			fHeader = "[" + Header + "]";

		std::string TimeString = GetTimeAsString();

		std::string Final = "[" + TimeString + "][" + Type + "]" + fHeader + " " + String;

		std::cout << Final << std::endl;
		MessageLog.push_back({ TimeString, Type, Header, String });
	}

	void Debug::Log(const std::string& String, const std::string& Header)
	{
		Log(String, "INFO", Header);
	}

	void Debug::Warn(const std::string& String, const std::string& Header)
	{
		Log(String, "WARN", Header);
	}

	void Debug::Error(const std::string& String, const std::string& Header)
	{
		Log(String, "ERROR", Header);
	}

	void Debug::OpenGLMessage(const std::string& String, GLenum Source, GLenum Type, GLuint Id, GLenum Severity)
	{
		std::string _source, _type, _severity;

		switch (Source) {
		case GL_DEBUG_SOURCE_API:
			_source = "API";
			break;

		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			_source = "WINDOW";
			break;

		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			_source = "SHADER";
			break;

		case GL_DEBUG_SOURCE_THIRD_PARTY:
			_source = "THIRD PARTY";
			break;

		case GL_DEBUG_SOURCE_APPLICATION:
			_source = "APPLICATION";
			break;

		case GL_DEBUG_SOURCE_OTHER:
			_source = "UNKNOWN";
			break;

		default:
			_source = "UNKNOWN";
			break;
		}

		switch (Type) {
		case GL_DEBUG_TYPE_ERROR:
			_type = "ERROR";
			break;

		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			_type = "DEPRECATED";
			break;

		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			_type = "UNDEFINED";
			break;

		case GL_DEBUG_TYPE_PORTABILITY:
			_type = "PORTABILITY";
			break;

		case GL_DEBUG_TYPE_PERFORMANCE:
			_type = "PERFORMANCE";
			break;

		case GL_DEBUG_TYPE_OTHER:
			_type = "OTHER";
			break;

		case GL_DEBUG_TYPE_MARKER:
			_type = "MARKER";
			break;

		default:
			_type = "UNKNOWN";
			break;
		}

		
		switch (Severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			_severity = "ERROR";
			break;

		case GL_DEBUG_SEVERITY_LOW:
		case GL_DEBUG_SEVERITY_MEDIUM:
			_severity = "WARN";
			break;

		default:
			_severity = "INFO";
		}
		

		Log("OpenGL Error " + std::to_string(Id) + " [" + _source + "]: " + String, _severity, "OpenGL");
	}

}