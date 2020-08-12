using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Java.Interop;
using Java.Lang;
using Org.Freedesktop.Gstreamer;
using Xamarin.Contracts;
using Xamarin.Droid;

namespace Xamarin
{
    public class WebRTC : Java.Lang.Object, IWebRTC
    {
        private string _signallingServer;
        private Surface _surface;
        private string _callID; 

        public long native_webrtc; 

        public WebRTC(Context context)
        {
            JavaSystem.LoadLibrary("gstreamer_android");
            GStreamer.Init(context);

            JavaSystem.LoadLibrary("gstwebrtc");

            //IntPtr klass = JNIEnv.FindClass(typeof(WebRTC)); 
            //NativeMethods.NativeClassInit(JNIEnv.Handle, klass);

            NativeMethods.NativeNew(JNIEnv.Handle, this.Handle);
        }

        public void StartCall()
        {
            NativeMethods.NativeCallOtherParty(JNIEnv.Handle, this.Handle); 
        }

        public void EndCall()
        {
            NativeMethods.NativeEndCall(JNIEnv.Handle, this.Handle); 
        }

        public string SignallingServer
        {
            get
            {
                return _signallingServer; 
            }
            set
            {
                _signallingServer = value;
                NativeMethods.NativeSetSignallingServer(
                    JNIEnv.Handle, 
                    this.Handle, 
                    new Java.Lang.String(_signallingServer).Handle); 
            }
        }

        public string CallID
        {
            get
            {
                return _callID; 
            }
            set
            {
                _callID = value;
                NativeMethods.NativeSetCallId(
                    JNIEnv.Handle,
                    this.Handle,
                    new Java.Lang.String(_callID).Handle); 
            }
        }

        public Surface Surface
        {
            get
            {
                return _surface; 
            }
            set
            {
                _surface = value;
                NativeMethods.NativeSetSurface(
                    JNIEnv.Handle,
                    this.Handle,
                    _surface.Handle); 
            }
        }

        public void Close()
        {
            NativeMethods.NativeFree(JNIEnv.Handle, this.Handle); 
        }
    }
}   