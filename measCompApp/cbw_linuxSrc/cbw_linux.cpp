#include <stdio.h>
#include <vector>
#include <string>

#include "mcBoard.h"

#define MAX_DEVICES 100

std::vector<mcBoard*> boardList(MAX_DEVICES);

extern "C" {
// System functions

int cbGetDaqDeviceInventory(DaqDeviceInterface InterfaceType, DaqDeviceDescriptor* Inventory, INT* NumberOfDevices)
{
  ulDaqDeviceDescriptor* ulInventory = (ulDaqDeviceDescriptor*) calloc(*NumberOfDevices, sizeof(ulDaqDeviceDescriptor));
  UlError error = ulGetDaqDeviceInventory((ulDaqDeviceInterface)InterfaceType, ulInventory, (unsigned int*)NumberOfDevices);
  for (int i=0; i<*NumberOfDevices; i++) {
    strcpy(Inventory[i].ProductName, ulInventory[i].productName);
    Inventory[i].ProductID = ulInventory[i].productId;
    Inventory[i].InterfaceType = (DaqDeviceInterface)ulInventory[i].devInterface;
    strcpy(Inventory[i].DevString, ulInventory[i].devString);
    strcpy(Inventory[i].UniqueID, ulInventory[i].uniqueId);
    memcpy(Inventory[i].Reserved, ulInventory[i].reserved, sizeof(Inventory[i].Reserved));
  }
  return error;
}

int cbIgnoreInstaCal()
{
  return 0;
}

int cbGetNetDeviceDescriptor(char* Host, int Port, DaqDeviceDescriptor* DeviceDescriptor, int Timeout)
{
    ulDaqDeviceDescriptor ulDeviceDescriptor;
    UlError error = ulGetNetDaqDeviceDescriptor((const char*)Host, (unsigned short)Port, NULL, &ulDeviceDescriptor, Timeout);
    strcpy(DeviceDescriptor->ProductName, ulDeviceDescriptor.productName);
    DeviceDescriptor->ProductID = ulDeviceDescriptor.productId;
    DeviceDescriptor->InterfaceType = (DaqDeviceInterface)ulDeviceDescriptor.devInterface;
    strcpy(DeviceDescriptor->DevString, ulDeviceDescriptor.devString);
    strcpy(DeviceDescriptor->UniqueID, ulDeviceDescriptor.uniqueId);
    memcpy(DeviceDescriptor->Reserved, ulDeviceDescriptor.reserved, sizeof(DeviceDescriptor->Reserved));
    return error;
}

int cbCreateDaqDevice(int BoardNum, DaqDeviceDescriptor DeviceDescriptor)
{
    ulDaqDeviceDescriptor ulDeviceDescriptor;
    strcpy(ulDeviceDescriptor.productName, DeviceDescriptor.ProductName);
    ulDeviceDescriptor.productId = DeviceDescriptor.ProductID;
    ulDeviceDescriptor.devInterface = (ulDaqDeviceInterface)DeviceDescriptor.InterfaceType;
    strcpy(ulDeviceDescriptor.devString, DeviceDescriptor.DevString);
    strcpy(ulDeviceDescriptor.uniqueId, DeviceDescriptor.UniqueID);
    memcpy(ulDeviceDescriptor.reserved, DeviceDescriptor.Reserved, sizeof(DeviceDescriptor.Reserved));
    DaqDeviceHandle devHandle = ulCreateDaqDevice(ulDeviceDescriptor);
    mcBoard *pBoard = new mcBoard(ulDeviceDescriptor, devHandle);
    boardList[BoardNum] = pBoard;
    return NOERRORS;
}

int cbGetConfig(int InfoType, int BoardNum, int DevNum,
                int ConfigItem, int *ConfigVal)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbGetConfig(InfoType, DevNum, ConfigItem, ConfigVal);
}

int cbSetConfig(int InfoType, int BoardNum, int DevNum, int ConfigItem, int ConfigVal)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbSetConfig(InfoType, DevNum, ConfigItem, ConfigVal);
}

int cbGetBoardName(int BoardNum, char *BoardName)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbGetBoardName(BoardName);
}

int cbGetErrMsg(int ErrCode, char *ErrMsg)
{
    static const char *functionName = "cbGetErrMsg";
    printf("Function %s not supported\n", functionName);
    return NOERRORS;
}

HGLOBAL cbWinBufAlloc(long NumPoints)
{
    return calloc(NumPoints, sizeof(short));
}

HGLOBAL cbScaledWinBufAlloc(long NumPoints)
{
    return calloc(NumPoints, sizeof(double));
}

HGLOBAL cbWinBufAlloc32(long NumPoints)
{
    return calloc(NumPoints, sizeof(int));
}

int cbStopIOBackground(int BoardNum, int FunctionType)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbStopIOBackground(FunctionType);
}

int cbGetIOStatus(int BoardNum, short *Status, long *CurCount, long *CurIndex,int FunctionType)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbGetIOStatus(Status, CurCount, CurIndex, FunctionType);
}


int cbAIn(int BoardNum, int Chan, int Gain, USHORT *DataValue)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbAIn(Chan, Gain, DataValue);
}

int cbAIn32(int BoardNum, int Chan, int Gain, ULONG *DataValue, int Options)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbAIn32(Chan, Gain, DataValue, Options);
}

int cbAInScan(int BoardNum, int LowChan, int HighChan, long Count,
              long *Rate, int Gain, HGLOBAL MemHandle, int Options)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbAInScan(LowChan, HighChan, Count, Rate, Gain, MemHandle, Options);
}

int cbAInputMode(int BoardNum, int InputMode)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbAInputMode(InputMode);
}

int cbALoadQueue(int BoardNum, short *ChanArray, short *GainArray, int NumChans)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbALoadQueue(ChanArray, GainArray, NumChans);
}

int cbAOut(int BoardNum, int Chan, int Gain, USHORT DataValue)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbAOut(Chan, Gain, DataValue);
}

int cbAOutScan(int BoardNum, int LowChan, int HighChan,
               long Count, long *Rate, int Gain,
               HGLOBAL MemHandle, int Options)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbAOutScan(LowChan, HighChan, Count, Rate, Gain, MemHandle, Options);
}

int cbCIn32(int BoardNum, int CounterNum, ULONG *Count)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbCIn32(CounterNum, Count);
}

int cbCLoad32(int BoardNum, int RegNum, ULONG LoadValue)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbCLoad32(RegNum, LoadValue);
}

int cbCInScan(int BoardNum, int FirstCtr,int LastCtr, LONG Count,
              LONG *Rate, HGLOBAL MemHandle, ULONG Options)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbCInScan(FirstCtr, LastCtr, Count, Rate, MemHandle, Options);
}

int cbCConfigScan(int BoardNum, int CounterNum, int Mode,int DebounceTime,
                  int DebounceMode, int EdgeDetection,
                  int TickSize, int MappedChannel)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbCConfigScan(CounterNum, Mode, DebounceTime, DebounceMode,
                                 EdgeDetection, TickSize, MappedChannel);
}


// Digital I/O functions
int cbDBitOut(int BoardNum, int PortType, int BitNum, USHORT BitValue)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbDBitOut(PortType, BitNum, BitValue);
}

int cbDConfigPort(int BoardNum, int PortType, int Direction)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbDConfigPort(PortType, Direction);
}

int cbDConfigBit(int BoardNum, int PortType, int BitNum, int Direction)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbDConfigBit(PortType, BitNum, Direction);
}

int cbDIn(int BoardNum, int PortType, USHORT *DataValue)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbDIn(PortType, DataValue);
}

int cbDIn32(int BoardNum, int PortType, UINT *DataValue)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbDIn32(PortType, DataValue);
}

int cbDOut(int BoardNum, int PortType, USHORT DataValue)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbDOut(PortType, DataValue);
}

int cbDOut32(int BoardNum, int PortType, UINT DataValue)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbDOut32(PortType, DataValue);
}

// Pulse functions
int cbPulseOutStart(int BoardNum, int TimerNum, double *Frequency,
                    double *DutyCycle, unsigned int PulseCount,
                    double *InitialDelay, int IdleState, int Options)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbPulseOutStart(TimerNum, Frequency, DutyCycle, PulseCount,
                                   InitialDelay, IdleState, Options);
}

int cbPulseOutStop(int BoardNum, int TimerNum)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbPulseOutStop(TimerNum);
}


// Temperature functions
int cbTIn(int BoardNum, int Chan, int Scale, float *TempValue, int Options)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbTIn(Chan, Scale, TempValue, Options);
}

// Voltage functions
int cbVIn(int BoardNum, int Chan, int Range, float *DataValue, int Options)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbVIn(Chan, Range, DataValue, Options);
}

// Trigger functions
int cbSetTrigger(int BoardNum, int TrigType, USHORT LowThreshold,
                 USHORT HighThreshold)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbSetTrigger(TrigType, LowThreshold, HighThreshold);
}

// Daq functions
int cbDaqInScan(int BoardNum, short *ChanArray, short *ChanTypeArray, short *GainArray, int ChanCount, long *Rate,
                long *PretrigCount, long *TotalCount, HGLOBAL MemHandle, int Options)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbDaqInScan(ChanArray, ChanTypeArray, GainArray, ChanCount, Rate,
                                             PretrigCount, TotalCount, MemHandle, Options);
}

int cbDaqSetTrigger(int BoardNum, int TrigSource, int TrigSense, int TrigChan, int ChanType,
                    int Gain, float Level, float Variance, int TrigEvent)
{
    if (BoardNum >= (int)boardList.size()) return BADBOARD;
    mcBoard *pBoard = boardList[BoardNum];
    return pBoard->cbDaqSetTrigger(TrigSource, TrigSense, TrigChan, ChanType,
                                                 Gain, Level, Variance, TrigEvent);
}

} /* extern "C" */
