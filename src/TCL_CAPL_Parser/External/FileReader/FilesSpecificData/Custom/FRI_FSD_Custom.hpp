#ifndef FRI_FSD_CUSTOM_HPP
#define FRI_FSD_CUSTOM_HPP

#include"FileReader/FilesSpecificData/FSD_Config_Priv.hpp"

using FSD_Custom = FSD<FSDTypes::Custom>;

template <>
enum class FSD_Custom::Reader::Stat{
    CHECK_ERRORS,
    CHECK_END_OF_FILE,
    READ_NEXT_LINE,

    SIZE
};

template<>
enum class FSD_Custom::Reader::Error{
    NO_ERROR,
    ERROR,

    SIZE
};

template <>
enum class FSD_Custom::FileSpecificInterpreterStat{
    PROCESS_LINE,
    SIZE
};

template<>
enum class FSD_Custom::COMCommand{
    SIZE
};

template<>
struct FileReaderInterpreter_Configuration_Priv<FSD_Custom>::Data{
};


template<>
QVector<COMFunctions_FRI<FSD_Custom>> FileReaderInterpreter_Configuration_Priv<FSD_Custom>::COMFunctions;



#endif // FRI_FSD_CUSTOM_HPP
