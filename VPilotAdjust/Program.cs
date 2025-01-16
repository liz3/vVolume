using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;
using Microsoft.Diagnostics.Runtime;

const int PROCESS_ALL_ACCESS = 0x001F0FFF;

[DllImport("kernel32.dll")]
static extern IntPtr OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);

[DllImport("kernel32.dll")]
static extern bool CloseHandle(IntPtr handle);


[DllImport("kernel32.dll")]
static extern bool WriteProcessMemory(
    IntPtr hProcess,
    IntPtr lpBaseAddress,
    byte[] lpBuffer,
    Int32 nSize,
    out IntPtr lpNumberOfBytesWritten);

Process[] processes = Process.GetProcessesByName("vPilot");
if (processes.Length == 0)
{
    Environment.Exit(1);
}
else
{
    var value = float.Parse(args[args.Length - 1], CultureInfo.InvariantCulture.NumberFormat);
    var id = int.Parse(args[args.Length - 2], CultureInfo.InvariantCulture.NumberFormat);
    var targetProcess = processes[0];
    byte[] data = new byte[4];

    var set = false;
    Buffer.BlockCopy(BitConverter.GetBytes(value), 0, data, 0, 4);
    using (var dataTarget = DataTarget.AttachToProcess(targetProcess.Id, suspend: false))
    {
        var runtime = dataTarget.ClrVersions[0].CreateRuntime();

        foreach (var enumerateObject in runtime.Heap.EnumerateObjects())
        {
            if (enumerateObject.ToString().EndsWith("ReceiverSampleProvider"))
            {
                //   if (enumerateObject.ToString().EndsWith("VolumeSampleProvider"))

                ushort eid = enumerateObject.ReadField<ushort>("<ID>k__BackingField");
                Console.WriteLine($"{eid}");
                if (eid != id)
                    continue;
                var vol = enumerateObject.ReadObjectField("volume");
                var proc = OpenProcess(PROCESS_ALL_ACCESS, false, targetProcess.Id);
                WriteProcessMemory(proc, new IntPtr((long)vol.Address + 8), data, data.Length, out _);
                CloseHandle(proc);
                set = true;
                break;
            }
        }

        runtime.Dispose();
    }

    Environment.Exit(set ? 0 : 2);
}