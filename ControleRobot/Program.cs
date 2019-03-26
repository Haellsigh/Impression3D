using System;
using System.Threading;
using YaskCombyCerinnov;

namespace ControleRobot
{
    class Program
    {
        static void Main(string[] args)
        {
            HSEClient clientUDP = new HSEClient();
            Console.WriteLine("bonjour cher ami");
            System.Net.IPAddress.TryParse("192.168.255.1", out clientUDP.IP_robot);

            while (true)
            {
                Console.Clear();
                HSEClient.Status_Information infos;
                clientUDP.StatusInformationRead(out infos);
                Console.WriteLine("Servo : " + infos.ServoON.ToString());
                Console.WriteLine("Alarming : " + infos.Alarming.ToString());
                Console.WriteLine("AutomaticAndContinous : " + infos.AutomaticAndContinous.ToString());
                Console.WriteLine("CommandRemote : " + infos.CommandRemote.ToString());
                Console.WriteLine("Cycle1 : " + infos.Cycle1.ToString());
                Console.WriteLine("ErrorOccuring : " + infos.ErrorOccuring.ToString());
                Console.WriteLine("InGuardSafeOperation : " + infos.InGuardSafeOperation.ToString());
                Console.WriteLine("InHoldStatusByPP : " + infos.InHoldStatusByPP.ToString());
                Console.WriteLine("InHoldStatusCmd : " + infos.InHoldStatusCmd.ToString());
                Console.WriteLine("InHoldStatusExt : " + infos.InHoldStatusExt.ToString());
                Console.WriteLine("Play : " + infos.Play.ToString());
                Console.WriteLine("Running : " + infos.Running.ToString());
                Console.WriteLine("Step : " + infos.Step.ToString());
                Console.WriteLine("Teach : " + infos.Teach.ToString());
                Thread.Sleep(40);
            }
        }
    }
}
