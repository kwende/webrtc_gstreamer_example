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

        public MainPage(IWebRTC webRTC)
        {
            _webRTC = webRTC; 
            InitializeComponent();
        }

        private void Button_Clicked(object sender, EventArgs e)
        {
            _webRTC.SignallingServer = "ws://192.168.1.28:8443";
            _webRTC.CallID = "1";
            _webRTC.StartCall(); 
        }
    }
}
