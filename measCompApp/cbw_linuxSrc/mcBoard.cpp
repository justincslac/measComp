#include <stdio.h>
#include <string.h>

#include "mcBoard.h"


mcBoard::mcBoard(uldaq::DaqDeviceDescriptor daqDeviceDescriptor, uldaq::DaqDeviceHandle daqDeviceHandle) {
    daqDeviceDescriptor_ = daqDeviceDescriptor;
    daqDeviceHandle_ = daqDeviceHandle;
    strcpy(boardName_, daqDeviceDescriptor.productName);
    biBoardType_    = 0;
    biNumADCChans_  = 0;
    biADCRes_       = 0;
    biNumDACChans_  = 0;
    biDACRes_       = 0;
    biNumTempChans_ = 0;
    biDInNumDevs_   = 0;
    diDevType_      = 0;
    diInMask_       = 0;
    diOutMask_      = 0;
    diNumBits_      = 0;
}

// System functions

int mcBoard::cbGetConfig(int InfoType, int DevNum, int ConfigItem, int *ConfigVal) {
    switch (InfoType) {
    case BOARDINFO:
        switch (ConfigItem) {
        case BIBOARDTYPE:
            *ConfigVal = biBoardType_;
            break;
        case BINUMADCHANS:
            *ConfigVal = biNumADCChans_;
            break;
        case BIADRES:
            *ConfigVal = biADCRes_;
            break;
        case BINUMDACHANS:
            *ConfigVal = biNumDACChans_;
            break;
        case BIDACRES:
            *ConfigVal = biDACRes_;
            break;
        case BINUMTEMPCHANS:
            *ConfigVal = biNumTempChans_;
            break;
        case BIDINUMDEVS:
            *ConfigVal = biDInNumDevs_;
            break;
    }
    break;

    case DIGITALINFO:
        switch(ConfigItem) {
        case DIDEVTYPE:
            *ConfigVal = diDevType_;
            break;
        case DIINMASK:
            *ConfigVal = diInMask_;
            break;
        case DIOUTMASK:
            *ConfigVal = diOutMask_;
            break;
        case DINUMBITS:
            *ConfigVal = diNumBits_;
            break;
        }
    break;
    }
    return NOERRORS;
}

int mcBoard::cbSetConfig(int InfoType, int DevNum, int ConfigItem, int ConfigVal)
{
    printf("Function cbSetConfig not supported\n");
    return NOERRORS;
}



int mcBoard::cbGetBoardName(char *BoardName)
{
    strcpy(BoardName, boardName_);
    return NOERRORS;
}

int mcBoard::cbGetIOStatus(short *Status, long *CurCount, long *CurIndex, int FunctionType)
{
    uldaq::ScanStatus scanStatus;
    uldaq::TransferStatus xferStatus;
    uldaq::UlError error = uldaq::ERR_NO_ERROR;
    switch (FunctionType) { 
      case AIFUNCTION:
        error = ulDaqInScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
        break;
      case AOFUNCTION:
        error = uldaq::ulDaqOutScanStop(daqDeviceHandle_);
        break;
    }
    *Status = scanStatus;
    *CurCount = xferStatus.currentTotalCount;
    return error;
}

int mcBoard::cbStopIOBackground(int FunctionType)
{
    uldaq::UlError error = uldaq::ERR_NO_ERROR;
    switch (FunctionType) { 
      case AIFUNCTION:
        error = uldaq::ulDaqInScanStop(daqDeviceHandle_);
        break;
      case AOFUNCTION:
        error = uldaq::ulDaqOutScanStop(daqDeviceHandle_);
        break;
    }
    return error;
}

// Analog I/O functions
int mcBoard::cbAIn(int Chan, int Gain, USHORT *DataValue)
{
    double data;
    uldaq::UlError error = ulAIn(daqDeviceHandle_, Chan, aiInputMode_, aiRange_, aiFlag_, &data);
    *DataValue = (USHORT)data;
    return error;
}

int mcBoard::cbAIn32(int Chan, int Gain, ULONG *DataValue, int Options)
{
    printf("Function cbAIn32 not supported\n");
    return NOERRORS;
}

int mcBoard::cbAInScan(int LowChan, int HighChan, long Count, long *Rate,
                       int Gain, HGLOBAL MemHandle, int Options)
{
    printf("Function cbAInScan not supported\n");
    return NOERRORS;
}

int mcBoard::cbAInputMode(int InputMode)
{
    printf("Function cbAInputMode not supported\n");
    return NOERRORS;
}

int mcBoard::cbALoadQueue(short *ChanArray, short *GainArray, int NumChans)
{
    printf("Function cbALoadQueue not supported\n");
    return NOERRORS;
}

int mcBoard::cbAOut(int Chan, int Gain, USHORT DataValue)
{
    printf("Function cbAOut not supported\n");
    return NOERRORS;
}

int mcBoard::cbAOutScan(int LowChan, int HighChan,
                        long Count, long *Rate, int Gain,
                        HGLOBAL MemHandle, int Options)
{
    printf("Function cbAOutScan not supported\n");
    return NOERRORS;
}

int mcBoard::cbCIn32(int CounterNum, ULONG *Count)
{
    printf("cbCIn32 is not supported\n");
    return NOERRORS;
}

int mcBoard::cbCLoad32(int RegNum, ULONG LoadValue)
{
    printf("Function cbCLoad32 not supported\n");
    return NOERRORS;
}

int mcBoard::cbCInScan(int FirstCtr,int LastCtr, LONG Count,
                       LONG *Rate, HGLOBAL MemHandle, ULONG Options)
{
    printf("Function cbCInScan not supported\n");
    return NOERRORS;
}

int mcBoard::cbCConfigScan(int CounterNum, int Mode,int DebounceTime,
                           int DebounceMode, int EdgeDetection,
                           int TickSize, int MappedChannel)
{
    printf("Function cbCConfigScan not supported\n");
    return NOERRORS;
}

// Digital I/O functions
int mcBoard::cbDBitOut(int PortType, int BitNum, USHORT BitValue)
{
    printf("Function cbDBitOut not supported\n");
    return NOERRORS;
}

int mcBoard::cbDConfigPort(int PortType, int Direction)
{
    printf("Function cbDConfigPort not supported\n");
    return NOERRORS;
}

int mcBoard::cbDConfigBit(int PortType, int BitNum, int Direction)
{
    printf("Function cbDConfigBit not supported\n");
    return NOERRORS;
}

int mcBoard::cbDIn(int PortType, USHORT *DataValue)
{
    printf("cbDIn is not supported\n");
    return NOERRORS;
}

int mcBoard::cbDIn32(int PortType, UINT *DataValue)
{
    printf("cbDIn32 is not supported\n");
    return NOERRORS;
}

int mcBoard::cbDOut(int PortType, USHORT DataValue)
{
    printf("cbDOut is not supported\n");
    return NOERRORS;
}

int mcBoard::cbDOut32(int PortType, UINT DataValue)
{
    printf("cbDOut32 is not supported\n");
    return NOERRORS;
}

// Pulse functions
int mcBoard::cbPulseOutStart(int TimerNum, double *Frequency,
                             double *DutyCycle, unsigned int PulseCount,
                             double *InitialDelay, int IdleState, int Options)
{
    printf("cbPulseOutStart is not supported\n");
    return NOERRORS;
}

int mcBoard::cbPulseOutStop(int TimerNum)
{
    printf("cbPulseOutStop is not supported\n");
    return NOERRORS;
}

// Temperature functions
int mcBoard::cbTIn(int Chan, int Scale, float *TempValue, int Options)
{
    printf("cbTIn is not supported\n");
    return NOERRORS;
}

// Voltage functions
int mcBoard::cbVIn(int Chan, int Range, float *DataValue, int Options)
{
    printf("cbVIn is not supported\n");
    return NOERRORS;
}

// Trigger functions
int mcBoard::cbSetTrigger(int TrigType, USHORT LowThreshold, USHORT HighThreshold)
{
    printf("cbSetTrigger is not supported\n");
    return NOERRORS;
}

// Daq functions
int mcBoard::cbDaqInScan(short *ChanArray, short *ChanTypeArray, short *GainArray, int ChanCount, long *Rate,
                         long *PretrigCount, long *TotalCount, HGLOBAL MemHandle, int Options)
{
    printf("cbDaqInScan is not supported\n");
    return NOERRORS;
}


int mcBoard::cbDaqSetTrigger(int TrigSource, int TrigSense, int TrigChan, int ChanType,
                             int Gain, float Level, float Variance, int TrigEvent)
{
    printf("cbDaqSetTrigger is not supported\n");
    return NOERRORS;
}

