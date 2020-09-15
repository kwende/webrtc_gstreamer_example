#pragma once
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>

#include <string>
#include <functional>

#include "GstreamerChat.h"
#include "Common.h"
#include <memory>

#include "ILogger.h"

namespace Ocuvera
{
    namespace Chat
    {
        namespace ChatShared
        {
            class WebSocketClient
            {
            public:
                WebSocketClient(std::string uri, std::string clientName, std::string sessionName, ILogger* logger);
                ~WebSocketClient();
                void Connect();
                void SendText(std::string text);
                void RegisterOnMessageCallback(std::function<void(MessageTypeEnum, std::string)>);
                void Close();

            private:
                SoupLogger* _soupLogger;
                SoupConnectionState _state;
                SoupWebsocketConnection* _connection;
                SoupSession* _session;
                std::string _uri;
                NegotiateType _negotiateType;
                GStreamerChat* _gstreamer;
                std::string _clientName;
                std::string _sessionName; 
                ILogger* _logger; 

                static void onMessageReceived(SoupWebsocketConnection* ws, SoupWebsocketDataType type, GBytes* message, gpointer user_data);
                static void onConnectionClosed(SoupWebsocketConnection* ws, gpointer user_data);
                static void onStateChanged(GObject* object, GAsyncResult* result, gpointer user_data);
                static void onConnectionCreated(SoupSession* session, GObject* conn, gpointer user_data);
                static void onConnectionComplete(GObject* object, GAsyncResult* result, gpointer user_data);
                static gboolean onNeedKeepAlive(gpointer user_data);
            };
        }
    }
}

