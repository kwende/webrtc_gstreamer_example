#include "WebSocketClient.h"

using namespace Ocuvera::Chat::ChatShared;

WebSocketClient::WebSocketClient(std::string uri, std::string clientName, std::string sessionName, ILogger* logger)
{
    _logger = logger;
    _clientName = clientName;
    _sessionName = sessionName;
    _uri = uri;
    _gstreamer = new GStreamerChat([this](std::string message) ->
        void
        {
            if (_connection != nullptr)
            {
                soup_websocket_connection_send_text(_connection, message.c_str());
            }
        }, logger);
}

WebSocketClient::~WebSocketClient()
{

}

gboolean WebSocketClient::onNeedKeepAlive(gpointer user_data)
{
    WebSocketClient* dis = reinterpret_cast<WebSocketClient*>(user_data);
    //dis->_logger->LogInfo("WebSocketClient::onKeepAlive: sending keep alive statement to server.");
    soup_websocket_connection_send_text(dis->_connection, "PING");
    return TRUE;
}

void WebSocketClient::onStateChanged(GObject* object, GAsyncResult* result, gpointer user_data)
{
    WebSocketClient* dis = reinterpret_cast<WebSocketClient*>(user_data);

    // book keeping for keeping track of the current connections's state. 
    SoupConnectionState newState;
    g_object_get(object, "state", &newState, NULL);
    dis->_logger->LogInfo(std::string(
        "WebSocketClient::onStateChanged from " +
        std::string(webSocketStateNames[dis->_state]) + " to " +
        std::string(webSocketStateNames[newState])));
    dis->_state = newState;
}

/// <summary>
/// Invoked whenever the websocket client receives a message from the server endpoint. 
/// </summary>
/// <param name="ws">SoupWebsocketConnection object</param>
/// <param name="type">Message data type</param>
/// <param name="message">The message</param>
/// <param name="user_data">The this pointer</param>
void WebSocketClient::onMessageReceived(SoupWebsocketConnection* ws, SoupWebsocketDataType type, GBytes* message, gpointer user_data)
{
    // we only care if the data from the server is text, we're not handling binary data. 
    if (type == SOUP_WEBSOCKET_DATA_TEXT)
    {
        WebSocketClient* dis = reinterpret_cast<WebSocketClient*>(user_data);

        gsize len;
        const char* contents = (const char*)g_bytes_get_data(message, &len);

        if (strcmp(contents, "HELLO") == 0)
        {
            dis->_logger->LogInfo("WebSocketClient::onMessageReceived: Received hello. Can create session.\n");
            soup_websocket_connection_send_text(dis->_connection, std::string("SESSION " + dis->_sessionName).c_str());
            g_timeout_add_seconds(3, onNeedKeepAlive, dis);
        }
        else if (strcmp(contents, "SESSION_OK") == 0)
        {
            dis->_logger->LogInfo("WebSocketClient::onMessageReceived: Received SESSION_OK. I'm in a session\n");
        }
        else if (strcmp(contents, "START_NEGOTIATE") == 0)
        {
            dis->_logger->LogInfo("WebSocketClient::onMessageReceived: Received START_NEGOTIATE. Spinning up the pipeline...\n");
            dis->_negotiateType = NegotiateType::Initiates;
            dis->_gstreamer->StartProcessing(dis->_negotiateType);
        }
        else if (strcmp(contents, "AWAIT_NEGOTIATE") == 0)
        {
            dis->_logger->LogInfo("WebSocketClient::onMessageReceived: Received AWAIT_NEGOTIATE. I'm spinning up the pipeline...\n");
            dis->_negotiateType = NegotiateType::Waits;
            dis->_gstreamer->StartProcessing(dis->_negotiateType);
        }
        else if (strstr(contents, "SERVER_RESPONSE"))
        {
            dis->_logger->LogInfo(std::string("WebSocketClient::onMessageReceived: Received SERVER_RESPONSE: " + std::string(contents)));
        }
        else if (strstr(contents, "{") != NULL)
        {
            gsize size;
            const gchar* data = (const gchar*)g_bytes_get_data(message, &size);
            gchar* text = g_strndup(data, size);

            JsonNode* root;
            JsonObject* object, * child;
            JsonParser* parser = json_parser_new();
            if (json_parser_load_from_data(parser, text, -1, NULL))
            {
                root = json_parser_get_root(parser);
                if (JSON_NODE_HOLDS_OBJECT(root))
                {
                    object = json_node_get_object(root);
                    /* Check type of JSON message */
                    if (json_object_has_member(object, "sdp"))
                    {
                        dis->_logger->LogInfo("WebSocketClient::onMessageReceived: Received SDP message in JSON format.\n");
                        int ret;
                        GstSDPMessage* sdp;
                        const gchar* text, * sdptype;
                        GstWebRTCSessionDescription* answer;

                        child = json_object_get_object_member(object, "sdp");

                        sdptype = json_object_get_string_member(child, "type");
                        /* In this example, we create the offer and receive one answer by default,
                         * but it's possible to comment out the offer creation and wait for an offer
                         * instead, so we handle either here.
                         *
                         * See tests/examples/webrtcbidirectional.c in gst-plugins-bad for another
                         * example how to handle offers from peers and reply with answers using webrtcbin. */
                        text = json_object_get_string_member(child, "sdp");
                        ret = gst_sdp_message_new(&sdp);
                        ret = gst_sdp_message_parse_buffer((guint8*)text, strlen(text), sdp);

                        if (g_str_equal(sdptype, "answer")) {
                            // call down to gstreamer
                            dis->_gstreamer->HandleAnswerFromPeer(sdp);
                        }
                        else {
                            // call down to gstreamer. 
                            dis->_gstreamer->HandleOfferFromPeer(sdp);
                        }

                    }
                    else if (json_object_has_member(object, "ice"))
                    {
                        dis->_logger->LogInfo("WebSocketClient::onMessageReceived: Received ICE message in JSON format.\n");
                        //g_print(contents); 
                        const gchar* candidate;
                        gint sdpmlineindex;

                        child = json_object_get_object_member(object, "ice");
                        candidate = json_object_get_string_member(child, "candidate");
                        sdpmlineindex = json_object_get_int_member(child, "sdpMLineIndex");

                        dis->_gstreamer->HandleICECandidate(candidate, sdpmlineindex);
                    }
                    else
                    {
                        dis->_logger->LogWarning(std::string("Ignoring unknown JSON message:") + std::string(text));
                    }
                    g_object_unref(parser);
                }
            }
        }
    }
}

/// <summary>
/// Invoked when the websocket's connection to the server endpoint is terminated. 
/// </summary>
/// <param name="ws">SoupWebSocketConnection object.</param>
/// <param name="user_data">this pointer</param>
void WebSocketClient::onConnectionClosed(SoupWebsocketConnection* ws, gpointer user_data)
{
    WebSocketClient* dis = reinterpret_cast<WebSocketClient*>(user_data);

    dis->_logger->LogInfo("WebSocketClient::onConnectionClosed.\n");
}

/// <summary>
/// Gets invoked when a connection has been successfully made to a websocket end point. Response for subsequently
/// registering for the notify::state event so the pipeline can be notified of state changes. 
/// </summary>
/// <param name="session">SoupSession object</param>
/// <param name="conn">connection object</param>
/// <param name="user_data">Passed the 'this' pointer</param>
void WebSocketClient::onConnectionCreated(SoupSession* session, GObject* conn, gpointer user_data)
{
    WebSocketClient* dis = reinterpret_cast<WebSocketClient*>(user_data);
    dis->_logger->LogInfo("WebSocketClient::onConnectionCreated.\n");


    //https://developer.gnome.org/gobject/stable/gobject-The-Base-Object-Type.html#g-object-get
    // pulls the state object from the connection object and assigns it to dis->_state
    g_object_get(conn, "state", &dis->_state, NULL);

    //https://developer.gnome.org/gobject/stable/gobject-Signals.html#g-signal-connect
    // with our new state object, register for the notify state change event on it.
    g_signal_connect(conn, "notify::state",
        G_CALLBACK(onStateChanged),
        dis);
}

void WebSocketClient::onConnectionComplete(GObject* object, GAsyncResult* result, gpointer user_data)
{
    WebSocketClient* dis = reinterpret_cast<WebSocketClient*>(user_data);
    dis->_logger->LogInfo("WebSocketClient::onConnectionComplete.\n");
    GError* error = NULL;

    dis->_connection = soup_session_websocket_connect_finish(
        SOUP_SESSION(object),
        result,
        &error);

    if (error != NULL) {
        dis->_logger->LogError(error->message);
        g_error_free(error);
    }
    else
    {
        if (dis->_connection) {
            g_signal_connect(dis->_connection, "closed", G_CALLBACK(onConnectionClosed), dis);
            g_signal_connect(dis->_connection, "message", G_CALLBACK(onMessageReceived), dis);

            soup_websocket_connection_send_text(dis->_connection, std::string("HELLO " + dis->_clientName).c_str());
        }
    }
}

void WebSocketClient::Connect()
{
    SoupMessage* msg;
    _session = soup_session_new();

    g_object_set(G_OBJECT(_session),
        SOUP_SESSION_SSL_STRICT, FALSE,
        NULL);

    g_signal_connect(_session, "connection-created",
        G_CALLBACK(onConnectionCreated), this);

    msg = soup_message_new(SOUP_METHOD_GET, _uri.c_str());

    soup_session_websocket_connect_async(_session,
        msg,
        NULL,
        NULL,
        NULL,
        onConnectionComplete,
        this);
}

