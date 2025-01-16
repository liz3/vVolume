using System.ComponentModel;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Timer = System.Timers.Timer;

namespace vVolume;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    [DllImport("VoiceLink.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern bool connectSim();

    [DllImport("VoiceLink.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern bool hasData();

    [DllImport("VoiceLink.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern bool requestData();

    [StructLayout(LayoutKind.Sequential)]
    public struct SimDataStruct
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
        public string title;

        int bat_master;
        int avionics_master;
        public double com1percent;
        public double com2percent;
        public int com1;
        public int com2;
        public int recvCom1;
        public int recvCom2;
        public int avCom1;
        public int avCom2;
        public uint com1Freq;
        public uint com2Freq;
    }

    [DllImport("VoiceLink.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern SimDataStruct getData();

    Timer timer;
    bool vPilotRunning = false;
    SimDataStruct storedData;

    public MainWindow()
    {
        InitializeComponent();
        statusLabel.Content = "vPilot not running";
        timer = new Timer(600);
        timer.Elapsed += (sender, args) =>
        {
            var processes = Process.GetProcessesByName("vPilot");
            var running = processes.Length > 0;
            if (connectSim())
            {
                requestData();
                if (hasData())
                {
                    var simData = getData();
                    if (storedData.com1percent != simData.com1percent)
                    {
                        if (running)
                            if (setVolume((float)((simData.com1percent / 100)), 1))
                                Application.Current.Dispatcher.Invoke(() => { com1Bar.Value = simData.com1percent; });
                    }

                    if (storedData.com2percent != simData.com2percent)
                    {
                        if (running)
                            if (setVolume((float)((simData.com2percent / 100)), 2))
                                Application.Current.Dispatcher.Invoke(() => { com2Bar.Value = simData.com2percent; });
                    }

                    if (storedData.com1Freq != simData.com1Freq || storedData.com2Freq != simData.com2Freq)
                    {
                        Application.Current.Dispatcher.Invoke(() =>
                        {
                            com1Label.Content = $"{DecodeBCD16ToFrequency((uint)simData.com1Freq)}";
                            com2Label.Content = $"{DecodeBCD16ToFrequency((uint)simData.com2Freq)}";
                        });
                    }

                    if (running)
                        storedData = simData;
                }
            }

            Application.Current.Dispatcher.Invoke(() =>
            {
                if (running != vPilotRunning)
                {
                    vPilotRunning = running;
                    statusLabel.Content = running ? "vPilot Running" : "vPilot Not Running";
                }
            });
        };
        timer.AutoReset = true;
        timer.Enabled = true;
    }

    static string DecodeBCD16ToFrequency(uint v)
    {
        return (v / 1000.0).ToString("0.000", CultureInfo.InvariantCulture);
    }

    bool setVolume(float value, ushort id)
    {
        Process process = new Process();
        process.StartInfo.FileName = "vph\\VPilotAdjust.exe";
        process.StartInfo.Arguments = $"{id} {value}";
        process.StartInfo.UseShellExecute = false;
        process.StartInfo.CreateNoWindow = true;
        try
        {
            process.Start();
            process.WaitForExit();
            return process.ExitCode == 0;
        }
        catch
        {
            return false;
        }
    }
}