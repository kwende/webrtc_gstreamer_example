using System;
using Xamarin.Contracts;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Xamarin
{
    public partial class App : Application
    {
        public App(IWebRTC webRTC, string locale)
        {
            InitializeComponent();

            MainPage = new MainPage(webRTC, locale);
        }

        protected override void OnStart()
        {
        }

        protected override void OnSleep()
        {
        }

        protected override void OnResume()
        {
        }
    }
}
