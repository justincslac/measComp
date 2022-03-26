#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mcBoard.h"

mcBoard::mcBoard(DaqDeviceDescriptor daqDeviceDescriptor, DaqDeviceHandle daqDeviceHandle)
 :  daqDeviceDescriptor_(daqDeviceDescriptor),
    daqDeviceHandle_(daqDeviceHandle),
    aiInputMode_(AI_SINGLE_ENDED),
    aiFlags_(AIN_FF_NOSCALEDATA),
    aiScanQueue_(0),
    aoFlags_(AOUT_FF_NOSCALEDATA),
    aiScanInProgress_(false),
    aoScanInProgress_(false),
    daqiScanInProgress_(false),
    daqoScanInProgress_(false),
    diScanInProgress_(false),
    doScanInProgress_(false),
    ctrScanInProgress_(false)
{
    UlError error;
    long long infoValue;
    
    error = ulConnectDaqDevice(daqDeviceHandle_);
    strcpy(boardName_, daqDeviceDescriptor_.productName);
    biBoardType_    = daqDeviceDescriptor.productId;
    diInMask_       = 0;
    diOutMask_      = 0;

    biNumADCChans_ = 0;
    error = ulAIGetInfo(daqDeviceHandle_, AI_INFO_NUM_CHANS_BY_TYPE, AI_VOLTAGE, &infoValue);
    if (error == ERR_NO_ERROR) biNumADCChans_ = infoValue;

    biNumTempChans_ = 0;
    error = ulAIGetInfo(daqDeviceHandle_, AI_INFO_NUM_CHANS_BY_TYPE, AI_TC, &infoValue);
    if (error == ERR_NO_ERROR) biNumTempChans_ = infoValue;

    biADCRes_ = 0;
    error = ulAIGetInfo(daqDeviceHandle_, AI_INFO_RESOLUTION, 0, &infoValue);
    if (error == ERR_NO_ERROR) biADCRes_ = infoValue;

    biNumDACChans_ = 0;
    error = ulAOGetInfo(daqDeviceHandle_, AO_INFO_NUM_CHANS, 0, &infoValue);
    if (error == ERR_NO_ERROR) biNumDACChans_ = infoValue;

    biDACRes_ = 0;
    error = ulAOGetInfo(daqDeviceHandle_, AO_INFO_RESOLUTION, 0, &infoValue);
    if (error == ERR_NO_ERROR) biDACRes_ = infoValue;

    biDInNumDevs_ = 0;
    error = ulDIOGetInfo(daqDeviceHandle_, DIO_INFO_NUM_PORTS, 0, &infoValue);
    if (error == ERR_NO_ERROR) biDInNumDevs_ = infoValue;

    diDevType_ = 0;
    error = ulDIOGetInfo(daqDeviceHandle_, DIO_INFO_PORT_TYPE, 0, &infoValue);
    if (error == ERR_NO_ERROR) diDevType_ = infoValue;

    diNumBits_ = 0;
    error = ulDIOGetInfo(daqDeviceHandle_, DIO_INFO_NUM_BITS, 0, &infoValue);
    if (error == ERR_NO_ERROR) diNumBits_ = infoValue;
}


int mcBoard::mapRange(int Gain, Range *range)
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
          printf("mcBoard::mapRange unsupported Gain=%d\n", Gain);
          return BADRANGE;
    }
    return NOERRORS;
}

int mcBoard::mapError(UlError error)
{
    // Converts UlError to cbw error
    int cbwError;
    char errMsg[ERR_MSG_LEN];
    switch (error) {
      case ERR_NO_ERROR:              cbwError = NOERRORS; break;
//      case ERR_UNHANDLED_EXCEPTION:   cbwError = ; break;
      case ERR_BAD_DEV_HANDLE:        cbwError = BADBOARD; break;
      case ERR_BAD_DEV_TYPE:          cbwError = BADBOARD; break;
      case ERR_USB_DEV_NO_PERMISSION: cbwError = NO_USB_BOARD; break;
      case ERR_USB_INTERFACE_CLAIMED: cbwError = NO_USB_BOARD; break;
      case ERR_DEV_NOT_FOUND:         cbwError = NOBOARDNAMEFILE; break;
      case ERR_DEV_NOT_CONNECTED:     cbwError = NETCONNECTIONFAILED; break;
      case ERR_OPEN_CONNECTION:       cbwError = OPENCONNECTION; break;
      default:
          printf("mcBoard::mapError unsupported UlError=%d\n", error);
          cbwError = 2000 + error;
    }
    if (error != ERR_NO_ERROR) {
        ulGetErrMsg(error, errMsg);
        printf("mcBoard::mapError error=%d, message=%s\n", error, errMsg);
    }
    return cbwError;
}

int mcBoard::mapTriggerType(int cbwTriggerType, TriggerType *triggerType)
{
    // Converts cbw trigger type to uldaq trigger type;
    switch (cbwTriggerType) {
      case CBW_TRIG_POS_EDGE:       *triggerType = TRIG_POS_EDGE; break;
      case CBW_TRIG_NEG_EDGE:       *triggerType = TRIG_NEG_EDGE; break;
      case CBW_TRIG_HIGH:           *triggerType = TRIG_HIGH; break;
      case CBW_TRIG_LOW:            *triggerType = TRIG_LOW; break;
      case CBW_GATE_HIGH:           *triggerType = GATE_HIGH; break;
      case CBW_GATE_LOW:            *triggerType = GATE_LOW; break;
      case CBW_TRIG_RISING:         *triggerType = TRIG_RISING; break;
      case CBW_TRIG_FALLING:        *triggerType = TRIG_FALLING; break;
      case CBW_TRIGABOVE:           *triggerType = TRIG_ABOVE; break;
      case CBW_TRIGBELOW:           *triggerType = TRIG_BELOW; break;
      case CBW_GATE_ABOVE:          *triggerType = GATE_ABOVE; break;
      case CBW_GATE_BELOW:          *triggerType = GATE_BELOW; break;
      case CBW_GATE_IN_WINDOW:      *triggerType = GATE_IN_WINDOW; break;
      case CBW_GATE_OUT_WINDOW:     *triggerType = GATE_OUT_WINDOW; break;
      case CBW_TRIG_PATTERN_EQ:     *triggerType = TRIG_PATTERN_EQ; break;
      case CBW_TRIG_PATTERN_NE:     *triggerType = TRIG_PATTERN_NE; break;
      case CBW_TRIG_PATTERN_ABOVE:  *triggerType = TRIG_PATTERN_ABOVE; break;
      case CBW_TRIG_PATTERN_BELOW:  *triggerType = TRIG_PATTERN_BELOW; break;
      default:
          printf("mcBoard::mapTriggerType unsupported cbwTriggerType=%d\n", cbwTriggerType);
          *triggerType = TRIG_NONE;
          return BADTRIGTYPE;
    }
    return NOERRORS;
}

int mcBoard::mapAiChanType(int cbwChanType, AiChanType *chanType)
{
    // Converts cbw chan type to uldaq AiChanType;
    switch (cbwChanType) {
      case AI_CHAN_TYPE_VOLTAGE:  *chanType = AI_VOLTAGE; break;
      case AI_CHAN_TYPE_TC:       *chanType = AI_TC; break;
      default:
          printf("mcBoard::mapAiChanType unsupported cbwChanType=%d\n", cbwChanType);
          *chanType = AI_VOLTAGE;
          return BADCHANTYPE;
    }
    return NOERRORS;
}

int mcBoard::mapScanOptions(int cbwOptions, ScanOption *scanOptions)
{
    // Converts cbw scan options to uldaq ScanOption;
    int options = SO_DEFAULTIO;
    if (cbwOptions & SINGLEIO)    options |= SO_SINGLEIO;
    if (cbwOptions & BLOCKIO)     options |= SO_BLOCKIO;
    if (cbwOptions & BURSTIO)     options |= SO_BURSTIO;
    if (cbwOptions & CONTINUOUS)  options |= SO_CONTINUOUS;
    if (cbwOptions & EXTCLOCK)    options |= SO_EXTCLOCK;
    if (cbwOptions & EXTTRIGGER)  options |= SO_EXTTRIGGER;
    if (cbwOptions & RETRIGMODE)  options |= SO_RETRIGGER;
    if (cbwOptions & BURSTMODE)   options |= SO_BURSTMODE;
//    if (cbwOptions & XXX)       options |= SO_PACEROUT;
//    if (cbwOptions & XXX)       options |= SO_EXTTIMEBASE;
//    if (cbwOptions & XXX)       options |= SO_TIMEBASEOUT;
  
    *scanOptions = (ScanOption) options;
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
    UlError error = ERR_NO_ERROR;
    switch (InfoType) {
      case BOARDINFO:
        switch (ConfigItem) {
          case BIADTRIGCOUNT:
            aiScanTrigCount_ = ConfigVal;
            break;
          case BIADCHANTYPE:
            AiChanType chanType;
            mapAiChanType(ConfigVal, &chanType);
            error = ulAISetConfig(daqDeviceHandle_, AI_CFG_CHAN_TC_TYPE, DevNum, chanType);
          case BICHANTCTYPE:
            // The enums for TcType are the same in cbw.h and uldaq.h, so we don't need to convert ConfigVal
            error = ulAISetConfig(daqDeviceHandle_, AI_CFG_CHAN_TC_TYPE, DevNum, ConfigVal);
            break;
/* How do we set RTD type?
          case BICHANRTDTYPE:
            usbSetItem_USBTEMP_AI(hidDevice_, DevNum/2, USB_TEMP_AI_CONNECTION_TYPE, ConfigVal);
            break;
*/
          case BIDACRANGE:
            mapRange(ConfigVal, &aoRange_);
          default:
            printf("mcBoard::cbSetConfig error unknown ConfigItem %d\n", ConfigItem);
            return BADCONFIGITEM;
            break;
        }
        break;
      default:
        printf("mcBoard::cbSetConfig error unknown InfoType %d\n", InfoType);
        return BADCONFIGTYPE;
        break;
    }
    return mapError(error);
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
        if (aiScanInProgress_) {
            error = ulAInScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
        }
        break;
      case AOFUNCTION:
        if (aoScanInProgress_) {
          error = ulAOutScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
        }
        break;
      case DIFUNCTION:
        if (diScanInProgress_) {
            error = ulDInScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
        }
        break;
      case DOFUNCTION:
        if (doScanInProgress_) {
          error = ulDOutScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
        }
        break;
      case CTRFUNCTION:
        if (ctrScanInProgress_) {
          error = ulCInScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
        }
        break;
      case DAQIFUNCTION:
        if (daqiScanInProgress_) {
          error = ulDaqInScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
        }
        break;
      case DAQOFUNCTION:
        if (daqoScanInProgress_) {
          error = ulDaqOutScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
        }
        break;
    }
    *Status = scanStatus;
    *CurCount = xferStatus.currentTotalCount;
    *CurIndex = xferStatus.currentIndex;
    return mapError(error);
}

int mcBoard::cbStopIOBackground(int FunctionType)
{
    UlError error = ERR_NO_ERROR;
    switch (FunctionType) { 
      case AIFUNCTION:
        error = ulDaqInScanStop(daqDeviceHandle_);
        aiScanInProgress_ = false;
        break;
      case AOFUNCTION:
        error = ulDaqOutScanStop(daqDeviceHandle_);
        aoScanInProgress_ = false;
        break;
    }
    return mapError(error);
}

// Analog I/O functions
int mcBoard::cbAIn(int Chan, int Gain, unsigned short *DataValue)
{
    double data;
    Range range;
    mapRange(Gain, &range);
    UlError error = ulAIn(daqDeviceHandle_, Chan, aiInputMode_, range, AIN_FF_NOSCALEDATA, &data);
    *DataValue = (unsigned short)data;
    return mapError(error);
}

int mcBoard::cbAIn32(int Chan, int Gain, unsigned int *DataValue, int Options)
{
    double data;
    Range range;
    mapRange(Gain, &range);
    UlError error = ulAIn(daqDeviceHandle_, Chan, aiInputMode_, range, AIN_FF_NOSCALEDATA, &data);
    *DataValue = (unsigned int)data;
    return mapError(error);
}

int mcBoard::cbAInScan(int LowChan, int HighChan, long Count, long *Rate,
                       int Gain, void * MemHandle, int Options)
{
    Range range;
    mapRange(Gain, &range);
    ScanOption scanOptions;
    mapScanOptions(Options, &scanOptions);
    double rate = (double) *Rate;
    int samplesPerChan = Count / (HighChan - LowChan + 1);

    UlError error = ulAInScan(daqDeviceHandle_, LowChan, HighChan, aiInputMode_, range, samplesPerChan, &rate, 
                              scanOptions, AINSCAN_FF_DEFAULT, (double *)MemHandle);
    aiScanInProgress_ = true;
    return mapError(error);
}

int mcBoard::cbAInputMode(int InputMode)
{
    aiInputMode_ = (InputMode == DIFFERENTIAL) ? AI_DIFFERENTIAL : AI_SINGLE_ENDED;
    return NOERRORS;
}

int mcBoard::cbALoadQueue(short *ChanArray, short *GainArray, int NumChans)
{
    if (aiScanQueue_) free(aiScanQueue_);
    aiScanQueue_ = (AiQueueElement *)calloc(NumChans, sizeof(AiQueueElement));
    for (int i=0; i<NumChans; i++) {
        aiScanQueue_[i].channel = ChanArray[i];
        aiScanQueue_[i].inputMode = aiInputMode_;
        mapRange(GainArray[i], &aiScanQueue_[i].range);
    }
    UlError error = ulAInLoadQueue(daqDeviceHandle_, aiScanQueue_, NumChans);
    return mapError(error);
}

int mcBoard::cbAOut(int Chan, int Gain, unsigned short DataValue)
{
    Range range;
    mapRange(Gain, &range);
    UlError error = ulAOut(daqDeviceHandle_, Chan, range, AOUT_FF_NOSCALEDATA, (double)DataValue);
    return mapError(error);
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
    return mapError(error);
}

int mcBoard::cbCLoad32(int RegNum, unsigned int LoadValue)
{
    // Hardcoding CTR_COUNT, may need to be CRT_LOAD?
    UlError error = ulCLoad(daqDeviceHandle_, RegNum, CRT_LOAD, LoadValue);
    return mapError(error);
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
    return mapError(error);
}

int mcBoard::cbDConfigPort(int PortType, int Direction)
{
    DigitalDirection dir = (Direction == DIGITALOUT) ? DD_OUTPUT : DD_INPUT;
    UlError error = ulDConfigPort(daqDeviceHandle_, (DigitalPortType)PortType, dir);
    return mapError(error);
}

int mcBoard::cbDConfigBit(int PortType, int BitNum, int Direction)
{
    DigitalDirection dir = (Direction == DIGITALOUT) ? DD_OUTPUT : DD_INPUT;
    UlError error = ulDConfigBit(daqDeviceHandle_, (DigitalPortType)PortType, BitNum, dir);
    return mapError(error);
}

int mcBoard::cbDIn(int PortType, unsigned short *DataValue)
{
    unsigned long long data;
    UlError error = ulDIn(daqDeviceHandle_, (DigitalPortType)PortType, &data);
    *DataValue = (unsigned short)data;
    return mapError(error);
}

int mcBoard::cbDIn32(int PortType, unsigned int *DataValue)
{
    unsigned long long data;
    UlError error = ulDIn(daqDeviceHandle_, (DigitalPortType)PortType, &data);
    *DataValue = (unsigned int)data;
    return mapError(error);
}

int mcBoard::cbDOut(int PortType, unsigned short DataValue)
{
    UlError error = ulDOut(daqDeviceHandle_, (DigitalPortType)PortType, DataValue);
    return mapError(error);
}

int mcBoard::cbDOut32(int PortType, unsigned int DataValue)
{
    UlError error = ulDOut(daqDeviceHandle_, (DigitalPortType)PortType, DataValue);
    return mapError(error);
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
    return mapError(error);
}

int mcBoard::cbPulseOutStop(int TimerNum)
{
    UlError error = ulTmrPulseOutStop(daqDeviceHandle_, TimerNum);
    return mapError(error);
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
    return mapError(error);
}

// Voltage functions
int mcBoard::cbVIn(int Chan, int Gain, float *DataValue, int Options)
{
    double data;
    Range range;
    mapRange(Gain, &range);
    UlError error = ulAIn(daqDeviceHandle_, Chan, aiInputMode_, range, AIN_FF_DEFAULT, &data);
    *DataValue = (float)data;
    return mapError(error);
}

// Trigger functions
int mcBoard::cbSetTrigger(int TrigType, unsigned short LowThreshold, unsigned short HighThreshold)
{
    // In uldaq there are separate calls for ulDaqInSetTrigger, ulDaqOutSetTrigger, etc.
    // We just cache the information here and then call those functions when starting the appropriate scan
    return mapTriggerType(TrigType, &triggerType_);
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
