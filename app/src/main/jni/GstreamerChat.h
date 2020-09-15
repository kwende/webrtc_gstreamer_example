#pragma once
#include <gst/gst.h>
#include <gst/sdp/sdp.h>

#define GST_USE_UNSTABLE_API
#include <gst/webrtc/webrtc.h>

/* For signalling */
#include <libsoup/soup.h>
#include <json-glib/json-glib.h>

#include <string.h>

#include "Common.h"
#include <functional>
#include <string>

#define RTP_CAPS_OPUS "application/x-rtp,media=audio,encoding-name=OPUS,payload=100"
#define RTP_CAPS_VP8 "application/x-rtp,media=video,encoding-name=VP8,payload=101"


#include <ILogger.h>

namespace Ocuvera
{
    namespace Chat
    {
        namespace ChatShared
        {
            class GStreamerChat
            {
            public:
                GStreamerChat(std::function<void(std::string)> callback, ILogger* logger);
                void StartProcessing(NegotiateType negotiateType);
                void HandleOfferFromPeer(GstSDPMessage* sdp);
                void HandleAnswerFromPeer(GstSDPMessage* sdp);
                void HandleICECandidate(std::string ice, int sdpmLineIndex);
            private:
                GstElement* _pipeline, * _webRtcElement;
                NegotiateType _negotiateType;
                std::function<void(std::string)> _onSendMessageCallback;
                ILogger* _logger; 

                static void onPipelineNeedsNegotiation(GstElement* element, gpointer user_data);
                static void onPipelineNeedsICECandidates(GstElement* webrtc G_GNUC_UNUSED, guint mlineindex,
                    gchar* candidate, gpointer user_data G_GNUC_UNUSED);
                static void onPipelineReceviedStream(GstElement* webrtc, GstPad* pad, gpointer user_data); 
                static void onPipelineHasCreatedOffer(GstPromise* promise, gpointer user_data);
                void sendSessionDescriptionToPeers(GstWebRTCSessionDescription* desc);
                static void onPipelineRemoteDescriptionSet(GstPromise* promise, gpointer user_data);
                static void onPipelineHasCreatedAnswer(GstPromise* promise, gpointer user_data);
                static void onPipelineDecodebinPadAdded(GstElement* decodebin, GstPad* pad, gpointer user_data); 
            };
        }
    }
}

