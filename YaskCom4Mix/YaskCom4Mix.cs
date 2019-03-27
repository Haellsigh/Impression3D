using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace YaskCombyCerinnov
{

    public class HSEClient
    {
        public HSEClient.StatusNotificationDelegate StatusNotification;
        static byte rq_nbr = 0;
        //public HSEClient(string value, int value, int value);
        public IPAddress IP_robot;

        /// <summary>
        /// Reset Alarms on the robot
        /// </summary>
        /// <returns>RecStatuts</returns>
        public HSEClient.RecStatus AlarmReset()
        {
            return SendCommand(IP_robot, 0x82, 0x01, 0x01, 0x10, new byte[4]{1,0,0,0});
        }

        /// <summary>
        /// Read the names of all axis in the selected ContolGroup
        /// </summary>
        /// <param name="ControlGroup"> ID of the ContolGroup(1 to 8 for Robot 1 to 8, 11 to 18 for Base 1 to 8, 21 to 44 for Station 1 to 24, 101 to 108 for Robot 1 to 8 in cartesian coordinate, 111 to 118 for Base 1 to 8 in cartesian coordinate)</param>
        /// <param name="AxisNames"> Output containing the name of each axis</param>
        /// <returns>RecStatuts</returns>
        public HSEClient.RecStatus AxisConfigurationInformationRead(byte ControlGroup, out string[] AxisNames)
        {
            byte[] output;
            HSEClient.RecStatus to_return = AskandReceive(IP_robot, 0x74, ControlGroup, 0x00, 0x01, new byte[0], out output);
            if (to_return.Status == 0x00)
            {
                AxisNames = new string[output.Length / 4];
                for (int i = 0; i < output.Length / 4; i++)
                {
                    AxisNames[i] = new string(new char[] { (char)output[i * 4], (char)output[i * 4 + 1], (char)output[i * 4 + 2], (char)output[i * 4 + 3] });
                }
            }
            else
                AxisNames = new string[0];
            return to_return;
        }

        /// <summary>
        /// Read the value of a B variable
        /// </summary>
        /// <param name="index"> B variable to read</param>
        /// <param name="value"> Output containing the value of the B variable</param>
        /// <returns>RecStatuts</returns>
        public HSEClient.RecStatus B_Read(short index, out byte value)
        {
            byte[] output;
            HSEClient.RecStatus to_return = AskandReceive(IP_robot, 0x7A, index, 0x01, 0x0e, new byte[0], out output);
            if (to_return.Status == 0x00)
                value = output[0];
            else
                value = 0;
            return to_return;
        }

        /// <summary>
        /// Read the value of multiple B variable
        /// </summary>
        /// <param name="index"> First B variable to read</param>
        /// <param name="nomber_to_read"> Nomber of variables to read</param>
        /// <param name="values"> Output containing the values of the B variables</param>
        /// <returns>RecStatuts</returns>
        public HSEClient.RecStatus B_Read(short index, int nomber_to_read, out byte[] values)
        {
            byte[] output;
            int nomber_to_read2 = nomber_to_read + nomber_to_read % 2;
            HSEClient.RecStatus to_return = AskandReceive(IP_robot, 0x302, index, 0x00, 0x33, BitConverter.GetBytes(nomber_to_read2), out output);
            if (to_return.Status == 0x00)
            {
                int nb_read =BitConverter.ToInt32(output, 0);
                values = new byte[nomber_to_read];
                for (int i = 0; (i < nb_read && i < nomber_to_read); i++)
                {
                    values[i] = output[i + 4];
                }
            }
            else
                values = new byte[0];
            return to_return;
        }

        /// <summary>
        /// Set the value of a B variable
        /// </summary>
        /// <param name="index"> B variable to read</param>
        /// <param name="value"> Value to set in the B variable</param>
        /// <returns>RecStatuts</returns>
        public HSEClient.RecStatus B_Write(short index, byte value)
        {
            return SendCommand(IP_robot, 0x7A, index, 0x01, 0x10, new byte[1] { value });
        }

        /// <summary>
        /// Write the value of multiple B variable
        /// </summary>
        /// <param name="index"> First B variable to write</param>
        /// <param name="number_to_write"> Nomber of variables to write</param>
        /// <param name="values"> Values to write in B variables</param>
        /// <returns>RecStatuts</returns>
        public HSEClient.RecStatus B_Write(short index, int number_to_write, byte[] values)
        {
            if (number_to_write%2!=0)
            {
                number_to_write+=-1;
                RecStatus b = B_Write((short)(index + number_to_write), values[number_to_write]);
                byte[] values2 = new ArraySegment<byte>(values, 0, number_to_write).ToArray();
                values = values2;
            }                
            return SendCommand(IP_robot, 0x302, index, 0x00, 0x34, BitConverter.GetBytes(number_to_write).Concat<byte>(values).ToArray());
        }

        //public HSEClient.RecStatus Bp_Read(short index, out HSEClient.BasePosition value);
        //public HSEClient.RecStatus Bp_Read(short index, int value, out HSEClient.BasePosition[] value);
        //public HSEClient.RecStatus Bp_Write(short index, HSEClient.BasePosition value);
        //public HSEClient.RecStatus Bp_Write(short index, int value, HSEClient.BasePosition[] value);
        //public extern static int C(string index, string value);
        //public extern static int C64(string index, string value);
        //public HSEClient.RecStatus D_Read(int index, out int value);
        //public HSEClient.RecStatus D_Read(int index, int value, out int[] value);
        //public HSEClient.RecStatus D_Write(int index, int value);
        //public HSEClient.RecStatus D_Write(int index, int value, int[] value);

        /// <summary>
        /// Reset Errors on the robot
        /// </summary>
        /// <returns>RecStatuts</returns>
        public HSEClient.RecStatus ErrorCancel()
        {
            return SendCommand(IP_robot, 0x82, 0x02, 0x01, 0x10, new byte[4] { 1, 0, 0, 0 });
        }

        //public HSEClient.RecStatus Ex_Read(int index, out HSEClient.ExternalAxis value);
        //public HSEClient.RecStatus Ex_Read(int index, int value, out HSEClient.ExternalAxis[] value);
        //public HSEClient.RecStatus Ex_Write(int index, HSEClient.ExternalAxis value);
        //public HSEClient.RecStatus Ex_Write(int index, int value, HSEClient.ExternalAxis[] value);
        //public HSEClient.RecStatus ExecutingJobInformationRead(int value, out HSEClient.ExecutingJobInfo value);
        //public HSEClient.RecStatus FileDelete(string value);
        //public HSEClient.RecStatus FileList(string value, out List<string> value);
        //public HSEClient.RecStatus FileLoad(string value, byte[] value);
        //public HSEClient.RecStatus FileSave(string value, out byte[] value);
        //public bool getLicenceState();
        //public HSEClient.RecStatus GetManagementTime(int value, out HSEClient.ManagementTime value);
        //public HSEClient.RecStatus GetSystemInformation(int value, out HSEClient.SystemInformation value);

        /// <summary>
        /// Active or Unactive HLock on the robot
        /// </summary>
        /// <param name="value"> 1 : HLock ON, 2 : HLock OFF</param>
        /// <returns></returns>
        public HSEClient.RecStatus HLock(int value)
        {
            return SendCommand(IP_robot, 0x83, 0x03, 0x01, 0x10, BitConverter.GetBytes(value));
        }

        /// <summary>
        /// Hold or Unhold the Servo
        /// </summary>
        /// <param name="value"> 1 : Hold ON, 2 : Hold OFF</param>
        /// <returns></returns>
        public HSEClient.RecStatus Hold(int value)
        {
            return SendCommand(IP_robot, 0x83, 0x01, 0x01, 0x10, BitConverter.GetBytes(value));
        }

        //public HSEClient.RecStatus I_Read(int index, out short value);
        //public HSEClient.RecStatus I_Read(int index, int value, out short[] value);
        //public HSEClient.RecStatus I_Write(int index, short value);
        //public HSEClient.RecStatus I_Write(int index, int value, short[] value);
        //public HSEClient.RecStatus IORead(int index, out byte value);
        //public HSEClient.RecStatus IORead(int index, int value, out byte[] value);
        //public HSEClient.RecStatus IOWrite(int index, byte value);
        //public HSEClient.RecStatus IOWrite(int index, int value, byte[] value);
        //public HSEClient.RecStatus JobSelect(int index, string value, uint value);
        
        /// <summary>
        /// Send a move instruction to the robot in cartesian coodinate
        /// </summary>
        /// <param name="type"> Define the type of mouvement (1 : absolute joint mouvement, 2 : absolute linear mouvement, 3 : increment linear mouvement)</param>
        /// <param name="Data"> Mouvement Data</param>
        /// <returns></returns>
        public HSEClient.RecStatus MoveCartesian(short type, HSEClient.MoveDataCartesian Data)
        {
            int[] datas = new int[26];
            datas[0] = Data.RobotNo;
            datas[1] = Data.StationNo;
            datas[2] = Data.Classification;
            datas[3] = Data.Speed;
            datas[4] = Data.Coordinate;
            datas[5] = Data.X;
            datas[6] = Data.Y;
            datas[7] = Data.Z;
            datas[8] = Data.Tx;
            datas[9] = Data.Ty;
            datas[10] = Data.Tz;
            datas[11] = 0x00;
            datas[12] = (int)Data.Type;
            datas[13] = (int)Data.ExpandedType;
            datas[14] = Data.ToolNo;
            datas[15] = Data.UserCoordinateNo;
            datas[16] = 0x00;
            datas[17] = Data.BaseAxisPosition[0];
            datas[18] = Data.BaseAxisPosition[1];
            datas[19] = Data.BaseAxisPosition[2];
            datas[20] = Data.StationAxisPosition[0];
            datas[21] = Data.StationAxisPosition[1];
            datas[22] = Data.StationAxisPosition[2];
            datas[23] = Data.StationAxisPosition[3];
            datas[24] = Data.StationAxisPosition[4];
            datas[25] = Data.StationAxisPosition[5];

            byte[] to_send = new byte[104];
            for (int i = 0; i < 26; i++)
            {
                to_send[i * 4] = BitConverter.GetBytes(datas[i])[0];
                to_send[i * 4 + 1] = BitConverter.GetBytes(datas[i])[1];
                to_send[i * 4 + 2] = BitConverter.GetBytes(datas[i])[2];
                to_send[i * 4 + 3] = BitConverter.GetBytes(datas[i])[3];
            }

                RecStatus st = SendCommand(IP_robot, 0x8a, type, 0x01, 0x02, to_send);
                return st;
        }

        public HSEClient.RecStatus MovePulse(short type, HSEClient.MoveDataPulse Data)
        {
            int[] datas = new int[22];
            datas[0] = Data.RobotNo;
            datas[1] = Data.StationNo;
            datas[2] = Data.Classification;
            datas[3] = Data.Speed;
            datas[4] = Data.RobotAxisPulseValue[0];
            datas[5] = Data.RobotAxisPulseValue[1];
            datas[6] = Data.RobotAxisPulseValue[2];
            datas[7] = Data.RobotAxisPulseValue[3];
            datas[8] = Data.RobotAxisPulseValue[4];
            datas[9] = Data.RobotAxisPulseValue[5];
            datas[10] = Data.RobotAxisPulseValue[6];
            datas[11] = Data.RobotAxisPulseValue[7];
            datas[12] = Data.ToolNo;
            datas[13] = Data.BaseAxisPosition[0];
            datas[14] = Data.BaseAxisPosition[1];
            datas[15] = Data.BaseAxisPosition[2];
            datas[16] = Data.StationAxisPosition[0];
            datas[17] = Data.StationAxisPosition[1];
            datas[18] = Data.StationAxisPosition[2];
            datas[19] = Data.StationAxisPosition[3];
            datas[20] = Data.StationAxisPosition[4];
            datas[21] = Data.StationAxisPosition[5];

            byte[] to_send = new byte[88];
            for (int i = 0; i < 22; i++)
            {
                to_send[i * 4] = BitConverter.GetBytes(datas[i])[0];
                to_send[i * 4 + 1] = BitConverter.GetBytes(datas[i])[1];
                to_send[i * 4 + 2] = BitConverter.GetBytes(datas[i])[2];
                to_send[i * 4 + 3] = BitConverter.GetBytes(datas[i])[3];
            }

            RecStatus st = SendCommand(IP_robot, 0x8b, type, 0x01, 0x02, to_send);
            return st;
        }
        //public HSEClient.RecStatus P_Read(int index, out HSEClient.RobotPositionData value);
        //public HSEClient.RecStatus P_Read(int index, int value, out HSEClient.RobotPositionData[] value);
        //public HSEClient.RecStatus P_Write(int index, HSEClient.RobotPositionData value);
        //public HSEClient.RecStatus P_Write(int index, int value, HSEClient.RobotPositionData[] value);
        //public HSEClient.RecStatus PositionErrorRead(int value, out int[] value);
        //public HSEClient.RecStatus R_Read(int index, out float value);
        //public HSEClient.RecStatus R_Read(int index, int value, out float[] value);
        //public HSEClient.RecStatus R_Write(int index, float value);
        //public HSEClient.RecStatus R_Write(int index, int value, float[] value);
        //public HSEClient.RecStatus ReadAlarmData(int value, out HSEClient.Alarm_Data value);
        //public HSEClient.RecStatus ReadAlarmData(int value, out HSEClient.Alarm_Data_Sub value);
        //public HSEClient.RecStatus ReadAlarmHistoryData(int value, out HSEClient.Alarm_Data value);
        //public HSEClient.RecStatus ReadAlarmHistoryData(int value, out HSEClient.Alarm_Data_Sub value);
        //public HSEClient.RecStatus RegRead(int value, out ushort value);
        //public HSEClient.RecStatus RegRead(int value, int value, out ushort[] value);
        //public HSEClient.RecStatus RegWrite(int value, ushort value);
        //public HSEClient.RecStatus RegWrite(int value, int value, ushort[] value);
        //public HSEClient.RecStatus RobotPositionDataRead(int value, out HSEClient.RobotPositionData value);
        //public HSEClient.RecStatus S_Read(int value, out string value);
        //public HSEClient.RecStatus S_Read(int value, int value, out string[] value);
        //public HSEClient.RecStatus S_Write(int value, string value);
        //public HSEClient.RecStatus S_Write(int value, int value, string[] value);

        /// <summary>
        /// Active or Unactive Servo on the robot
        /// </summary>
        /// <param name="value"> 1 : ON, 2 : OFF</param>
        /// <returns></returns>
        public HSEClient.RecStatus Servo(int value)
        {
            return SendCommand(IP_robot, 0x83, 0x02, 0x01, 0x10, BitConverter.GetBytes(value));
        }
        //public HSEClient.RecStatus SetContinousMode();
        //public HSEClient.RecStatus SetCycleMode();
        //public void SetStatusNotification(int value);
        //public HSEClient.RecStatus SetStepMode();
        //public HSEClient.RecStatus StartJob();
        public HSEClient.RecStatus StatusInformationRead(out HSEClient.Status_Information value)
        {
            byte[] output;
            value = new Status_Information();
            HSEClient.RecStatus to_return = AskandReceive(IP_robot, 0x72, 0x01, 0x00, 0x01, new byte[0], out output);
            value.Statut1 = output[0];
            value.Statut2 = output[4];
            return to_return;
        }
        //public HSEClient.RecStatus StringDisplay(string value);
        //public HSEClient.RecStatus TorqueDataRead(int value, out int[] value);
        //public string Version();

        public delegate void StatusNotificationDelegate(HSEClient.Status_Information SI);

        public class Alarm_Data
        {
            public uint AlarmCode;
            public uint AlarmData;
            public string AlarmName;
            public string AlarmTime;
            public uint AlarmType;

            public Alarm_Data()
            { }
        }

        public class Alarm_Data_Sub : HSEClient.Alarm_Data
        {
            public string SubCodeDataAdditionalInfo;
            public string SubCodeDataCharacterStrings;
            public string SubCodeDataCharacterStringsRev;

            public Alarm_Data_Sub()
            { }
        }

        public class BasePosition
        {
            public int[] CoordinateData;
            public uint DataType;
            public HSEClient.RecStatus S;

            public BasePosition()
            { }
        }

        public class ExecutingJobInfo
        {
            public string JobName;
            public uint LineNo;
            public uint SpeedOverrideValue;
            public uint StepNo;

            public ExecutingJobInfo()
            { }
        }

        public class ExternalAxis
        {
            public int[] CoordinateData;
            public uint DataType;
            public HSEClient.RecStatus S;

            public ExternalAxis()
            { }
        }

        public class ManagementTime
        {
            public string ElapsedTime;
            public string OperationStartTime;

            public ManagementTime()
            { }
        }

        public class MoveDataCartesian
        {
            public int[] BaseAxisPosition;
            public int Classification;
            public int Coordinate;
            public uint ExpandedType;
            public int RobotNo;
            public int Speed;
            public int[] StationAxisPosition;
            public int StationNo;
            public int ToolNo;
            public int Tx;
            public int Ty;
            public uint Type;
            public int Tz;
            public int UserCoordinateNo;
            public int X;
            public int Y;
            public int Z;

            public MoveDataCartesian()
            {
                BaseAxisPosition = new int[3];
                StationAxisPosition = new int[6];
            }

            public int Arm { get; set; }
            public int B180 { get; set; }
            public int E180 { get; set; }
            public int Flip { get; set; }
            public int FrontBack { get; set; }
            public int L180 { get; set; }
            public int R180 { get; set; }
            public int S180 { get; set; }
            public int T180 { get; set; }
            public int U180 { get; set; }
            public int W180 { get; set; }
        }

        public class MoveDataPulse
        {
            public int[] BaseAxisPosition;
            public int Classification;
            public int[] RobotAxisPulseValue;
            public int RobotNo;
            public int Speed;
            public int[] StationAxisPosition;
            public int StationNo;
            public int ToolNo;

            public MoveDataPulse()
            {
                BaseAxisPosition = new int[3];
                RobotAxisPulseValue = new int[8];
                StationAxisPosition = new int[6];
            }
        }

        public class RecStatus
        {
            public byte AddedStatusSize;
            public byte Status;
            public short Status1;
            public short Status2;

            public RecStatus()
            { }

            //public string Status1Text { get; }
            //public string Status2Text { get; }

            //public override string ToString()
            //{ }
        }

        public class RobotPositionData
        {
            public int[] AxisData;
            public uint DataType;
            public uint ToolNumber;
            public uint UserCoordinateNumber;

            public RobotPositionData()
            { }

            public int Arm { get; set; }
            public int B180 { get; set; }
            public int Conversion { get; set; }
            public int E180 { get; set; }
            public int Flip { get; set; }
            public int FrontBack { get; set; }
            public int L180 { get; set; }
            public int R180 { get; set; }
            public int Redundant { get; set; }
            public int S180 { get; set; }
            public int T180 { get; set; }
            public int U180 { get; set; }
            public int W180 { get; set; }
        }

        public class Status_Information
        {
            public bool NetworkError;
            public int Statut1;
            public int Statut2;
            public Status_Information()
            { }

            public bool Alarming
            {
                get
                {
                    return (BitConverter.GetBytes(Statut2)[0] & 16)>0;
                }
            }
            public bool AutomaticAndContinous
            {
                get
                {
                    return (BitConverter.GetBytes(Statut1)[0] & 4)>0;
                }
            }
            public bool CommandRemote
            {
                get
                {
                    return (BitConverter.GetBytes(Statut1)[0] & 128)>0;
                }
            }
            public bool Cycle1
            {
                get
                {
                    return (BitConverter.GetBytes(Statut1)[0] & 2)>0;
                }
            }
            public bool ErrorOccuring
            {
                get
                {
                    return (BitConverter.GetBytes(Statut2)[0] & 32)>0;
                }
            }
            public bool InGuardSafeOperation
            {
                get
                {
                    return (BitConverter.GetBytes(Statut1)[0] & 16)>0;
                }
            }
            public bool InHoldStatusByPP
            {
                get
                {
                    return (BitConverter.GetBytes(Statut2)[0] & 2)>0;
                }
            }
            public bool InHoldStatusCmd
            {
                get
                {
                    return (BitConverter.GetBytes(Statut2)[0] & 8)>0;
                }
            }
            public bool InHoldStatusExt
            {
                get
                {
                    return (BitConverter.GetBytes(Statut2)[0] & 4)>0;
                }
            }
            public bool Play
            {
                get
                {
                    return (BitConverter.GetBytes(Statut1)[0] & 64)>0;
                }
            }
            public bool Running
            {
                get
                {
                    return (BitConverter.GetBytes(Statut1)[0] & 8)>0;
                }
            }
            public bool ServoON
            {
                get
                {
                    return (BitConverter.GetBytes(Statut2)[0] & 64)>0;
                }
            }
            public bool Step
            {
                get
                {
                    return (BitConverter.GetBytes(Statut1)[0] & 1)>0;
                }
            }
            public bool Teach
            {
                get
                {
                    return (BitConverter.GetBytes(Statut1)[0] & 32)>0;
                }
            }

            //public bool Matches(HSEClient.Status_Information SI)
        }

        public class SystemInformation
        {
            public string ModelName;
            public string ParameterVersion;
            public string SystemSoftwareVersion;

            public SystemInformation()
            { }
        }

        public static HSEClient.RecStatus SendCommand(IPAddress IP, Int16 command_ID, Int16 instance, byte attribut, byte service, byte[] data_to_send)
        {
            byte[] to_Send = new byte[data_to_send.Length + 32];

            //Identifier
            to_Send[0] = (byte)'Y';
            to_Send[1] = (byte)'E';
            to_Send[2] = (byte)'R';
            to_Send[3] = (byte)'C';

            //Header lenght
            to_Send[4] = 0x20;
            to_Send[5] = 0x00;

            //Data lenght
            to_Send[6] = BitConverter.GetBytes(data_to_send.Length)[0];
            to_Send[7] = BitConverter.GetBytes(data_to_send.Length)[1];

            //Reserve1
            to_Send[8] = 3;

            //Processing division
            to_Send[9] = 0x01;

            //ACK
            to_Send[10] = 0x00;

            //Request ID
            to_Send[11] = 00;
            if (rq_nbr < 255)
                rq_nbr++;
            else
                rq_nbr = 0;

            //Block Number
            to_Send[12] = 0x00;
            to_Send[13] = 0x00;
            to_Send[14] = 0x00;
            to_Send[15] = 0x00;

            //Reserve
            to_Send[16] = (byte)'9';
            to_Send[17] = (byte)'9';
            to_Send[18] = (byte)'9';
            to_Send[19] = (byte)'9';
            to_Send[20] = (byte)'9';
            to_Send[21] = (byte)'9';
            to_Send[22] = (byte)'9';
            to_Send[23] = (byte)'9';

            //Command ID
            to_Send[24] = BitConverter.GetBytes(command_ID)[0];
            to_Send[25] = BitConverter.GetBytes(command_ID)[1];

            //Instance
            to_Send[26] = BitConverter.GetBytes(instance)[0];
            to_Send[27] = BitConverter.GetBytes(instance)[1];

            //Attribute
            to_Send[28] = attribut;

            //Service
            to_Send[29] = service;

            //Padding
            to_Send[30] = 0x00;
            to_Send[31] = 0x00;

            for (int i=0; i<data_to_send.Length;i++)
            {
                to_Send[i + 32] = data_to_send[i];
            }

            UdpClient UDP_Client = new UdpClient();
            IPEndPoint End_Point = new IPEndPoint(IP,10040);
            UDP_Client.Connect(End_Point);
            UDP_Client.Send(to_Send, to_Send.Length);
            byte[] receive_bytes = UDP_Client.Receive(ref End_Point);

            RecStatus OutStatuts = new RecStatus();

            OutStatuts.Status = receive_bytes[25];
            OutStatuts.AddedStatusSize = receive_bytes[26];

            if (OutStatuts.AddedStatusSize > 0)
                OutStatuts.Status1 = BitConverter.ToInt16(receive_bytes,28);
            if (OutStatuts.AddedStatusSize > 1)
                OutStatuts.Status2 = BitConverter.ToInt16(receive_bytes, 30);

            return OutStatuts;
        }

        public static HSEClient.RecStatus AskandReceive(IPAddress IP, Int16 command_ID, Int16 instance, byte attribut, byte service, byte[] data_to_send, out byte[] bytes_to_recive)
        {
            byte[] to_Send = new byte[data_to_send.Length + 32];

            //Identifier
            to_Send[0] = (byte)'Y';
            to_Send[1] = (byte)'E';
            to_Send[2] = (byte)'R';
            to_Send[3] = (byte)'C';

            //Header lenght
            to_Send[4] = 0x20;
            to_Send[5] = 0x00;

            //Data lenght
            to_Send[6] = BitConverter.GetBytes(data_to_send.Length)[0];
            to_Send[7] = BitConverter.GetBytes(data_to_send.Length)[1];

            //Reserve1
            to_Send[8] = 3;

            //Processing division
            to_Send[9] = 0x01;

            //ACK
            to_Send[10] = 0x00;

            //Request ID
            to_Send[11] = rq_nbr;
            if (rq_nbr < 255)
                rq_nbr++;
            else
                rq_nbr = 0;

            //Block Number
            to_Send[12] = 0x00;
            to_Send[13] = 0x00;
            to_Send[14] = 0x00;
            to_Send[15] = 0x00;

            //Reserve
            to_Send[16] = (byte)'9';
            to_Send[17] = (byte)'9';
            to_Send[18] = (byte)'9';
            to_Send[19] = (byte)'9';
            to_Send[20] = (byte)'9';
            to_Send[21] = (byte)'9';
            to_Send[22] = (byte)'9';
            to_Send[23] = (byte)'9';

            //Command ID
            to_Send[24] = BitConverter.GetBytes(command_ID)[0];
            to_Send[25] = BitConverter.GetBytes(command_ID)[1];

            //Instance
            to_Send[26] = BitConverter.GetBytes(instance)[0];
            to_Send[27] = BitConverter.GetBytes(instance)[1];

            //Attribute
            to_Send[28] = attribut;

            //Service
            to_Send[29] = service;

            //Padding
            to_Send[30] = 0x00;
            to_Send[31] = 0x00;

            for (int i = 0; i < data_to_send.Length; i++)
            {
                to_Send[i + 32] = data_to_send[i];
            }

            UdpClient UDP_Client = new UdpClient();
            IPEndPoint End_Point = new IPEndPoint(IP, 10040);
            UDP_Client.Connect(End_Point);
            UDP_Client.Send(to_Send, to_Send.Length);
            byte[] receive_bytes = UDP_Client.Receive(ref End_Point);

            RecStatus OutStatuts = new RecStatus();

            OutStatuts.Status = receive_bytes[25];
            OutStatuts.AddedStatusSize = receive_bytes[26];

            if (OutStatuts.AddedStatusSize > 0)
                OutStatuts.Status1 = BitConverter.ToInt16(receive_bytes, 28);
            if (OutStatuts.AddedStatusSize > 1)
                OutStatuts.Status2 = BitConverter.ToInt16(receive_bytes, 30);


            bytes_to_recive = new byte[receive_bytes.Length-32];
            for (int i=0; i< receive_bytes.Length-32; i++)
            {
                bytes_to_recive[i] = receive_bytes[i + 32];
            }
            
            return OutStatuts;
        }
    }
    
}
