#pragma once

namespace Ocuvera
{
    namespace Chat
    {
        namespace ChatShared
        {
            enum MessageTypeEnum
            {
                HelloResponse,
                SessionResponse,
                SDP,
                ICE,
                Unknown,
            };

            enum class NegotiateType
            {
                Unknown,
                Initiates,
                Waits,
            };

            static const char* webSocketStateNames[] = {
                "NEW", "CONNECTING", "IDLE", "IN_USE",
                "REMOTE_DISCONNECTED", "DISCONNECTED"
            };
        }
    }
}
