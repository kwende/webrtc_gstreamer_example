#pragma once
#include <string>

namespace Ocuvera
{
    namespace Chat
    {
        namespace ChatShared
        {
            class ILogger
            {
            public:
                ILogger() {}
                ~ILogger() {}
                virtual void Initialize(std::string relativeLogPath) = 0;
                virtual void LogInfo(std::string message) = 0;
                virtual void LogWarning(std::string message) = 0;
                virtual void LogError(std::string message) = 0;
            };
        }
    }
}