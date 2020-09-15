#include "WebSocketClient.h"
#include "AndroidLogger.h"
#include "GStreamerEventLoop.h"

using namespace Ocuvera::Chat::ChatShared;

extern "C"
{
	void Init(onInfoCallback onInfo, onErrorCallback onError); 
	void* Start(char* ip, int port, char* clientName, char* sessionName);
	int Stop(void* handle);
}

AndroidLogger* logger = nullptr;
WebSocketClient *client = nullptr;  

void Init(onInfoCallback onInfo, onErrorCallback onError)
{
	logger = new AndroidLogger(onInfo, onError); 
	logger->LogInfo("Successfully registered callback!");
}

void* Start(char* ip, int port, char* clientName, char* sessionName)
{
	logger->LogInfo("Starting the websocket client"); 
	client = new WebSocketClient("ws://" + std::string(ip) + ":" + std::to_string(port) , clientName, sessionName, logger);
    client->Connect(); 

	logger->LogInfo("Starting the GLib event loop."); 
	GStreamerEventLoop::StartLoop(logger); 
	logger->LogInfo("Broke out of the GLib event loop. Quitting."); 

	return (void*)client; 
}

int Stop(void* handle)
{
	//client->Close();
	return 2;
}