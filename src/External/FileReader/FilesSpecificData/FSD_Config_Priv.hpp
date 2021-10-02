#ifndef FSD_CONFIG_PRIV_HPP
#define FSD_CONFIG_PRIV_HPP
#include"FSD_Config_Control.hpp"
#include"FSDTemplate.hpp"
#include"External/FileReader/Interpreter/filereaderinterpreter.hpp"


template <class FSD>
class FileReaderInterpreter_Configuration_Priv {
public:
    FileReaderInterpreter_Configuration_Priv() : COMChannel(FRI_COM_Manager::activate()){

    }

    ~FileReaderInterpreter_Configuration_Priv(){
        //TODO: Add Error control by exception catch (destructor)
        FRI_COM_Manager::deactivate(COMChannel);
    }

    virtual bool initialize(){return true;};
    virtual bool processing(){return true;};
    virtual bool deinitialize(){return true;};
    virtual QString fileFilter(){return QString();}

    virtual bool exec(int) = 0;
    FRI_COM_Manager::Channel *const COMChannel;
    typename FSD::Error errorMsg;

    inline bool ERROR_CALL(typename FSD::ErrorMsg error){
        errorMsg.setErrorMsg(error); return false;
    }

    inline bool isError(){return errorMsg.isErrorMsg();}
    inline typename FSD::ErrorMsg error(){return errorMsg.errorMsg();}

    bool onTheBlackList = false;

    struct Data;
    Data* data = nullptr;

    // File specific functions
    static QVector<COMFunctions_FRI<FSD>> COMFunctions;

    inline QString readFile(QString description, QStringList blackList){
        FileReaderInterpreter<FSD> interpreter(*this);
        QString ret = interpreter.readFile(description, fileFilter(), blackList);
        //updateErrorCodes(interpreter.getReaderErrorCode(), interpreter.getErrorCode());        
        updateOnTheBlackList(interpreter.getOnTheBlackList());
        return ret;
    }
    inline QString readFileByFilePath(QString filePath, QStringList blackList){
        FileReaderInterpreter<FSD> interpreter(*this);
        QString ret = interpreter.readFileByFilePath(filePath, fileFilter(), blackList);
        //updateErrorCodes(interpreter.getReaderErrorCode(), interpreter.getErrorCode());
        updateOnTheBlackList(interpreter.getOnTheBlackList());
        return ret;
    }

    //inline void updateErrorCodes(int readerCode, int interpreterCode){readerErrorCode = readerCode; errorCode = interpreterCode;}
    //inline bool isError(){return readerErrorCode || errorCode;}
    //inline bool readOccured(){return data;}
    void updateOnTheBlackList(bool updateValue){onTheBlackList = updateValue;}
    bool getOnTheBlackList(){return onTheBlackList;}
};

template<class FSD>
QVector<COMFunctions_FRI<FSD>> FileReaderInterpreter_Configuration_Priv<FSD>::COMFunctions ;



#endif // FSD_CONFIG_PRIV_HPP
