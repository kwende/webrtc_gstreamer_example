#include "GstreamerChat.h"
#include <vector>
#include <string>
#include <sstream>

using namespace Ocuvera::Chat::ChatShared;

//#define DEBUG_AUDIO_SOURCE

GStreamerChat::GStreamerChat(std::function<void(std::string)> callback, ILogger* logger)
{
    _logger = logger;
    _pipeline = nullptr;
    _webRtcElement = nullptr;
    _negotiateType = NegotiateType::Unknown;
    _onSendMessageCallback = callback;
}

/// <summary>
/// Invoked when the Gstreamer WebRTC pipeline has successfully generated a
/// reponse to the offer handed to us by the websocket client through HandleOfferfromPeer. 
/// Need to take this response and generate SDP to send as well as register it officially with our pipeline.
/// </summary>
/// <param name="promise"></param>
/// <param name="user_data"></param>
void GStreamerChat::onPipelineHasCreatedAnswer(GstPromise* promise, gpointer user_data)
{
    GStreamerChat* dis = reinterpret_cast<GStreamerChat*>(user_data);
    dis->_logger->LogInfo("GStreamerChat::onAnswerCreated\n");
    GstWebRTCSessionDescription* answer = NULL;
    const GstStructure* reply;

    g_assert_cmphex(gst_promise_wait(promise), == , GST_PROMISE_RESULT_REPLIED);
    reply = gst_promise_get_reply(promise);
    gst_structure_get(reply, "answer",
        GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &answer, NULL);
    gst_promise_unref(promise);

    promise = gst_promise_new();
    g_signal_emit_by_name(dis->_webRtcElement, "set-local-description", answer, promise);
    gst_promise_interrupt(promise);
    gst_promise_unref(promise);

    /* Send answer to peer */
    dis->sendSessionDescriptionToPeers(answer);
    gst_webrtc_session_description_free(answer);
}

/// <summary>
/// Invoked when the gstreamer pipeline has fully integrated and registered the 
/// WebRTC offer sent to us by a peer and handed to this class by HandleOfferFromPeer
/// </summary>
/// <param name="promise"></param>
/// <param name="user_data"></param>
void GStreamerChat::onPipelineRemoteDescriptionSet(GstPromise* promise, gpointer user_data)
{
    GStreamerChat* dis = reinterpret_cast<GStreamerChat*>(user_data);
    dis->_logger->LogInfo("GStreamerChat::onRemoteDescriptionSet\n");
    gst_promise_unref(promise);
    promise = gst_promise_new_with_change_func(onPipelineHasCreatedAnswer, dis, NULL);
    g_signal_emit_by_name(dis->_webRtcElement, "create-answer", NULL, promise);
}

/// <summary>
/// Invoked when an SDP message has been received by the websocket client that indicates a
/// WebRTC offer has been sent to this pipeline from a peer. 
/// </summary>
/// <param name="sdp"></param>
void GStreamerChat::HandleOfferFromPeer(GstSDPMessage* sdp)
{
    _logger->LogInfo("GStreamerChat::HandleOfferFromPeer\n");
    GstWebRTCSessionDescription* offer = NULL;
    GstPromise* promise;

    offer = gst_webrtc_session_description_new(GST_WEBRTC_SDP_TYPE_OFFER, sdp);
    g_assert_nonnull(offer);

    /* Set remote description on our pipeline */
    promise = gst_promise_new_with_change_func(onPipelineRemoteDescriptionSet, this, NULL);
    g_signal_emit_by_name(_webRtcElement, "set-remote-description", offer, promise);
    gst_webrtc_session_description_free(offer);
}

/// <summary>
/// A utility method for packaging up the session descrption object into text SDP message
/// and invoking the callback to the web socket implementation to send it. 
/// </summary>
/// <param name="desc"></param>
void GStreamerChat::sendSessionDescriptionToPeers(GstWebRTCSessionDescription* desc)
{
    _logger->LogInfo("GStreamerChat::sendSessionDescriptionToPeers\n");
    gchar* text;
    JsonObject* msg, * sdp;

    text = gst_sdp_message_as_text(desc->sdp);
    sdp = json_object_new();

    if (desc->type == GST_WEBRTC_SDP_TYPE_OFFER) {
        //g_print ("Sending offer:\n%s\n", text);
        json_object_set_string_member(sdp, "type", "offer");
    }
    else if (desc->type == GST_WEBRTC_SDP_TYPE_ANSWER) {
        //g_print ("Sending answer:\n%s\n", text);
        json_object_set_string_member(sdp, "type", "answer");
    }
    else {
        g_assert_not_reached();
    }

    json_object_set_string_member(sdp, "sdp", text);
    g_free(text);

    msg = json_object_new();
    json_object_set_object_member(msg, "sdp", sdp);

    JsonNode* root;
    JsonGenerator* generator;

    /* Make it the root node */
    root = json_node_init_object(json_node_alloc(), msg);
    generator = json_generator_new();
    json_generator_set_root(generator, root);
    text = json_generator_to_data(generator, NULL);

    /* Release everything */
    g_object_unref(generator);
    json_node_free(root);
    json_object_unref(msg);

    // invoke the callback. 
    _onSendMessageCallback(std::string(text));

    g_free(text);
}

void GStreamerChat::HandleAnswerFromPeer(GstSDPMessage* sdp)
{
    _logger->LogInfo("GStreamerChat::HandleAnswerFromPeer\n");
    GstWebRTCSessionDescription* answer = gst_webrtc_session_description_new(GST_WEBRTC_SDP_TYPE_ANSWER,
        sdp);
    g_assert_nonnull(answer);

    /* Set remote description on our pipeline */
    GstPromise* promise = gst_promise_new();
    g_signal_emit_by_name(_webRtcElement, "set-remote-description", answer,
        promise);
    gst_promise_interrupt(promise);
    gst_promise_unref(promise);
}

/// <summary>
/// Fires when offer has been created by the GStreamer WebRTC pipeline that needs to be encased in SDP
/// and transmitted to the peer(s) via the signalling server.
/// </summary>
/// <param name="promise">The promise to be encapsulated.</param>
/// <param name="user_data">The this pointer</param>
void GStreamerChat::onPipelineHasCreatedOffer(GstPromise* promise, gpointer user_data)
{
    GStreamerChat* dis = reinterpret_cast<GStreamerChat*>(user_data);
    dis->_logger->LogInfo("GStreamerChat::onOfferCreated\n");
    GstWebRTCSessionDescription* offer = NULL;
    const GstStructure* reply;

    reply = gst_promise_get_reply(promise);
    gst_structure_get(reply, "offer",
        GST_TYPE_WEBRTC_SESSION_DESCRIPTION, &offer, NULL);
    gst_promise_unref(promise);

    promise = gst_promise_new();
    g_signal_emit_by_name(dis->_webRtcElement, "set-local-description", offer, promise);
    gst_promise_interrupt(promise);
    gst_promise_unref(promise);

    /* Send offer to peer */
    dis->sendSessionDescriptionToPeers(offer);
    gst_webrtc_session_description_free(offer);
}

void GStreamerChat::onPipelineNeedsNegotiation(GstElement* element, gpointer user_data)
{
    GStreamerChat* dis = reinterpret_cast<GStreamerChat*>(user_data);
    dis->_logger->LogInfo("GStreamerChat::onNegotitationNeeded.\n");

    switch (dis->_negotiateType)
    {
    case NegotiateType::Initiates:
    {
        g_print("\tI'm starting the negotiation process.\n");
        GstPromise* promise;
        // register my callback (see below)
        promise = gst_promise_new_with_change_func(onPipelineHasCreatedOffer, user_data, NULL);
        // tell the GStreamer pipeline to build an offer of its capabilities and generate
        // a promise that can be transmitted to the peers via SDP and the signalling server
        // by invoking onOfferCrated
        g_signal_emit_by_name(dis->_webRtcElement, "create-offer", NULL, promise);
    }
    break;
    case NegotiateType::Waits:
        g_print("\tI'm awaiting negotation from the peer\n");
        break;
    }
}

void GStreamerChat::HandleICECandidate(std::string ice, int sdpmLineIndex)
{
    _logger->LogInfo("GStreamerChat:HandleICECandidate");
    /* Add ice candidate sent by remote peer */
    g_signal_emit_by_name(this->_webRtcElement, "add-ice-candidate", sdpmLineIndex, ice.c_str());
}

void GStreamerChat::onPipelineNeedsICECandidates(GstElement* webrtc G_GNUC_UNUSED, guint mlineindex,
    gchar* candidate, gpointer user_data)
{
    GStreamerChat* dis = reinterpret_cast<GStreamerChat*>(user_data);
    dis->_logger->LogInfo("GStreamerChat:onSendICECandidateMessageNeeded\n");

    GstElement* ice;
    g_object_get(webrtc, "ice-agent", &ice, NULL);

    if (strstr(candidate, "TCP") == NULL)
    {
        g_print("\tStarting with %s\n", candidate);
        GStreamerChat* dis = reinterpret_cast<GStreamerChat*>(user_data);


        gchar* text;
        JsonObject* ice, * msg;

        ice = json_object_new();
        json_object_set_string_member(ice, "candidate", candidate);
        json_object_set_int_member(ice, "sdpMLineIndex", mlineindex);
        msg = json_object_new();
        json_object_set_object_member(msg, "ice", ice);

        JsonNode* root;
        JsonGenerator* generator;

        /* Make it the root node */
        root = json_node_init_object(json_node_alloc(), msg);
        generator = json_generator_new();
        json_generator_set_root(generator, root);
        text = json_generator_to_data(generator, NULL);

        /* Release everything */
        g_object_unref(generator);
        json_node_free(root);

        json_object_unref(msg);


        dis->_onSendMessageCallback(std::string(text));
        g_free(text);
    }
}

/// <summary>
/// Fired when by decodebin in the pipeline when it has figured out what's going 
/// on with this stream using its automagic.  
/// </summary>
/// <param name="decodebin"></param>
/// <param name="pad"></param>
/// <param name="pipe"></param>
void GStreamerChat::onPipelineDecodebinPadAdded(GstElement* decodebin, GstPad* pad, gpointer user_data)
{
    GStreamerChat* dis = reinterpret_cast<GStreamerChat*>(user_data);
    dis->_logger->LogInfo("GStreamerChat::onPipelineDecodebinPadAdded\n");
    GstCaps* caps;
    const gchar* name;

    if (!gst_pad_has_current_caps(pad)) {
        g_printerr("Pad '%s' has no caps, can't do anything, ignoring\n",
            GST_PAD_NAME(pad));
        return;
    }

    caps = gst_pad_get_current_caps(pad);
    name = gst_structure_get_name(gst_caps_get_structure(caps, 0));

    if (g_str_has_prefix(name, "video"))
    {
        //handle_media_stream(pad, pipe, "videoconvert", "autovideosink");
    }
    else if (g_str_has_prefix(name, "audio"))
    {
        g_print("\tGStreamerChat::onPipelineDecodebinAdded: handling audio.\n");
        GstPad* qpad;
        GstElement* q, * conv, * resample, * sink;
        GstPadLinkReturn ret;

        q = gst_element_factory_make("queue", NULL);
        g_assert_nonnull(q);
        conv = gst_element_factory_make("audioconvert", NULL);
        g_assert_nonnull(conv);
        sink = gst_element_factory_make("autoaudiosink", NULL);
        g_assert_nonnull(sink);

        /* Might also need to resample, so add it just in case.
            * Will be a no-op if it's not required. */
        resample = gst_element_factory_make("audioresample", NULL);
        g_assert_nonnull(resample);
        gst_bin_add_many(GST_BIN(dis->_pipeline), q, conv, resample, sink, NULL);
        gst_element_sync_state_with_parent(q);
        gst_element_sync_state_with_parent(conv);
        gst_element_sync_state_with_parent(resample);
        gst_element_sync_state_with_parent(sink);
        gst_element_link_many(q, conv, resample, sink, NULL);

        qpad = gst_element_get_static_pad(q, "sink");

        ret = gst_pad_link(pad, qpad);
        g_assert_cmphex(ret, == , GST_PAD_LINK_OK);
    }
    else
    {
        g_printerr("Unknown pad %s, ignoring", GST_PAD_NAME(pad));
    }
}

/// <summary>
/// Fired when the pipeline has begun receiving stream data from a peer. 
/// </summary>
/// <param name="webrtc"></param>
/// <param name="pad"></param>
/// <param name="pipe"></param>
void GStreamerChat::onPipelineReceviedStream(GstElement* webrtc, GstPad* pad, gpointer user_data)
{
    GStreamerChat* dis = reinterpret_cast<GStreamerChat*>(user_data);
    dis->_logger->LogInfo("GStreamerChat::onPipelineReceivedStream\n");

    GstElement* decodebin;
    GstPad* sinkpad;

    if (GST_PAD_DIRECTION(pad) != GST_PAD_SRC)
        return;

    decodebin = gst_element_factory_make("decodebin", NULL);
    g_signal_connect(decodebin, "pad-added",
        G_CALLBACK(onPipelineDecodebinPadAdded), dis);
    gst_bin_add(GST_BIN(dis->_pipeline), decodebin);
    gst_element_sync_state_with_parent(decodebin);

    sinkpad = gst_element_get_static_pad(decodebin, "sink");
    gst_pad_link(pad, sinkpad);
    gst_object_unref(sinkpad);
}

void GStreamerChat::StartProcessing(NegotiateType negotiateType)
{
    _logger->LogInfo("GStreamerChat::StartProcess\n");
    _negotiateType = negotiateType;

    GstStateChangeReturn ret;
    GError* error = NULL;

#ifdef DEBUG_AUDIO_SOURCE
    _pipeline = gst_parse_launch("webrtcbin bundle-policy=max-bundle name=sendrecv "
        "audiotestsrc is-live=true wave=red-noise ! audioconvert ! audioresample ! queue ! opusenc ! rtpopuspay ! "
        "queue ! " RTP_CAPS_OPUS "97 ! sendrecv. ", &error);
#elif _WIN32 
    _pipeline = gst_parse_launch("webrtcbin bundle-policy=max-bundle name=sendrecv "
        "autoaudiosrc ! audioconvert ! audioresample ! queue ! opusenc ! rtpopuspay ! "
        "queue ! " RTP_CAPS_OPUS "97 ! sendrecv. ", &error);
#else

    _pipeline = gst_parse_launch("webrtcbin name=sendrecv "
        //"audiotestsrc ! audioconvert ! audioresample ! queue ! opusenc ! rtpopuspay ! "
        "openslessrc ! queue ! audioconvert ! audioresample ! audiorate ! queue ! opusenc ! rtpopuspay ! "
        "queue ! " RTP_CAPS_OPUS " ! sendrecv. ", &error);
#endif

    if (!error)
    {
        _webRtcElement = gst_bin_get_by_name(GST_BIN(_pipeline), "sendrecv");
        if (_webRtcElement)
        {
            /* This is the gstwebrtc entry point where we create the offer and so on. It
            * will be called when the pipeline goes to PLAYING. */
            g_signal_connect(_webRtcElement, "on-negotiation-needed",
                G_CALLBACK(onPipelineNeedsNegotiation), this);
            /* We need to transmit this ICE candidate to the browser via the websockets
            * signalling server. Incoming ice candidates from the browser need to be
            * added by us too, see on_server_message() */
            g_signal_connect(_webRtcElement, "on-ice-candidate",
                G_CALLBACK(onPipelineNeedsICECandidates), this);

            gst_element_set_state(_pipeline, GST_STATE_READY);

            /* Incoming streams will be exposed via this signal */
            g_signal_connect(_webRtcElement, "pad-added", G_CALLBACK(onPipelineReceviedStream), this);
            /* Lifetime is the same as the pipeline itself */
            //gst_object_unref(webRtc);

            ret = gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);

            //GstElement* value;
            //g_object_get(_webRtcElement, "ice-agent", &value);

            GValue value = { 0, };
            g_value_init(&value, G_TYPE_OBJECT);
            g_object_get_property(G_OBJECT(_webRtcElement), "ice-agent", &value);
            GObject* object = (GObject*)g_value_get_object(&value);
        }
    }
    else
    {
        g_print("\tGStreamerChat::StartProcessing: failed to start processing due to %s\n", error->message);
    }
}