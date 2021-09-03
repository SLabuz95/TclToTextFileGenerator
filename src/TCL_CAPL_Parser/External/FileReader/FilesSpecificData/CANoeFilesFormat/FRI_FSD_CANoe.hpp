#ifndef FRI_FSD_CANOE_HPP
#define FRI_FSD_CANOE_HPP

//#include"CANoe/Configuration/canoeconfig.hpp"
//#include"CANoe/Configuration/canoeconfigtestenvironment.hpp"
#include"FileReader/FilesSpecificData/FSD_Config_Priv.hpp"

using FSD_CANoe = FSD<FSDTypes::CANoe>;

template <>
enum class FSD_CANoe::Reader::Stat{
    CHECK_ERRORS,
    CHECK_END_OF_FILE,
    READ_NEXT_LINE,

    SIZE
};

template<>
enum class FSD_CANoe::Reader::Error{
    NO_ERROR,
    ERROR,

    SIZE
};

template <>
enum class FSD_CANoe::FileSpecificInterpreterStat{
    BEGIN_OF_OBJECT,
    END_OF_OBJECT,
    DATA,
    SIZE
};

template<>
enum class FSD_CANoe::COMCommand{
    SIZE
};

template<>
struct FileReaderInterpreter_Configuration_Priv<FSD_CANoe>::Data{
    int falseLevel = -1;
};


template<>
QVector<COMFunctions_FRI<FSD_CANoe>> FileReaderInterpreter_Configuration_Priv<FSD_CANoe>::COMFunctions;





#endif // FRI_FSD_CANOE_HPP
