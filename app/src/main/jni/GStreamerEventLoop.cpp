#include "GStreamerEventLoop.h"

using namespace Ocuvera::Chat::ChatShared;

void GStreamerEventLoop::StartLoop()
{
    gst_init(NULL, NULL);

    if (_loop == nullptr)
    {
        _loop = g_main_loop_new(NULL, FALSE);
        g_main_loop_run(_loop);
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