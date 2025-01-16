#include "main.h"


enum class ComState {
  Failed = -1,
  Ok = 0,
  DoesNotExist = 1,
  PoweredOff = 2,
  ComFailed = 3
};

std::string comStateStr(ComState v) {
    if(v == ComState::Failed)
        return "FAILED";
    if(v == ComState::Ok)
        return "OK";
    if(v == ComState::DoesNotExist)
        return "DOES_NOT_EXIST";
    if(v == ComState::PoweredOff)
        return "OFF";
    if(v == ComState::ComFailed)
        return "FAILED";
    return "";
}

struct State {
    bool initialised = false;
    bool hasData = false;
    SimData data;
    bool connected = false;
    HANDLE hSimConnect = NULL;
};

static enum EVENT_ID{
    EVENT_SIM_START,
    EVENT_1_SEC
};

static enum DATA_DEFINE_ID {
    DEFINITION_1,
};

static enum DATA_REQUEST_ID {
    REQUEST_1,
};


State state;

bool isReceiving(ComState status, bool isReceiving, bool isAvailable) {
  return status == ComState::Ok && isReceiving && isAvailable;
}


void CALLBACK MyDispatchProcRD(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext)
{
    HRESULT hr;
    
    switch(pData->dwID)
    {

        case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
        {
            SIMCONNECT_RECV_SIMOBJECT_DATA *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;
            
            switch(pObjData->dwRequestID)
            {
                case REQUEST_1:
                {
                    DWORD ObjectID = pObjData->dwObjectID;
                    SimData *data = (SimData*)&pObjData->dwData;
                    if (SUCCEEDED(StringCbLengthA(&data->title[0], sizeof(data->title), NULL))) // security check
                    {
                        state.data = *data;
                        state.hasData = true;
                    } 
                    break;
                }

                default:
                   break;
            }
            break;
        }

        case SIMCONNECT_RECV_ID_QUIT:
        {
       
            state.connected = false;
            break;
        }

        default:
          
            break;
    }
}

void init() {
    if(state.initialised)
        return;
    state.initialised = true;
    CoInitialize(nullptr);
}

__declspec(dllexport) SimData getData() {
    init();
    return state.data;
}

__declspec(dllexport) bool hasData() {
    init();
    bool v =  state.hasData;
    state.hasData = false;
    return v;
}

__declspec(dllexport) void requestData() {
    SimConnect_CallDispatch(state.hSimConnect, MyDispatchProcRD, NULL);
}

__declspec(dllexport) bool connectSim() {
     init();
    if(state.connected)
        return true;
      HRESULT hr;
    if(state.hSimConnect) {
        hr = SimConnect_Close(state.hSimConnect);
        state.hSimConnect = NULL;
    }

    if (SUCCEEDED(SimConnect_Open(&state.hSimConnect, "VoiceLink", NULL, 0, 0, 0))) {
        hr = SimConnect_AddToDataDefinition(state.hSimConnect, DEFINITION_1, "Title", NULL, SIMCONNECT_DATATYPE_STRING256);
        hr = SimConnect_AddToDataDefinition(state.hSimConnect, DEFINITION_1, "ELECTRICAL MASTER BATTERY", "bool", SIMCONNECT_DATATYPE_INT32);
        hr = SimConnect_AddToDataDefinition(state.hSimConnect, DEFINITION_1, "AVIONICS MASTER SWITCH", "bool", SIMCONNECT_DATATYPE_INT32);
        hr = SimConnect_AddToDataDefinition(state.hSimConnect, DEFINITION_1, "COM VOLUME:1", "percent");
        hr = SimConnect_AddToDataDefinition(state.hSimConnect, DEFINITION_1, "COM VOLUME:2", "percent");
        hr = SimConnect_AddToDataDefinition(state.hSimConnect, DEFINITION_1, "COM STATUS:1", "enum", SIMCONNECT_DATATYPE_INT32);
        hr = SimConnect_AddToDataDefinition(state.hSimConnect, DEFINITION_1, "COM STATUS:2", "enum", SIMCONNECT_DATATYPE_INT32);
        hr = SimConnect_AddToDataDefinition(state.hSimConnect, DEFINITION_1, "COM RECEIVE:1", "bool", SIMCONNECT_DATATYPE_INT32);
        hr = SimConnect_AddToDataDefinition(state.hSimConnect, DEFINITION_1, "COM RECEIVE:2", "bool", SIMCONNECT_DATATYPE_INT32);
        hr = SimConnect_AddToDataDefinition(state.hSimConnect, DEFINITION_1, "COM AVAILABLE:1", "bool", SIMCONNECT_DATATYPE_INT32);
        hr = SimConnect_AddToDataDefinition(state.hSimConnect, DEFINITION_1, "COM AVAILABLE:2", "bool", SIMCONNECT_DATATYPE_INT32);
           hr = SimConnect_AddToDataDefinition(state.hSimConnect, DEFINITION_1, "COM ACTIVE FREQUENCY:1", "Khz", SIMCONNECT_DATATYPE_INT32);
        hr = SimConnect_AddToDataDefinition(state.hSimConnect, DEFINITION_1, "COM ACTIVE FREQUENCY:2", "Khz", SIMCONNECT_DATATYPE_INT32);
     
        hr = SimConnect_RequestDataOnSimObject(state.hSimConnect, REQUEST_1, DEFINITION_1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND);

        state.connected = true;

        return true;
    }
    return false;
}

