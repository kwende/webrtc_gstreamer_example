using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

namespace Xamarin.Droid
{
    public static class NativeMethods
    {
        // NOTE: I think in terms of the JNI, the "thiz" pointer is essentially 
        // used as a key into some tables used to hold resources on a per-java-object 
        // basis in the JNIEnv object. Thus enabling multiple instances in Java to be backed
        // by resources kept in the JNIEnv and to have those resources touchable by "instance" methods below
        // when interoperability with native code is necessary. 
        // Nothing makes this more obvious than the fact the instance data passed to these methods 
        // (such as the URLs and whatnot) are set on the JNIEnv by SetLongField and retrieved from it
        // by GetLongField, both of which require the "thiz" pointer as likely lookup key. 

        [DllImport("libgstwebrtc.so", EntryPoint = "native_new")]
        public static extern void NativeNew(IntPtr env, IntPtr thiz);

        [DllImport("libgstwebrtc.so", EntryPoint = "native_free")]
        public static extern void NativeFree(IntPtr env, IntPtr thiz);

        [DllImport("libgstwebrtc.so", EntryPoint = "native_class_init")]
        public static extern void NativeClassInit(IntPtr env, IntPtr thiz);

        [DllImport("libgstwebrtc.so", EntryPoint = "native_set_surface")]
        public static extern void NativeSetSurface(IntPtr env, IntPtr thiz, IntPtr surface);

        //https://stackoverflow.com/questions/47433520/load-so-file-in-xamarin-android
        //https://forums.xamarin.com/discussion/91301/get-a-string-from-a-c-library-with-a-java-wrapper-around-it
        [DllImport("libgstwebrtc.so", EntryPoint = "native_set_signalling_server")]
        public static extern void NativeSetSignallingServer(IntPtr env, IntPtr thiz, IntPtr server);

        [DllImport("libgstwebrtc.so", EntryPoint = "native_set_call_id")]
        public static extern void NativeSetCallId(IntPtr env, IntPtr thiz, IntPtr peer_id);

        [DllImport("libgstwebrtc.so", EntryPoint = "native_call_other_party")]
        public static extern void NativeCallOtherParty(IntPtr env, IntPtr thiz);

        [DllImport("libgstwebrtc.so", EntryPoint = "native_end_call")]
        public static extern void NativeEndCall(IntPtr env, IntPtr thiz); 
    }
}