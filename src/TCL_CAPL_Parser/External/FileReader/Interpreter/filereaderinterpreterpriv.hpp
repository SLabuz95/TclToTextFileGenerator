#ifndef FILEREADERINTERPRETERPRIV_HPP
#define FILEREADERINTERPRETERPRIV_HPP

#include"filereaderinterpreterprivreadinfo.hpp"
#include<QVarLengthArray>
#include"FileReader/COMManager/FRI_COM_Manager.hpp"
//#include"FileReader/FilesSpecificData/FSDTemplate.hpp"
//#include"FileReader/Reader/filereader.hpp"
#include"FileReader/Interpreter/filereaderinterpreterprivreadinfo.hpp"
//#include"FileReader/FilesSpecificData/FSD_Config_Priv.hpp"

enum class FileReaderInterpreter_Priv_Stat{
    NO_STAT,
    PROCESSING,
    INITIALIZE,
    DEINITIALIZE,    
    SIZE
};
class FileReaderInterpreter_Priv;
using ProcessingFunction_FRI_Priv = bool (FileReaderInterpreter_Priv::*)();
using COM_ProcessingFunctionPtr = bool (FileReaderInterpreter_Priv::*)(NS_FRI_COM_Manager::ChannelData&);

template <class FSD>
class FileReader;

class FileReaderInterpreter_Priv{
   public:
    FileReaderInterpreter_Priv(FRI_COM_Manager::COMChannelRef COMChannel) : COMChannel(COMChannel){}
protected:
    FileReaderInterpreter_Priv_ReadInfo readInfo;
    static QVarLengthArray<ProcessingFunction_FRI_Priv, (int)FileReaderInterpreter_Priv_Stat::SIZE> fileReaderInterpreterPriv_processingFunctions;
    static QVarLengthArray<COM_ProcessingFunctionPtr, (int)NS_FRI_COM_Manager::Commands::SIZE> COM_processingFunctions;

    template<NS_FRI_COM_Manager::Commands>
    bool COM_processingFunction(NS_FRI_COM_Manager::ChannelData&);

    virtual bool processing(){return true;};
    virtual bool initialize() = 0;
    virtual bool deinitialize() = 0;
    virtual QString fileFilter(){return QString();}

    virtual bool exec();

public:
    FRI_COM_Manager::COMChannelRef COMChannel;

    bool onTheBlackList = false;
    inline bool noStatReturned(){return false;}
    inline static constexpr int realStat(FileReaderInterpreter_Priv_Stat stat){return (int)stat;}
    inline static constexpr int numbOfStats(){return (int)FileReaderInterpreter_Priv_Stat::SIZE;}
    //bool resultChecking();
    virtual bool process();
    //inline void updateReaderErrorCode(int errorCode){readerErrorCode = errorCode;}
    //inline int getReaderErrorCode(){return readerErrorCode;}
    //inline int getErrorCode(){return errorCode;}
    inline void updateOnTheBlackList(bool updateValue){onTheBlackList = updateValue;}
    inline bool getOnTheBlackList(){return onTheBlackList;}

    bool simulate_processCOMMessage(NS_FRI_COM_Manager::ChannelDatas &cmds){
        FRI_COM_Manager::send(COMChannel, cmds);
        return process();
    }

};

template<>
bool FileReaderInterpreter_Priv::COM_processingFunction<NS_FRI_COM_Manager::Commands::EXEC_STAT>(NS_FRI_COM_Manager::ChannelData&);

#endif // FILEREADERINTERPRETERPRIV_HPP
