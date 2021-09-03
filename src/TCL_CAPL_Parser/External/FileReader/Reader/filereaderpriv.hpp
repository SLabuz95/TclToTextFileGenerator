#ifndef FILEREADERPRIV_HPP
#define FILEREADERPRIV_HPP
#include<QString>

#include"FileReader/COMManager/FRI_COM_Manager.hpp"
//#include"FileReader/Interpreter/filereaderinterpreterpriv.hpp"
//#include"filereaderprivreadinfo.hpp"
//#include<QFile>
//#include<QVarLengthArray>
//#include<QFileInfo>
#include"filereaderprivreadinfo.hpp"

enum class FileReader_Priv_ReadStat{
    NO_STAT,
    PROCESSING,
    INITIALIZE,
    DEINITIALIZE,
    OPEN_FILE,
    CLOSE_FILE,
    //ACTIVE_COM,
    //DEACTIVATE_COM,
    SIZE
};
class QFileInfo;
class FileReader_Priv;


using ProcessingFunctions_FR_Priv = bool (FileReader_Priv::*)();

class FileReaderInterpreter_Priv;
class FileReader_Priv_ReadInfo;
class FileReader_Priv{
    enum class Error : quint8{
        NoError,
        Internal,
        FileDoesntExistError,
        FileOpenError,

        Count
    } errorType = Error::NoError;

    using ErrorTypeMessages = const QVector< QString>;
    ErrorTypeMessages errorTypeMessages
    {
      QStringLiteral("No Error"),
      QStringLiteral("Internal Error"),
      QStringLiteral("File Doesnt Exist Error"),
      QStringLiteral("File Open Error"),
    };

    FileReader_Priv() = delete;
public:
    FileReader_Priv(FileReaderInterpreter_Priv& interpreter);
    inline bool noStatReturned(){return false;}

    inline bool isNoError(){return errorType == Error::NoError;}
    inline bool isError(){return !isNoError();}
    inline QString errorMsgStr(){return errorTypeMessages.at(static_cast<std::underlying_type_t<Error>>(errorType));};
protected:
    inline void setErrorType(Error err){errorType = err;}
    //int _errorCode = 0;
    bool onTheBlackList = false;
    FRI_COM_Manager::COMChannelRef COMChannel;

    //bool comActive = false;
    virtual QString fileFilters() = 0;
    // Execute Functions only called by exec function
    virtual bool processing() = 0;
    bool openFile();
   // inline bool activateCOM(){return FRI_COM_Manager::activate() && (comActive = true);}
    bool processCOMMessage(NS_FRI_COM_Manager::ChannelDatas &cmds);
    QFile* fileAccess();
   // inline bool deactivateCOM(){return FRI_COM_Manager::deactivate() && !(comActive = false);}
    bool closeFile();
    virtual bool initialize();
    virtual bool deinitialize();

    FileReaderInterpreter_Priv& interpreter;
    FileReader_Priv_ReadInfo readInfo;
    static QVarLengthArray<ProcessingFunctions_FR_Priv, (int)FileReader_Priv_ReadStat::SIZE> fileReaderPriv_processingFunctions;

    virtual bool exec();
    bool resultChecking();


public:
    QString readFile(QString, QString, QStringList blackList);
    QString readFileByFilePath(QFileInfo&&, QString, QStringList blackList);
    inline bool getOnTheBlackList(){return onTheBlackList;}
    //inline int errorCode(){return _errorCode;}
    inline static constexpr int realReadStat(FileReader_Priv_ReadStat stat){return (int)stat;}
    inline static constexpr int numbOfReadStats(){return (int)FileReader_Priv_ReadStat::SIZE;}

};

#endif // FILEREADERPRIV_HPP
