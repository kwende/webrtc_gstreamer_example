//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;

//using Android.App;
//using Android.Content;
//using Android.OS;
//using Android.Runtime;
//using Android.Views;
//using Android.Widget;
//using Java.Interop;
//using Java.Lang;
//using Org.Freedesktop.Gstreamer;
//using Xamarin.Contracts;
//using Xamarin.Droid;

//namespace Xamarin
//{
//    public class WebRTCJava : Java.Lang.Object, IWebRTC
//    {
//        private WebRTC _webRtc; 
//        private string _signallingServer;
//        private Surface _surface;
//        private string _callID; 

//        public long native_webrtc; 

//        public WebRTCJava(Context context)
//        {
//            WebRTC.Init(context); 
//            _webRtc = new WebRTC(); 
//        }

//        public void StartCall()
//        {
//            _webRtc.CallOtherParty(); 
//        }

//        public void EndCall()
//        {
//            _webRtc.EndCall(); 
//        }

//        public string SignallingServer
//        {
//            get
//            {
//                return _webRtc.SignallingServer; 
//            }
//            set
//            {
//                _webRtc.SignallingServer = value; 
//            }
//        }

//        public string CallID
//        {
//            get
//            {
//                return _webRtc.CallID; 
//            }
//            set
//            {
//                _webRtc.CallID = value; 
//            }
//        }

//        public Surface Surface
//        {
//            get
//            {
//                return _webRtc.Surface; 
//            }
//            set
//            {
//                _webRtc.Surface = value; 
//            }
//        }

//        public void Close()
//        {
//            _webRtc.Close(); 
//        }
//    }
//}   