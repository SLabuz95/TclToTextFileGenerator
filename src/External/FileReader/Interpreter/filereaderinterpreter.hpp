#ifndef FILEREADERINTERPRETER_HPP
#define FILEREADERINTERPRETER_HPP

#include"filereaderinterpreterpriv.hpp"
#include"FileReader/Reader/filereader.hpp"
//#include"FileReader/Reader/filereaderpriv.hpp"
#include"FileReader/FilesSpecificData/FSDTemplate.hpp"
#include<QFileInfo>
#include<QDebug>
#include"FileReader/FilesSpecificData/FSD_Config_Priv.hpp"

template <class FSD>
class FileReaderInterpreter_Configuration_Priv;
template<class FSD>
class FileReaderInterpreter : public FileReaderInterpreter_Priv{
    using Config = FileReaderInterpreter_Configuration_Priv<FSD>;
    FileReaderInterpreter() = delete ;
    FileReaderInterpreter(const FileReaderInterpreter&) = delete;

public:
    FileReaderInterpreter(FileReaderInterpreter_Configuration_Priv<FSD>& config) : FileReaderInterpreter_Priv(config.COMChannel), config(config), errorMsg(config.errorMsg){}
    ~FileReaderInterpreter(){config.deinitialize();}

    inline bool ERROR_CALL(typename FSD::ErrorMsg error){errorMsg = error;}
protected:
    FileReaderInterpreter_Configuration_Priv<FSD>& config;
    typename FSD::ErrorRef errorMsg;

    bool initialize() override;
    bool deinitialize() override;

    virtual bool exec() override{
        if(readInfo.getLastStat() < FileReaderInterpreter_Priv::numbOfStats()){
            return FileReaderInterpreter_Priv::exec() and errorMsg.isNoErrorMsg();

        }else{
            if(readInfo.getLastStat() < (int)FSD::FileSpecificInterpreterStat::SIZE + (int)FileReaderInterpreter_Priv::numbOfStats())
            {
                return config.exec((int)readInfo.takeLastStat() - (int)FileReaderInterpreter_Priv::numbOfStats()) and
                 errorMsg.isNoErrorMsg();
            }else{
                return false;
            }
        }
    }

    virtual bool process() override{        
        if(FRI_COM_Manager::receive(COMChannel).command < NS_FRI_COM_Manager::Commands::SIZE){
            return FileReaderInterpreter_Priv::process();
        }else{
           if(static_cast<int>(FRI_COM_Manager::receive(COMChannel).command) < static_cast<int>(NS_FRI_COM_Manager::Commands::SIZE) + static_cast<int>(FSD::COMCommand::SIZE)){
               return (config.*(config.COMFunctions.at(static_cast<int>(FRI_COM_Manager::receive(COMChannel).command) - static_cast<int>(NS_FRI_COM_Manager::Commands::SIZE))))(FRI_COM_Manager::receive(COMChannel));
           }else{
               return false;
           }
        }
    }
public:
    inline QString readFile(QString description, QString fileFilter, QStringList blackList){
        const QString READER_PRE_ERROR_MSG = "Reader Error: ";
        FileReader<typename FSD::Reader> reader(*this, errorMsg);
        QString ret = reader.readFile(description, fileFilter, blackList);
        if(errorMsg.isNoErrorMsg()){
            if(reader.isError()){
                errorMsg.setErrorMsg(READER_PRE_ERROR_MSG + reader.errorMsgStr());
            }
        }
        updateOnTheBlackList(reader.getOnTheBlackList());
        return ret;
    }
    inline QString readFileByFilePath(QString filePath, QString fileFilter, QStringList blackList){
        const QString READER_PRE_ERROR_MSG = "Reader Error: ";
        FileReader<typename FSD::Reader>  reader(*this, errorMsg);
        QString ret = reader.readFileByFilePath(QFileInfo(filePath), fileFilter, blackList);
        if(errorMsg.isNoErrorMsg()){
            if(reader.isError()){
                errorMsg.setErrorMsg(READER_PRE_ERROR_MSG + reader.errorMsgStr());
            }
        }
        updateOnTheBlackList(reader.getOnTheBlackList());
        return ret;
    }
    //inline FSD_DataModelBase* getDataModel(){return config.dataModel;}

};

#endif // FILEREADERINTERPRETER_HPP
