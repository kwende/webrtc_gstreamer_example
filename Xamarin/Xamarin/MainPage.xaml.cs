using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Contracts;
using Xamarin.Forms;

namespace Xamarin
{
    // Learn more about making custom code visible in the Xamarin.Forms previewer
    // by visiting https://aka.ms/xamarinforms-previewer
    [DesignTimeVisible(false)]
    public partial class MainPage : ContentPage
    {
        IWebRTC _webRTC;
        string _locale; 

        public MainPage(IWebRTC webRTC, string locale)
        {
            _locale = locale; 
            _webRTC = webRTC; 
            InitializeComponent();
        }

        private async void Button_Clicked(object sender, EventArgs e)
        {
            await DisplayAlert("Locale", _locale, "Cancel"); 

            _webRTC.SignallingServer = "ws://192.168.1.28:8443";
            _webRTC.CallID = "1";
            _webRTC.StartCall(); 
        }
    }
}
