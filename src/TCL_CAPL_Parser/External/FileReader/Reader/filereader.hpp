#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include"filereaderpriv.hpp"
//#include"CompileTimeErrorFunctions/compiletimeerrorfunctions.hpp"
#include"FileReader/Reader/filereaderprivreadinfo.hpp"
#include"FileReader/Reader/filereaderconfiguration.hpp"

template<class FSD>    // Interpreter Type - Mainly for overriding Filters
class FileReader;

template<class FSD>
using ProcessingFunctions_FR = bool (FileReader<FSD>::*)();

class FileReaderInterpreter_Priv;
template<class FSD>
class FileReader : public FileReader_Priv /*public CompileTimeErrorControl<typename FSD::Reader::Error>*/{
    using FSD_Stat = typename FSD::Reader::Stat;
    using FSD_Error = typename FSD::ErrorRef;
public:
    FileReader(FileReaderInterpreter_Priv& interpreter, FSD_Error error)
        : FileReader_Priv(interpreter), errorMsg(error){}
    ~FileReader(){delete config; config = nullptr;}
    inline static constexpr int realReadStat(FSD_Stat stat){return static_cast<int>(stat) + FileReader_Priv::numbOfReadStats();}
    inline static constexpr int numbOfReadStats(){return (int)FSD_Stat::SIZE;}
    static QVector<ProcessingFunctions_FR<FSD>> fileReader_processingFunctions;
    //static QString errorMessage(int){return QString();}
protected:
    FileReader_Configuration<FSD_Stat>* config = nullptr;
    FSD_Error errorMsg;
    // Specialization Specific
    virtual QString fileFilters()override;

    // ----------------------------------------------

    template<FSD_Stat>
    bool processingFunction();

    inline bool ERROR_CALL(FSD_Error error){errorMsg.setError(error); return false;}
    virtual bool processing() override;
    virtual bool initialize() override;
    virtual bool deinitialize() override;
    virtual bool exec() override{
        bool result = false;
        if(readInfo.getLastStat() < FileReader_Priv::numbOfReadStats()){
            result = FileReader_Priv::exec();
        }else{
            if(readInfo.getLastStat() < realReadStat(FSD_Stat::SIZE))
                result = (this->*fileReader_processingFunctions.at(readInfo.takeLastStat() - FileReader_Priv::numbOfReadStats()))();
        }
        return result and errorMsg.isNoErrorMsg() and isNoError();
    }

};

//template <class FSD>
//QVector<ProcessingFunctions_FR<FSD>> FileReader<FSD>::fileReader_processingFunctions;


#endif // FILEREADER_HPP
