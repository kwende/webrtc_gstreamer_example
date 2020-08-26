using System;

using Android.App;
using Android.Content.PM;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Android.OS;
using Java.Lang;
using Org.Freedesktop.Gstreamer;
using Xamarin.Forms;
using Xamarin.Contracts;
using Android.Systems;
using System.IO;
using Android.Content.Res;

namespace Xamarin.Droid
{
    [Activity(Label = "Xamarin", Icon = "@mipmap/icon", Theme = "@style/MainTheme", MainLauncher = true, ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation)]
    public class MainActivity : global::Xamarin.Forms.Platform.Android.FormsAppCompatActivity
    {
        private IWebRTC _webRtc; 

        protected override void OnCreate(Bundle savedInstanceState)
        {
            TabLayoutResource = Resource.Layout.Tabbar;
            ToolbarResource = Resource.Layout.Toolbar;

            base.OnCreate(savedInstanceState);

            Xamarin.Essentials.Platform.Init(this, savedInstanceState);
            global::Xamarin.Forms.Forms.Init(this, savedInstanceState);

            //Os.Setenv("G_MESSAGES_DEBUG", "all", true); 
            //Os.Setenv("G_DEBUG", "fatal-warnings", true);
            //Os.Setenv("GST_DEBUG", "6", true);
            //Os.Setenv("GST_DEBUG_NO_COLOR", "1", true); 

            JavaSystem.LoadLibrary("gstreamer_android");
            GStreamer.Init(ApplicationContext);

            JavaSystem.LoadLibrary("gstwebrtc");

            //_webRtc = new WebRTCNative(ApplicationContext);
            _webRtc = new WebRTCJava(ApplicationContext);

            //string content = "";
            //AssetManager assets = this.Assets;
            //using (StreamReader sr = new StreamReader(assets.Open("ssl/certs/ca-certificates.crt")))
            //{
            //    content = sr.ReadToEnd();
            //}

            LoadApplication(new App(_webRtc, "")); 
        }
        public override void OnRequestPermissionsResult(int requestCode, string[] permissions, [GeneratedEnum] global::Android.Content.PM.Permission[] grantResults)
        {
            Xamarin.Essentials.Platform.OnRequestPermissionsResult(requestCode, permissions, grantResults);

            base.OnRequestPermissionsResult(requestCode, permissions, grantResults);
        }

        private void RegisterDependencies()
        {
         
        }
    }
}