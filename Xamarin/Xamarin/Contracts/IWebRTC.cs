using System;
using System.Collections.Generic;
using System.Text;

namespace Xamarin.Contracts
{
    public interface IWebRTC
    {
        string SignallingServer { get; set; }
        string CallID { get; set; }
        void StartCall();
        void EndCall();
    }
}
