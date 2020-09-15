#include "GStreamerEventLoop.h"
#include "ILogger.h"

using namespace Ocuvera::Chat::ChatShared;

void GStreamerEventLoop::StartLoop(ILogger* logger)
{
    gst_init(NULL, NULL);
    logger->LogInfo("GStreamerEventLoop::StartLoop: Intialized the GStreamer runtime."); 

    if (_loop == nullptr)
    {
        logger->LogInfo("GStreamerEventLoop::StartLoop: There is no event loop, creating one..."); 
        _loop = g_main_loop_new(NULL, FALSE);
        logger->LogInfo("GStreamerEventLoop::StartLoop: created the loop."); 
        g_main_loop_run(_loop);
        logger->LogInfo("GStreamerEventLoop::StartLoop: breaking out of the loop."); 
    }
}

void GStreamerEventLoop::StopLoop()
{
    if (_loop != nullptr)
    {
        g_main_loop_quit(_loop);
        g_main_loop_unref(_loop);
        _loop = nullptr; 
    }
}

GMainLoop* GStreamerEventLoop::_loop = nullptr;