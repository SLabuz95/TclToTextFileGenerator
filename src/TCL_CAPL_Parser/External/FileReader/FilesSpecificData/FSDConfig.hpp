#ifndef FSDCONFIG_HPP
#define FSDCONFIG_HPP

#include"FSD_Config_Priv.hpp"
#include"FileReader/Reader/filereader.hpp"


template<class Interpreter>
class FileReaderInterpreter_Configuration :  public FileReaderInterpreter_Configuration_Priv<typename Interpreter::Parent_FSD>, public FSD_Config_Control, public Interpreter{
protected:
    //static QString errorMessage(int){return QString();}

public:
    using NewDataModel = typename Interpreter::DataModel;
    // Functional
    FileReaderInterpreter_Configuration(NewDataModel& dataModel) : Interpreter(*this), dataModel(dataModel){};
    FileReaderInterpreter_Configuration(const FileReaderInterpreter_Configuration&) = delete;

    bool initialize()override;
    bool deinitialize()override;
    QString fileFilter() override{return FileReaderInterpreter_Configuration_Priv<typename Interpreter::Parent_FSD>::fileFilter();}


    static QVarLengthArray<ProcessingFunctions_FRI<FileReaderInterpreter_Configuration<Interpreter>>, (int)Interpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)Interpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> processingFunctions_FS;
    template<typename Interpreter::Parent_FSD::FileSpecificInterpreterStat>
    bool FSFunction();
    // Parameters
    NewDataModel& dataModel;
    inline NewDataModel& getDataModel(){return dataModel;}

    //QString errorStr(){return QString();}

    virtual bool exec(int stat) override{
        stats.append(stat);
        bool result = false;
        while(!stats.isEmpty()){
            stat = stats.takeLast();
            if(stat < (int)Interpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE)
                result =  (this->*processingFunctions_FS.at(stat))();
            else
            {
                if(stat < (int)Interpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE + (int)Interpreter::Stat::SIZE)
                    result = (this->*Interpreter::processingFunctions.at(stat - (int)Interpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE))();
                else
                    result = false;
            }
            if(!result)
                return false;
        }
        return true;
    }

};

template <class Interpreter>
QVarLengthArray<ProcessingFunctions_FRI<FileReaderInterpreter_Configuration<Interpreter>>, (int)Interpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)Interpreter::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FileReaderInterpreter_Configuration<Interpreter>::processingFunctions_FS;


#endif // FSDCONFIG_HPP
