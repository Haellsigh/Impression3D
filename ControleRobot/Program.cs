using System;
using System.Threading;
using YaskCombyCerinnov;

namespace ControleRobot
{
    class Program
    {
        static void Main(string[] args)
        {
            GCodeReader reader = new GCodeReader();

            HSEClient clientUDP = new HSEClient();
            Console.WriteLine("bonjour cher ami");
            System.Net.IPAddress.TryParse("192.168.255.1", out clientUDP.IP_robot);

            /*bool toggleServo = false;

            while (true)
            {
                Console.WriteLine("Enter to toggle servo");
                Console.ReadLine();
                if (toggleServo)
                {
                    clientUDP.Servo(1);
                }
                else
                {
                    clientUDP.Servo(2);
                }
                toggleServo = !toggleServo;

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
            }*/

            Console.WriteLine("Enter to active servos");
            Console.ReadLine();
            clientUDP.Servo(1);
            Console.WriteLine("Enter to move continuously");
            Console.ReadLine();

            /// MOVE CARTESIAN

            HSEClient.MoveDataCartesian Position1 = new HSEClient.MoveDataCartesian();

            Position1.Classification = 0;
            Position1.Coordinate = 17;
            Position1.ExpandedType = (int)0;
            Position1.Tx = -173 * 10000;
            Position1.Ty = -1 * 10000;
            Position1.Tz = 0 * 10000;
            Position1.UserCoordinateNo = 0;
            Position1.X = (int)1022 * 1000;
            Position1.Y = (int)188 * 1000;
            Position1.Z = (int)75.7 * 1000;
            Position1.Type = 0;
            Position1.RobotNo = 1;
            Position1.StationNo = 0;
            Position1.Classification = 1;
            Position1.Speed = 10000;
            Position1.ToolNo = 0;
            Position1.StationAxisPosition = new int[6] { 0, 0, 0, 0, 0, 0 };
            Position1.BaseAxisPosition = new int[3] { 0, 0, 0 };

            HSEClient.MoveDataCartesian Position2 = new HSEClient.MoveDataCartesian();

            Position2.Classification = 0;
            Position2.Coordinate = 17;
            Position2.ExpandedType = (int)0;
            Position2.Tx = -173 * 10000;
            Position2.Ty = -4 * 10000;
            Position2.Tz = 28 * 10000;
            Position2.UserCoordinateNo = 0;
            Position2.X = (int)1245 * 1000;
            Position2.Y = (int)-479 * 1000;
            Position2.Z = (int)56 * 1000;
            Position2.Type = 0;
            Position2.RobotNo = 1;
            Position2.StationNo = 0;
            Position2.Classification = 1;
            Position2.Speed = 10000;
            Position2.ToolNo = 0;
            Position2.StationAxisPosition = new int[6] { 0, 0, 0, 0, 0, 0 };
            Position2.BaseAxisPosition = new int[3] { 0, 0, 0 };

            /*HSEClient.Status_Information infos;
            while (true)
            {
                clientUDP.MoveCartesian(1, Position1);
                do
                {
                    clientUDP.StatusInformationRead(out infos);
                    Thread.Sleep(40);
                } while (infos.Running);

                clientUDP.MoveCartesian(1, Position2);
                do
                {
                    clientUDP.StatusInformationRead(out infos);
                    Thread.Sleep(40);
                } while (infos.Running);
            }*/

            /// MOVE PULSE

            HSEClient.MoveDataPulse PulseHome = new HSEClient.MoveDataPulse();
            PulseHome.RobotAxisPulseValue = new int[8] { -50000, 0, 0, 0, 0, 0, 0, 0 };
            PulseHome.RobotNo = 1;
            PulseHome.StationNo = 0;
            PulseHome.Classification = 0;
            PulseHome.Speed = 10000;
            PulseHome.ToolNo = 0;
            PulseHome.StationAxisPosition = new int[6] { 0, 0, 0, 0, 0, 0 };
            PulseHome.BaseAxisPosition = new int[3] { 0, 0, 0 };

            HSEClient.MoveDataPulse PulsePosition = new HSEClient.MoveDataPulse();
            PulsePosition.RobotAxisPulseValue = new int[8] { 50000, 0, 0, 0, 0, 0, 0, 0 };
            PulsePosition.RobotNo = 1;
            PulsePosition.StationNo = 0;
            PulsePosition.Classification = 0;
            PulsePosition.Speed = 10000;
            PulsePosition.ToolNo = 0;
            PulsePosition.StationAxisPosition = new int[6] { 0, 0, 0, 0, 0, 0 };
            PulsePosition.BaseAxisPosition = new int[3] { 0, 0, 0 };
            HSEClient.Status_Information infos;
            while (true)
            {
                clientUDP.MovePulse(1, PulseHome);
                do
                {
                    clientUDP.StatusInformationRead(out infos);
                    Thread.Sleep(40);
                } while (infos.Running);

                clientUDP.MovePulse(1, PulsePosition);
                do
                {
                    clientUDP.StatusInformationRead(out infos);
                    Thread.Sleep(40);
                } while (infos.Running);
            }

            /*while (true)
            {
                clientUDP.MoveCartesian(1, Data2);
                Thread.Sleep(50);
                clientUDP.MoveCartesian(1, Data3);
                Thread.Sleep(50);
            }*/



            //HSEClient.MoveDataPulse Data = new HSEClient.MoveDataPulse();
            //Data.RobotAxisPulseValue = new int[8] { 1000, 0, 0, 0, 0, 0, 0, 0 };
            //Data.RobotNo = 1;
            //Data.StationNo = 0;
            //Data.Classification = 0;
            //Data.Speed = 1000;
            //Data.ToolNo = 0;
            //Data.StationAxisPosition = new int[6] { 0, 0, 0, 0, 0, 0 };
            //Data.BaseAxisPosition = new int[3] { 0, 0, 0 };

            //HSEClient.MoveDataCartesian Data2 = new HSEClient.MoveDataCartesian();

            //Data2.Classification = 0;
            //Data2.Coordinate = 17;
            //Data2.ExpandedType = (int)0;
            //Data2.Tx = 0;
            //Data2.Ty = 0;
            //Data2.Tz = 0;
            //Data2.UserCoordinateNo = 0;
            //Data2.X = 400000;
            //Data2.Y = 200000;
            //Data2.Z = 600000;
            //Data2.Type = 0;
            //Data2.RobotNo = 1;
            //Data2.StationNo = 0;
            //Data2.Classification = 0;
            //Data2.Speed = 1000;
            //Data2.ToolNo = 0;
            //Data2.StationAxisPosition = new int[6] { 0, 0, 0, 0, 0, 0 };
            //Data2.BaseAxisPosition = new int[3] { 0, 0, 0 };

            //HSEClient.MoveDataCartesian Data3 = new HSEClient.MoveDataCartesian();

            //Data3.Classification = 0;
            //Data3.Coordinate = 17;
            //Data3.ExpandedType = (int)0;
            //Data3.Tx = 0;
            //Data3.Ty = 0;
            //Data3.Tz = 0;
            //Data3.UserCoordinateNo = 0;
            //Data3.X = 400000;
            //Data3.Y = -200000;
            //Data3.Z = 600000;
            //Data3.Type = 0;
            //Data3.RobotNo = 1;
            //Data3.StationNo = 0;
            //Data3.Classification = 0;
            //Data3.Speed = 1000;
            //Data3.ToolNo = 0;
            //Data3.StationAxisPosition = new int[6] { 0, 0, 0, 0, 0, 0 };
            //Data3.BaseAxisPosition = new int[3] { 0, 0, 0 };

            //Console.WriteLine("press Enter to active");
            //Console.ReadLine();
            //clientUDP.Servo(1);
            //Console.WriteLine("press Enter to move");
            //Console.ReadLine();

            //clientUDP.MovePulse(1, Data);
            ////for (int k = 0; k < 10; k++)
            ////{
            ////    clientUDP.MoveCartesian(1, Data2);
            ////    Thread.Sleep(50);
            ////    clientUDP.MoveCartesian(1, Data3);
            ////    Thread.Sleep(50);
            ////}
            //Console.WriteLine("press Enter to unactive");
            //Console.ReadLine();
            //clientUDP.Servo(2);
            ////Console.WriteLine("First B to Write");
            ////short F_B = Convert.ToInt16(Console.ReadLine());
            ////Console.WriteLine("Number of Bs to Write");
            ////int N_B = Convert.ToInt16(Console.ReadLine());
            ////byte[] values = new byte[N_B];
            ////for (int j=0; j<N_B;j++)
            ////{
            ////    Console.WriteLine("value of B" + (F_B + j).ToString());
            ////    values[j] = Convert.ToByte(Console.ReadLine());
            ////}

            ////clientUDP.B_Write(F_B,N_B, values);
            ////foreach (byte n in values)
            ////    Console.WriteLine(n.ToString());
            Console.ReadLine();
        }
    }
}
