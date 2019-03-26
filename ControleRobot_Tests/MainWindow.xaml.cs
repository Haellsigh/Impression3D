using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Net;

using YaskCombyCerinnov;

namespace ControleRobot_Tests
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        HSEClient clientUDP;

        public MainWindow()
        {
            clientUDP = new HSEClient();
            System.Net.IPAddress.TryParse("192.168.255.1", out clientUDP.IP_robot);

            HSEClient.Status_Information infos;
            clientUDP.StatusInformationRead(out infos);

            InitializeComponent();

            
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            // Rafraichir les données
            
            Label1.Text = "bonjour";
        }
    }
}
