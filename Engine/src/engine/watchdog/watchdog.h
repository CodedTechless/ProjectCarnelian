#pragma once

#include <engineincl.h>

#include <GL/glew.h>

namespace Techless 
{
	
	struct LogMessage
	{
		std::string Time;
		std::string Type;
		std::string Header;
		std::string Contents;
	};

	class Debug
	{
	public:
		static void Log(const std::string& String, const std::string& Type, const std::string& Header);
		static void Log(const std::string& String, const std::string& Header = "");

		static void Warn(const std::string& String, const std::string& Header = "");
		static void Error(const std::string& String, const std::string& Header = "");

		static void OpenGLMessage(const std::string& String, GLenum Source, GLenum Type, GLuint Id, GLenum Severity);

		static std::vector<LogMessage>& GetMessageLog() { return MessageLog; };
	private:
		static std::vector<LogMessage> MessageLog;
	};
}