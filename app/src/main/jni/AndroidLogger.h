#include "ILogger.h"

namespace Ocuvera
{
    namespace Chat
    {
        namespace ChatShared
        {
            typedef int (*onInfoCallback) (const char*); 
            typedef int (*onErrorCallback) (const char*); 

            class AndroidLogger : public ILogger
            {
            public:
                AndroidLogger(onInfoCallback onInfo, onErrorCallback onError)
                {
                    _onError = onError; 
                    _onInfo = onInfo; 
                }
                void Initialize(std::string relativeLogPath){}
                void LogInfo(std::string message)
                {
                    if(_onInfo != nullptr)
                    {
                        _onInfo(message.c_str()); 
                    }
                }
                void LogWarning(std::string message)
                {
                    if(_onInfo != nullptr)
                    {
                        _onInfo(message.c_str()); 
                    }
                }
                void LogError(std::string message)
                {
                    if(_onError != nullptr)
                    {
                        _onError(message.c_str()); 
                    }
                }
            private:
                onErrorCallback _onError; 
                onInfoCallback _onInfo; 
            };
        }
    }
}