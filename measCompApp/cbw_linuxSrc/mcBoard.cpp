#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mcBoard.h"

mcBoard::mcBoard(DaqDeviceDescriptor daqDeviceDescriptor, DaqDeviceHandle daqDeviceHandle)
 :  daqDeviceDescriptor_(daqDeviceDescriptor),
    daqDeviceHandle_(daqDeviceHandle),
    aiInputMode_(AI_SINGLE_ENDED),
    aiFlags_(AIN_FF_NOSCALEDATA),
    aoFlags_(AOUT_FF_NOSCALEDATA)
{
    UlError error;
    long long infoValue;
    
    error = ulConnectDaqDevice(daqDeviceHandle_);
printf("mcBoard::mcBoard ulConnectDaqDevice returned %d\n", error);
    strcpy(boardName_, daqDeviceDescriptor_.productName);
    biBoardType_    = daqDeviceDescriptor.productId;
    biNumTempChans_ = 0;
    diInMask_       = 0;
    diOutMask_      = 0;

    error = ulAIGetInfo(daqDeviceHandle_, AI_INFO_NUM_CHANS_BY_TYPE, AI_VOLTAGE, &infoValue);
    biNumADCChans_ = infoValue;
    error = ulAIGetInfo(daqDeviceHandle_, AI_INFO_NUM_CHANS_BY_TYPE, AI_TC, &infoValue);
    biNumTempChans_ = infoValue;
    error = ulAIGetInfo(daqDeviceHandle_, AI_INFO_RESOLUTION, 0, &infoValue);
    biADCRes_ = infoValue;
    error = ulAOGetInfo(daqDeviceHandle_, AO_INFO_NUM_CHANS, 0, &infoValue);
    biNumDACChans_ = infoValue;
    error = ulAOGetInfo(daqDeviceHandle_, AO_INFO_RESOLUTION, 0, &infoValue);
    biDACRes_ = infoValue;
    error = ulDIOGetInfo(daqDeviceHandle_, DIO_INFO_NUM_PORTS, 0, &infoValue);
    biDInNumDevs_ = infoValue;
    error = ulDIOGetInfo(daqDeviceHandle_, DIO_INFO_PORT_TYPE, 0, &infoValue);
    diDevType_ = infoValue;
    error = ulDIOGetInfo(daqDeviceHandle_, DIO_INFO_NUM_BITS, 0, &infoValue);
    diNumBits_ = infoValue;
}


int mcBoard::gainToRange(int Gain, Range *range)
{
    // Converts cbw Gain to uldaq Range
    switch (Gain) {
      case CBW_BIP60VOLTS:    *range = BIP60VOLTS; break;
      case CBW_BIP30VOLTS:    *range = BIP30VOLTS; break;
      case CBW_BIP20VOLTS:    *range = BIP20VOLTS; break;
      case CBW_BIP15VOLTS:    *range = BIP15VOLTS; break;
      case CBW_BIP10VOLTS:    *range = BIP10VOLTS; break;
      case CBW_BIP5VOLTS:     *range = BIP5VOLTS; break;
      case CBW_BIP4VOLTS:     *range = BIP4VOLTS; break;
      case CBW_BIP2PT5VOLTS:  *range = BIP2PT5VOLTS; break;
      case CBW_BIP2VOLTS:     *range = BIP2VOLTS; break;
      case CBW_BIP1PT25VOLTS: *range = BIP1PT25VOLTS; break;
      case CBW_BIP1VOLTS:     *range = BIP1VOLTS; break;
      case CBW_BIPPT625VOLTS: *range = BIPPT625VOLTS; break;
      case CBW_BIPPT5VOLTS:   *range = BIPPT5VOLTS; break;
      case CBW_BIPPT25VOLTS:  *range = BIPPT25VOLTS; break;
      case CBW_BIPPT2VOLTS:   *range = BIPPT2VOLTS; break;
      case CBW_BIPPT1VOLTS:   *range = BIPPT1VOLTS; break;
      case CBW_BIPPT05VOLTS:  *range = BIPPT05VOLTS; break;
      case CBW_BIPPT01VOLTS:  *range = BIPPT01VOLTS; break;
      case CBW_BIPPT005VOLTS: *range = BIPPT005VOLTS; break;
//      case CBW_BIP1PT67VOLTS: *range = BIP1PT67VOLTS; break;
      case CBW_BIPPT312VOLTS: *range = BIPPT312VOLTS; break;
      case CBW_BIPPT156VOLTS: *range = BIPPT156VOLTS; break;
      case CBW_BIPPT125VOLTS: *range = BIPPT125VOLTS; break;
      case CBW_BIPPT078VOLTS: *range = BIPPT078VOLTS; break;

      case CBW_UNI10VOLTS:    *range = UNI10VOLTS; break;
      case CBW_UNI5VOLTS:     *range = UNI5VOLTS; break;
      case CBW_UNI4VOLTS:     *range = UNI4VOLTS; break;
      case CBW_UNI2PT5VOLTS:  *range = UNI2PT5VOLTS; break;
      case CBW_UNI2VOLTS:     *range = UNI2VOLTS; break;
//      case CBW_UNI1PT67VOLTS: *range = UNI1PT67VOLTS; break;
      case CBW_UNI1PT25VOLTS: *range = UNI1PT25VOLTS; break;
      case CBW_UNI1VOLTS:     *range = UNI1VOLTS; break;
      case CBW_UNIPT5VOLTS:   *range = UNIPT5VOLTS; break;
      case CBW_UNIPT25VOLTS:  *range = UNIPT25VOLTS; break;
      case CBW_UNIPT2VOLTS:   *range = UNIPT2VOLTS; break;
      case CBW_UNIPT1VOLTS:   *range = UNIPT1VOLTS; break;
      case CBW_UNIPT05VOLTS:  *range = UNIPT05VOLTS; break;
//      case CBW_UNIPT02VOLTS:  *range = UNIPT02VOLTS; break;
      case CBW_UNIPT01VOLTS:  *range = UNIPT01VOLTS; break;

      case CBW_MA0TO20:       *range = MA0TO20; break;
      default:
          printf("mcBoard::GainToRange unsupported Gain=%d\n", Gain);
          return BADRANGE;
    }
    return NOERRORS;
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
    ScanStatus scanStatus;
    TransferStatus xferStatus;
    UlError error = ERR_NO_ERROR;
    switch (FunctionType) { 
      case AIFUNCTION:
        error = ulDaqInScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
        break;
      case AOFUNCTION:
        error = ulDaqOutScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
        break;
    }
    *Status = scanStatus;
    *CurCount = xferStatus.currentTotalCount;
    *CurIndex = xferStatus.currentIndex;
    return error;
}

int mcBoard::cbStopIOBackground(int FunctionType)
{
    UlError error = ERR_NO_ERROR;
    switch (FunctionType) { 
      case AIFUNCTION:
        error = ulDaqInScanStop(daqDeviceHandle_);
        break;
      case AOFUNCTION:
        error = ulDaqOutScanStop(daqDeviceHandle_);
        break;
    }
    return error;
}

// Analog I/O functions
int mcBoard::cbAIn(int Chan, int Gain, unsigned short *DataValue)
{
    double data;
    Range range;
    gainToRange(Gain, &range);
    UlError error = ulAIn(daqDeviceHandle_, Chan, aiInputMode_, range, AIN_FF_NOSCALEDATA, &data);
    *DataValue = (unsigned short)data;
    return error;
}

int mcBoard::cbAIn32(int Chan, int Gain, unsigned int *DataValue, int Options)
{
    double data;
    Range range;
    gainToRange(Gain, &range);
    UlError error = ulAIn(daqDeviceHandle_, Chan, aiInputMode_, range, AIN_FF_NOSCALEDATA, &data);
    *DataValue = (unsigned int)data;
    return error;
}

int mcBoard::cbAInScan(int LowChan, int HighChan, long Count, long *Rate,
                       int Gain, void * MemHandle, int Options)
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

int mcBoard::cbAOut(int Chan, int Gain, unsigned short DataValue)
{
    Range range;
    gainToRange(Gain, &range);
    UlError error = ulAOut(daqDeviceHandle_, Chan, range, AOUT_FF_NOSCALEDATA, (double)DataValue);
    return error;
}

int mcBoard::cbAOutScan(int LowChan, int HighChan,
                        long Count, long *Rate, int Gain,
                        void * MemHandle, int Options)
{
    printf("Function cbAOutScan not supported\n");
    return NOERRORS;
}

int mcBoard::cbCIn32(int CounterNum, unsigned int *Count)
{
    unsigned long long data;
    UlError error = ulCIn(daqDeviceHandle_, CounterNum, &data);
    *Count = (unsigned int)data;
    return error;
}

int mcBoard::cbCLoad32(int RegNum, unsigned int LoadValue)
{
    // Hardcoding CTR_COUNT, may need to be CRT_LOAD?
    UlError error = ulCLoad(daqDeviceHandle_, RegNum, CRT_LOAD, LoadValue);
printf("mcBoard::cbCLoad32 wrote to CRT_LOAD register error=%d\n", error);
    return error;
}

int mcBoard::cbCInScan(int FirstCtr,int LastCtr, int Count,
                       int *Rate, void * MemHandle, unsigned int Options)
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
int mcBoard::cbDBitOut(int PortType, int BitNum, unsigned short BitValue)
{
    // cbw.h and uldaq.h use the same enums for PortType
    UlError error = ulDBitOut(daqDeviceHandle_, (DigitalPortType)PortType, BitNum, BitValue);
    return error;
}

int mcBoard::cbDConfigPort(int PortType, int Direction)
{
    DigitalDirection dir = (Direction == DIGITALOUT) ? DD_OUTPUT : DD_INPUT;
    UlError error = ulDConfigPort(daqDeviceHandle_, (DigitalPortType)PortType, dir);
    return error;
}

int mcBoard::cbDConfigBit(int PortType, int BitNum, int Direction)
{
    DigitalDirection dir = (Direction == DIGITALOUT) ? DD_OUTPUT : DD_INPUT;
    UlError error = ulDConfigBit(daqDeviceHandle_, (DigitalPortType)PortType, BitNum, dir);
    return error;
}

int mcBoard::cbDIn(int PortType, unsigned short *DataValue)
{
    unsigned long long data;
    UlError error = ulDIn(daqDeviceHandle_, (DigitalPortType)PortType, &data);
    *DataValue = (unsigned short)data;
    return error;
}

int mcBoard::cbDIn32(int PortType, unsigned int *DataValue)
{
    unsigned long long data;
    UlError error = ulDIn(daqDeviceHandle_, (DigitalPortType)PortType, &data);
    *DataValue = (unsigned int)data;
    return error;
}

int mcBoard::cbDOut(int PortType, unsigned short DataValue)
{
    UlError error = ulDOut(daqDeviceHandle_, (DigitalPortType)PortType, DataValue);
    return error;
}

int mcBoard::cbDOut32(int PortType, unsigned int DataValue)
{
    UlError error = ulDOut(daqDeviceHandle_, (DigitalPortType)PortType, DataValue);
    return error;
}

// Pulse functions
int mcBoard::cbPulseOutStart(int TimerNum, double *Frequency,
                             double *DutyCycle, unsigned int PulseCount,
                             double *InitialDelay, int IdleState, int Options)
{
    TmrIdleState idle = (IdleState == IDLE_LOW) ? TMRIS_LOW : TMRIS_HIGH;
    // For now we don't handle any options
    PulseOutOption pulseOptions = (PulseOutOption)Options;
    UlError error = ulTmrPulseOutStart(daqDeviceHandle_, TimerNum, Frequency, DutyCycle, PulseCount, InitialDelay,
                                       idle, pulseOptions);
    return error;
}

int mcBoard::cbPulseOutStop(int TimerNum)
{
    UlError error = ulTmrPulseOutStop(daqDeviceHandle_, TimerNum);
    return error;
}

// Temperature functions
int mcBoard::cbTIn(int Chan, int Scale, float *TempValue, int Options)
{
    double data;
    TempScale tempScale;
    // cbTin has a filter option but ulTin does not?
    TInFlag flags = TIN_FF_DEFAULT;
    switch (Scale) {
        case CELSIUS:     tempScale = TS_CELSIUS; break;
        case FAHRENHEIT:  tempScale = TS_FAHRENHEIT; break;
        case KELVIN:      tempScale = TS_KELVIN; break;
        case VOLTS:       tempScale = TS_VOLTS; break;
        case NOSCALE:     tempScale = TS_NOSCALE; break;
        default:
            printf("mcBoard::cbTIn unsupported Scale=%d\n", Scale);
            return BADTEMPSCALE;
    }
    UlError error = ulTIn(daqDeviceHandle_, Chan, tempScale, flags, &data);
    *TempValue = (float)data;
    return error;
}

// Voltage functions
int mcBoard::cbVIn(int Chan, int Gain, float *DataValue, int Options)
{
    double data;
    Range range;
    gainToRange(Gain, &range);
    UlError error = ulAIn(daqDeviceHandle_, Chan, aiInputMode_, range, AIN_FF_DEFAULT, &data);
    *DataValue = (float)data;
    return error;
}

// Trigger functions
int mcBoard::cbSetTrigger(int TrigType, unsigned short LowThreshold, unsigned short HighThreshold)
{
    printf("cbSetTrigger is not supported\n");
    return NOERRORS;
}

// Daq functions
int mcBoard::cbDaqInScan(short *ChanArray, short *ChanTypeArray, short *GainArray, int ChanCount, long *Rate,
                         long *PretrigCount, long *TotalCount, void * MemHandle, int Options)
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

