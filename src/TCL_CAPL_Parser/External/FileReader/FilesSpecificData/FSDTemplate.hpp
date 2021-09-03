#ifndef FSDTEMPLATE_HPP
#define FSDTEMPLATE_HPP


#include"FileReader/COMManager/FRI_COM_Manager.hpp"
//#include"FileReader/Interpreter/filereaderinterpreter.hpp"
#include"FSDDataModelBase.hpp"
#include"FileReader/Interpreter/statsconfiguration.hpp"



enum class FSDTypes{
    XML,
    CANoe,
    CANoeDBC,
    ByLine,
    SIZE
};

template<class Interpreter>
class FileReaderInterpreter_Configuration;
template <class FSD>
class FileReaderInterpreter_Configuration_Priv;

template<class ClassName>
using ProcessingFunctions_FRI = bool (ClassName::*)();

template<class ClassName>
using ProcessingFunctions_FSI = bool (ClassName::*)();

template<class FSD>
using COMFunctions_FRI = bool (FileReaderInterpreter_Configuration_Priv<FSD>::*)(NS_FRI_COM_Manager::ChannelData&);

template<class FSD>
class FileReaderInterpreter;

template<FSDTypes FSDType>
struct FSD{
    using ErrorMsg = QString;
    //using ErrorMsgRef = ErrorMsg&;

    class Error{
    private:
        ErrorMsg errorMsg_;

    public:
        Error(){}
        inline bool isErrorMsg(){return !isNoErrorMsg();}
        inline bool isNoErrorMsg(){return errorMsg_.isEmpty();}
        inline ErrorMsg errorMsg(){return errorMsg_;}
        inline void setErrorMsg(ErrorMsg errorMsg){errorMsg_ = errorMsg;}
    };
    using ErrorRef = Error&;
    //enum class ErrorType;
    struct Reader{
        using ErrorMsg = ErrorMsg;
        using ErrorRef = ErrorRef;

       enum class Stat;
       //enum class Error;
       //static QVarLengthArray<ProcessingFunctions_FRI<FileReaderInterpreter_Configuration<(int)FSDType, >>, (int)FSDStats::SIZE> FSD_processingFunctions;
   };
    template<class DataModelClass>
    requires (std::is_base_of<typename DataModelClass::DataModelBase, DataModelClass>::value)
    class Interpreter{
       public:
        struct Data;
        struct Functions;
        using Parent_FSD = FSD<FSDType>;
        using DataModel = DataModelClass;
        using Config = FileReaderInterpreter_Configuration<Interpreter<DataModel>>;
        using InterpreterData = Data;
        Interpreter(Config& config) : config(config){}
       enum class Stat;
       //enum class Error;
       static const InterpreterStatsConfiguration statConfig;
       Config& config;

       Data* interpreterData = nullptr;
       template<Stat>
       bool processingFunction();
       static QVector<ProcessingFunctions_FRI<Config>> processingFunctions;

   };

    enum class FileSpecificInterpreterStat;

    enum class COMCommand;
};

template <FSDTypes FSDType>
template<class DataModelClass>
QVector<ProcessingFunctions_FRI<typename FSD<FSDType>::template Interpreter<DataModelClass>::Config>> FSD<FSDType>::Interpreter<DataModelClass>::processingFunctions;


#endif // FSDTEMPLATE_HPP
