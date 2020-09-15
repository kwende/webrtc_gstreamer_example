#pragma once
#include <gst/gst.h>
#include <gst/sdp/sdp.h>

#define GST_USE_UNSTABLE_API
#include <gst/webrtc/webrtc.h>

#include "ILogger.h"

namespace Ocuvera
{
    namespace Chat
    {
        namespace ChatShared
        {
            class GStreamerEventLoop
            {
            public:
                static void StartLoop(ILogger* logger);
                static void StopLoop();
            private:
                static GMainLoop* _loop;
            };
        }
    }
}


