using System;
using Xamarin.Contracts;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace Xamarin
{
    public partial class App : Application
    {
        public App(IWebRTC webRTC)
        {
            InitializeComponent();

            MainPage = new MainPage(webRTC);
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
