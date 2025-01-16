#ifndef VOICE_LINK_MAIN_H
#define VOICE_LINK_MAIN_H
#include <windows.h>
#include <SimConnect.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <string>
#include <strsafe.h>
#include <tlhelp32.h>


extern "C" {
  struct SimData
{
    char title[256];
    int bat_master;
    int avionics_master;
    double com1percent;
    double com2percent;
    int com1;
    int com2;
    int recvCom1;
    int recvCom2;
    int avCom1;
    int avCom2;
    uint32_t com1Freq;
    uint32_t com2Freq;
};
__declspec(dllexport) bool connectSim();
__declspec(dllexport) void requestData();
__declspec(dllexport) bool hasData();
__declspec(dllexport) SimData getData();
}
#endif