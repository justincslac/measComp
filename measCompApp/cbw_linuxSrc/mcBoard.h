#ifndef mcBoardInclude
#define mcBoardInclude

#include <uldaq.h>
#include "cbw_linux_noconflict.h"

#define MAX_TEMP_CHANS 32

class mcBoard {
public:
    mcBoard(DaqDeviceDescriptor daqDeviceDescriptor, DaqDeviceHandle daqDeviceHandle);

    int mapRange(int Gain, Range *range);
    int mapTriggerType(int cbwTriggerType, TriggerType *triggerType);
    int mapAInChanType(int cbwChanType, AiChanType *chanType);
    int mapDaqInChanType(int cbwChanType, DaqInChanType *chanType);
    int mapError(UlError error, const char *message);
    int mapScanOptions(int cbwOptions, ScanOption *options);

    // System functions
    int cbGetBoardName(char *BoardName);
    int cbGetConfig(int InfoType, int DevNum, int ConfigItem, int *ConfigVal);
    int cbSetConfig(int InfoType, int DevNum, int ConfigItem, int ConfigVal);
    int cbGetIOStatus(short *Status, long *CurCount, long *CurIndex,int FunctionType);
    int cbStopIOBackground(int FunctionType);

    // Analog I/O functions
    int cbAIn(int Chan, int Gain, unsigned short *DataValue);
    int cbAIn32(int Chan, int Gain, unsigned int *DataValue, int Options);
    int cbAInScan(int LowChan, int HighChan, long Count, long *Rate,
                          int Gain, void *MemHandle, int Options);
    int cbAInputMode(int InputMode);
    int cbALoadQueue(short *ChanArray, short *GainArray, int NumChans);
    int cbAOut(int Chan, int Gain, unsigned short DataValue);
    int cbAOutScan(int LowChan, int HighChan, long Count,
                           long *Rate, int Gain, void *MemHandle, int Options);

    // Counter functions
    int cbCIn32(int CounterNum, unsigned int *Count);
    int cbCLoad32(int RegNum, unsigned int LoadValue);
    int cbCInScan(int FirstCtr,int LastCtr, int Count,
                          int *Rate, void *MemHandle, unsigned int Options);
    int cbCConfigScan(int CounterNum, int Mode,int DebounceTime,
                              int DebounceMode, int EdgeDetection,
                              int TickSize, int MappedChannel);

    // Digital I/O functions
    int cbDBitOut(int PortType, int BitNum, unsigned short BitValue);
    int cbDConfigPort(int PortType, int Direction);
    int cbDConfigBit(int PortType, int BitNum, int Direction);
    int cbDIn(int PortType, unsigned short *DataValue);
    int cbDIn32(int PortType, unsigned int *DataValue);
    int cbDOut(int PortType, unsigned short DataValue);
    int cbDOut32(int PortType, unsigned int DataValue);

    // Pulse functions
    int cbPulseOutStart(int TimerNum, double *Frequency,
                                double *DutyCycle, unsigned int PulseCount,
                                double *InitialDelay, int IdleState, int Options);
    int cbPulseOutStop(int TimerNum);

    // Temperature functions
    int cbTIn(int Chan, int Scale, float *TempValue, int Options);

    // Voltage functions
    int cbVIn(int Chan, int Range, float *DataValue, int Options);

    // Trigger functions
    int cbSetTrigger(int TrigType, unsigned short LowThreshold, unsigned short HighThreshold);

    // Daq functions
    int cbDaqInScan(short *ChanArray, short *ChanTypeArray, short *GainArray, int ChanCount, long *Rate,
                            long *PretrigCount, long *TotalCount, void *MemHandle, int Options);
    int cbDaqSetTrigger(int TrigSource, int TrigSense, int TrigChan, int ChanType,
                                int Gain, float Level, float Variance, int TrigEvent);

protected:
    DaqDeviceDescriptor daqDeviceDescriptor_;
    DaqDeviceHandle daqDeviceHandle_;
    char boardName_[BOARDNAMELEN];
    AiInputMode aiInputMode_;
    Range aiRange_;
    AInFlag aiFlags_;
    int aiScanTrigCount_;
    AiQueueElement *aiScanQueue_;
    TcType tcType_[MAX_TEMP_CHANS];
    Range aoRange_;
    AOutFlag aoFlags_;
    int aoTrigCount_;
    DaqInChanDescriptor *daqiChanDescriptors_;
    bool aiScanInProgress_;
    bool aoScanInProgress_;
    bool daqiScanInProgress_;
    bool daqoScanInProgress_;
    bool diScanInProgress_;
    bool doScanInProgress_;
    bool ctrScanInProgress_;
    TriggerType triggerType_;
    int biBoardType_;
    int biNumADCChans_;
    int biADCRes_;
    int biNumDACChans_;
    int biDACRes_;
    int biNumTempChans_;
    int biDInNumDevs_;
    int diDevType_;
    int diInMask_;
    int diOutMask_;
    int diNumBits_;
};

#endif /* mcBoardInclude */
