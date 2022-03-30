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
    
    // Default values for thermocouple type
    for (int i=0; i<MAX_TEMP_CHANS; i++) {
        tcType_[i] = TC_J;
    }
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

int mcBoard::mapError(UlError error, const char *message)
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
          printf("mcBoard::mapError unmapped UlError=%d\n", error);
          cbwError = 2000 + error;
    }
    if (error != ERR_NO_ERROR) {
        ulGetErrMsg(error, errMsg);
        printf("mcBoard::mapError %s error=%d, message=%s\n", message, error, errMsg);
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
    if ((cbwOptions & BLOCKIO) == BLOCKIO)     options |= SO_BLOCKIO;
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
    long long infoValue;
    switch (InfoType) {
      case BOARDINFO:
        switch (ConfigItem) {
          case BIADTRIGCOUNT:
            aiScanTrigCount_ = ConfigVal;
            break;
          case BIADCHANTYPE:
            AiChanType chanType;
            mapAiChanType(ConfigVal, &chanType);
            error = ulAISetConfig(daqDeviceHandle_, AI_CFG_CHAN_TYPE, DevNum, chanType);
            // When changing to AI_TC mode may need to set the thermocouple type because it may only be cached
            if (chanType == AI_TC) {
                error = ulAISetConfig(daqDeviceHandle_, AI_CFG_CHAN_TC_TYPE, DevNum, tcType_[DevNum]);
            }
           
            break;
          case BICHANTCTYPE:
            // Cache the type
            tcType_[DevNum] = (TcType)ConfigVal;
            // We get an error writing the thermocouple type if the input mode is not thermocouple
            // so read the current channel type and only set the TC type if it is in thermocouple mode
            error = ulAIGetConfig(daqDeviceHandle_, AI_CFG_CHAN_TYPE, DevNum, &infoValue);
            if (infoValue == AI_TC) {
                // The enums for TcType are the same in cbw.h and uldaq.h, so we don't need to convert ConfigVal
                error = ulAISetConfig(daqDeviceHandle_, AI_CFG_CHAN_TC_TYPE, DevNum, tcType_[DevNum]);
            }
            break;
/* How do we set RTD type?
          case BICHANRTDTYPE:
            usbSetItem_USBTEMP_AI(hidDevice_, DevNum/2, USB_TEMP_AI_CONNECTION_TYPE, ConfigVal);
            break;
*/
          case BIDACRANGE:
            mapRange(ConfigVal, &aoRange_);
            break;
          case BIDACTRIGCOUNT:
            aoTrigCount_ = ConfigVal;
            break;
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
    return mapError(error, "ulAISetConfig()");
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
    bool scanInProgress = false;
    UlError error = ERR_NO_ERROR;
    switch (FunctionType) { 
      case AIFUNCTION:
        if (aiScanInProgress_) {
          error = ulAInScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
          scanInProgress = true;
        }
        break;
      case AOFUNCTION:
        if (aoScanInProgress_) {
          error = ulAOutScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
          scanInProgress = true;
        }
        break;
      case DIFUNCTION:
        if (diScanInProgress_) {
          error = ulDInScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
          scanInProgress = true;
        }
        break;
      case DOFUNCTION:
        if (doScanInProgress_) {
          error = ulDOutScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
          scanInProgress = true;
        }
        break;
      case CTRFUNCTION:
        if (ctrScanInProgress_) {
          error = ulCInScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
          scanInProgress = true;
        }
        break;
      case DAQIFUNCTION:
        if (daqiScanInProgress_) {
          error = ulDaqInScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
          scanInProgress = true;
        }
        break;
      case DAQOFUNCTION:
        if (daqoScanInProgress_) {
          error = ulDaqOutScanStatus(daqDeviceHandle_, &scanStatus, &xferStatus);
          scanInProgress = true;
        }
        break;
    }
    if (scanInProgress) {
        *Status = scanStatus;
        *CurCount = xferStatus.currentTotalCount;
        *CurIndex = xferStatus.currentIndex;
    } else {
        *Status = 0;
        *CurCount = 0;
        *CurIndex = 0;
    }
    return mapError(error, "cbGetIOStatus()");
}

int mcBoard::cbStopIOBackground(int FunctionType)
{
    UlError error = ERR_NO_ERROR;
    switch (FunctionType) { 
      case AIFUNCTION:
        error = ulAInScanStop(daqDeviceHandle_);
        aiScanInProgress_ = false;
        break;
      case AOFUNCTION:
        error = ulAOutScanStop(daqDeviceHandle_);
        aoScanInProgress_ = false;
        break;
      case DAQIFUNCTION:
        error = ulDaqInScanStop(daqDeviceHandle_);
        daqiScanInProgress_ = false;
        break;
      case DAQOFUNCTION:
        error = ulDaqOutScanStop(daqDeviceHandle_);
        daqoScanInProgress_ = false;
        break;
      case DIFUNCTION:
        error = ulDInScanStop(daqDeviceHandle_);
        diScanInProgress_ = false;
        break;
      case DOFUNCTION:
        error = ulDOutScanStop(daqDeviceHandle_);
        doScanInProgress_ = false;
        break;
      case CTRFUNCTION:
        error = ulCInScanStop(daqDeviceHandle_);
        ctrScanInProgress_ = false;
        break;
    }
    return mapError(error, "cbStopIOBackground()");
}

// Analog I/O functions
int mcBoard::cbAIn(int Chan, int Gain, unsigned short *DataValue)
{
    double data;
    Range range;
    mapRange(Gain, &range);
    UlError error = ulAIn(daqDeviceHandle_, Chan, aiInputMode_, range, AIN_FF_NOSCALEDATA, &data);
    *DataValue = (unsigned short)data;
    return mapError(error, "ulAIn()");
}

int mcBoard::cbAIn32(int Chan, int Gain, unsigned int *DataValue, int Options)
{
    double data;
    Range range;
    mapRange(Gain, &range);
    UlError error = ulAIn(daqDeviceHandle_, Chan, aiInputMode_, range, AIN_FF_NOSCALEDATA, &data);
    *DataValue = (unsigned int)data;
    return mapError(error, "ulAIn()");
}

int mcBoard::cbAInScan(int LowChan, int HighChan, long Count, long *Rate,
                       int Gain, void * MemHandle, int Options)
{
    Range range;
    UlError error;
    mapRange(Gain, &range);
    ScanOption scanOptions;
    mapScanOptions(Options, &scanOptions);
    double rate = (double) *Rate;
    int samplesPerChan = Count / (HighChan - LowChan + 1);

    error = ulAInSetTrigger(daqDeviceHandle_, triggerType_, 0, 0, 0, 0);    
    mapError(error, "ulAInSetTrigger()");

    error = ulAInScan(daqDeviceHandle_, LowChan, HighChan, aiInputMode_, range, samplesPerChan, &rate, 
                      scanOptions, AINSCAN_FF_DEFAULT, (double *)MemHandle);
    aiScanInProgress_ = true;
    return mapError(error, "ulAInScan()");
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
    return mapError(error, "ulAInLoadQueue()");
}

int mcBoard::cbAOut(int Chan, int Gain, unsigned short DataValue)
{
    Range range;
    mapRange(Gain, &range);
    UlError error = ulAOut(daqDeviceHandle_, Chan, range, AOUT_FF_NOSCALEDATA, (double)DataValue);
    return mapError(error, "ulAOut()");
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
    return mapError(error, "ulCIn()");
}

int mcBoard::cbCLoad32(int RegNum, unsigned int LoadValue)
{
    int counterNum = RegNum;
    CounterRegisterType registerType = CRT_LOAD;
    if (RegNum >= MINLIMITREG0 && RegNum <= MINLIMITREG7) {
        counterNum = RegNum - MINLIMITREG0;
        registerType = CRT_MIN_LIMIT;
    }
    else if (RegNum >= MAXLIMITREG0 && RegNum <= MAXLIMITREG7) {
        counterNum = RegNum - MAXLIMITREG0;
        registerType = CRT_MAX_LIMIT;
    }
    else if (RegNum >= OUTPUTVAL0REG0 && RegNum <= OUTPUTVAL0REG7) {
        counterNum = RegNum - OUTPUTVAL0REG0;
        registerType = CRT_OUTPUT_VAL0;
    }
    else if (RegNum >= OUTPUTVAL1REG0 && RegNum <= OUTPUTVAL1REG7) {
        counterNum = RegNum - OUTPUTVAL1REG0;
        registerType = CRT_OUTPUT_VAL1;
    }
    UlError error = ulCLoad(daqDeviceHandle_, counterNum, registerType, LoadValue);
    return mapError(error, "ulCLoad()");
}

int mcBoard::cbCInScan(int FirstCtr,int LastCtr, int Count,
                       int *Rate, void * MemHandle, unsigned int Options)
{
    int samplesPerCounter = 1;
    UlError error;
    double rate = *Rate;
    ScanOption scanOptions;
    int flags = CINSCAN_FF_DEFAULT;
    mapScanOptions(Options, &scanOptions);

//    error = ulCInSetTrigger(daqDeviceHandle_, triggerType_, 0, 0, 0, 0);
//    mapError(error, "ulCInSetTrigger");

    if (Options & CTR16BIT) flags |= CINSCAN_FF_CTR16_BIT;
    if (Options & CTR32BIT) flags |= CINSCAN_FF_CTR32_BIT;
    if (Options & CTR48BIT) flags |= CINSCAN_FF_CTR48_BIT;
    if (Options & CTR64BIT) flags |= CINSCAN_FF_CTR64_BIT;
    if (Options & CBW_NOCLEAR) flags |= CINSCAN_FF_NOCLEAR;
    error = ulCInScan(daqDeviceHandle_, FirstCtr, LastCtr, samplesPerCounter, &rate, scanOptions, (CInScanFlag)flags, (unsigned long long *)MemHandle);
    ctrScanInProgress_ = true;
    return mapError(error, "ulCInScan");
}

int mcBoard::cbCConfigScan(int CounterNum, int Mode,int DebounceTime,
                           int DebounceMode, int EdgeDetection,
                           int TickSize, int MappedChannel)
{
    CounterMeasurementType type = CMT_COUNT;
    int                                                   mode = CMM_DEFAULT;
    if (Mode & CLEAR_ON_READ)                             mode |= CMM_CLEAR_ON_READ;
    if (Mode & COUNTDOWN)                                 mode |= CMM_COUNT_DOWN;
    if (Mode & GATE_CONTROLS_DIR)                         mode |= CMM_GATE_CONTROLS_DIR;
    if (Mode & GATE_CLEARS_CTR)                           mode |= CMM_GATE_CLEARS_CTR;
    if (Mode & GATE_TRIG_SRC)                             mode |= CMM_GATE_TRIG_SRC;
    if (Mode & OUTPUT_ON)                                 mode |= CMM_OUTPUT_ON;
    if (Mode & OUTPUT_INITIAL_STATE_HIGH)                 mode |= CMM_OUTPUT_INITIAL_STATE_HIGH;
    if (Mode & NO_RECYCLE)                                mode |= CMM_NO_RECYCLE;
    if (Mode & RANGE_LIMIT_ON)                            mode |= CMM_RANGE_LIMIT_ON;
    if (Mode & GATING_ON)                                 mode |= CMM_GATING_ON;
    if (Mode & PERIOD & PERIOD_MODE_X1)                   mode |= CMM_PERIOD_X1;
    if (Mode & PERIOD & PERIOD_MODE_X10)                  mode |= CMM_PERIOD_X10;
    if (Mode & PERIOD & PERIOD_MODE_X100)                 mode |= CMM_PERIOD_X100;
    if (Mode & PERIOD & PERIOD_MODE_X1000)                mode |= CMM_PERIOD_X1000;
    if (Mode & PERIOD & PERIOD_MODE_GATING_ON)            mode |= CMM_PERIOD_GATING_ON;
    if (Mode & PERIOD & PERIOD_MODE_INVERT_GATE)          mode |= CMM_PERIOD_INVERT_GATE;
    if (Mode & PULSEWIDTH & PULSEWIDTH_MODE_GATING_ON)    mode |= CMM_PULSE_WIDTH_GATING_ON;
    if (Mode & PULSEWIDTH & PULSEWIDTH_MODE_INVERT_GATE)  mode |= CMM_PULSE_WIDTH_INVERT_GATE;
    if (Mode & TIMING & TIMING_MODE_INVERT_GATE)          mode |= CMM_TIMING_MODE_INVERT_GATE;
    if (Mode & ENCODER & ENCODER_MODE_X1)                 mode |= CMM_ENCODER_X1;
    if (Mode & ENCODER & ENCODER_MODE_X2)                 mode |= CMM_ENCODER_X2;
    if (Mode & ENCODER & ENCODER_MODE_X4)                 mode |= CMM_ENCODER_X4;
    if (Mode & ENCODER & ENCODER_MODE_LATCH_ON_Z)         mode |= CMM_ENCODER_LATCH_ON_Z;
    if (Mode & ENCODER & ENCODER_MODE_CLEAR_ON_Z_ON)      mode |= CMM_ENCODER_CLEAR_ON_Z;
    if (Mode & ENCODER & ENCODER_MODE_NO_RECYCLE_ON)      mode |= CMM_ENCODER_NO_RECYCLE;
    if (Mode & ENCODER & ENCODER_MODE_RANGE_LIMIT_ON)     mode |= CMM_ENCODER_RANGE_LIMIT_ON;
    //if (Mode & ENCODER_MODE_Z_ACTIVE_EDGE)              mode |= CMM_ENCODER_Z_ACTIVE_EDGE;
    //if (Mode & LATCH_ON_INDEX)                          mode |= CMM_LATCH_ON_INDEX;
    //if (Mode & PHB_CONTROLS_DIR)                        mode |= CMM_PHB_CONTROLS_DIR;
    if (Mode & DECREMENT_ON)                              mode |= CMM_DECREMENT_ON;
    CounterEdgeDetection edgeDetection;
    edgeDetection = (EdgeDetection == RISING_EDGE) ? CED_RISING_EDGE : CED_FALLING_EDGE;
    CounterTickSize tickSize;
    switch (TickSize) {
      case CTR_TICK20PT83ns:    tickSize = CTS_TICK_20PT83ns; break;
      case CTR_TICK208PT3ns:    tickSize = CTS_TICK_208PT3ns; break;
      case CTR_TICK2083PT3ns:   tickSize = CTS_TICK_2083PT3ns; break;
      case CTR_TICK20833PT3ns:  tickSize = CTS_TICK_20833PT3ns; break;
      case CTR_TICK20ns:        tickSize = CTS_TICK_20ns; break;
      case CTR_TICK200ns:       tickSize = CTS_TICK_200ns; break;
      case CTR_TICK2000ns:      tickSize = CTS_TICK_2000ns; break;
      case CTR_TICK20000ns:     tickSize = CTS_TICK_20000ns; break;
      default:
        printf("mcBoard::cbCConfigScan unknown TickSize=%d\n", TickSize);
        tickSize = CTS_TICK_20PT83ns;
        break;
    }
    CounterDebounceMode debounceMode;
    if (DebounceTime == CTR_DEBOUNCE_NONE)          debounceMode = CDM_NONE;
    if (DebounceMode == CTR_TRIGGER_AFTER_STABLE)   debounceMode = CDM_TRIGGER_AFTER_STABLE;
    if (DebounceMode == CTR_TRIGGER_BEFORE_STABLE)  debounceMode = CDM_TRIGGER_BEFORE_STABLE;
    CounterDebounceTime debounceTime;
    switch (DebounceTime) {
      case CTR_DEBOUNCE_NONE:     debounceTime = CDT_DEBOUNCE_0ns; debounceMode = CDM_NONE; break;
      case CTR_DEBOUNCE500ns:     debounceTime = CDT_DEBOUNCE_500ns;
      case CTR_DEBOUNCE1500ns:    debounceTime = CDT_DEBOUNCE_1500ns;
      case CTR_DEBOUNCE3500ns:    debounceTime = CDT_DEBOUNCE_3500ns;
      case CTR_DEBOUNCE7500ns:    debounceTime = CDT_DEBOUNCE_7500ns;
      case CTR_DEBOUNCE15500ns:   debounceTime = CDT_DEBOUNCE_15500ns;
      case CTR_DEBOUNCE31500ns:   debounceTime = CDT_DEBOUNCE_31500ns;
      case CTR_DEBOUNCE63500ns:   debounceTime = CDT_DEBOUNCE_63500ns;
      case CTR_DEBOUNCE127500ns:  debounceTime = CDT_DEBOUNCE_127500ns;
      case CTR_DEBOUNCE100us:     debounceTime = CDT_DEBOUNCE_100us;
      case CTR_DEBOUNCE300us:     debounceTime = CDT_DEBOUNCE_300us;
      case CTR_DEBOUNCE700us:     debounceTime = CDT_DEBOUNCE_700us;
      case CTR_DEBOUNCE1500us:    debounceTime = CDT_DEBOUNCE_1500us;
      case CTR_DEBOUNCE3100us:    debounceTime = CDT_DEBOUNCE_3100us;
      case CTR_DEBOUNCE6300us:    debounceTime = CDT_DEBOUNCE_6300us;
      case CTR_DEBOUNCE12700us:   debounceTime = CDT_DEBOUNCE_12700us;
      case CTR_DEBOUNCE25500us:   debounceTime = CDT_DEBOUNCE_25500us;
      default:
        printf("mcBoard::cbCConfigScan unknown DebounceTime=%d\n", DebounceTime);
        break;
    }

    UlError error = ulCConfigScan(daqDeviceHandle_, CounterNum, type,  (CounterMeasurementMode) mode,
					                        edgeDetection, tickSize, debounceMode, debounceTime, CF_DEFAULT);
	  return mapError(error, "ulCConfigScan");
}

// Digital I/O functions
int mcBoard::cbDBitOut(int PortType, int BitNum, unsigned short BitValue)
{
    // cbw.h and uldaq.h use the same enums for PortType
    UlError error = ulDBitOut(daqDeviceHandle_, (DigitalPortType)PortType, BitNum, BitValue);
    return mapError(error, "ulDBitOut()");
}

int mcBoard::cbDConfigPort(int PortType, int Direction)
{
    DigitalDirection dir = (Direction == DIGITALOUT) ? DD_OUTPUT : DD_INPUT;
    UlError error = ulDConfigPort(daqDeviceHandle_, (DigitalPortType)PortType, dir);
    return mapError(error, "ulDConfigPort()");
}

int mcBoard::cbDConfigBit(int PortType, int BitNum, int Direction)
{
    DigitalDirection dir = (Direction == DIGITALOUT) ? DD_OUTPUT : DD_INPUT;
    UlError error = ulDConfigBit(daqDeviceHandle_, (DigitalPortType)PortType, BitNum, dir);
    return mapError(error, "ulDConfigBit()");
}

int mcBoard::cbDIn(int PortType, unsigned short *DataValue)
{
    unsigned long long data;
    UlError error = ulDIn(daqDeviceHandle_, (DigitalPortType)PortType, &data);
    *DataValue = (unsigned short)data;
    return mapError(error, "ulDIn()");
}

int mcBoard::cbDIn32(int PortType, unsigned int *DataValue)
{
    unsigned long long data;
    UlError error = ulDIn(daqDeviceHandle_, (DigitalPortType)PortType, &data);
    *DataValue = (unsigned int)data;
    return mapError(error, "ulDIn()");
}

int mcBoard::cbDOut(int PortType, unsigned short DataValue)
{
    UlError error = ulDOut(daqDeviceHandle_, (DigitalPortType)PortType, DataValue);
    return mapError(error, "ulDOut()");
}

int mcBoard::cbDOut32(int PortType, unsigned int DataValue)
{
    UlError error = ulDOut(daqDeviceHandle_, (DigitalPortType)PortType, DataValue);
    return mapError(error, "ulDOut()");
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
    return mapError(error, "ulTmrPulseOutStart()");
}

int mcBoard::cbPulseOutStop(int TimerNum)
{
    UlError error = ulTmrPulseOutStop(daqDeviceHandle_, TimerNum);
    return mapError(error, "ulTmrPulseOutStop()");
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
    return mapError(error, "ulTIn()");
}

// Voltage functions
int mcBoard::cbVIn(int Chan, int Gain, float *DataValue, int Options)
{
    double data;
    Range range;
    mapRange(Gain, &range);
    UlError error = ulAIn(daqDeviceHandle_, Chan, aiInputMode_, range, AIN_FF_DEFAULT, &data);
    *DataValue = (float)data;
    return mapError(error, "ulAIn()");
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
