#ifndef TCFILEREADERCONFIG_HPP
#define TCFILEREADERCONFIG_HPP


#include"External/FileReader/FilesSpecificData/FSD_Config_Priv.hpp"

using FSD_ByLine = FSD<FSDTypes::ByLine>;

template <>
enum class FSD_ByLine::Reader::Stat{
    CHECK_END_OF_FILE,
    READ_NEXT_LINE,

    SIZE
};
/*
template<>
enum class FSD_ByLine::Reader::Error{
    NO_ERROR,
    ERROR,

    SIZE
};
*/
template <>
enum class FSD_ByLine::FileSpecificInterpreterStat{
    PROCESS_LINE,
    SIZE
};

template<>
enum class FSD_ByLine::COMCommand{
    SIZE
};

template<>
struct FileReaderInterpreter_Configuration_Priv<FSD_ByLine>::Data{
};


template<>
QVector<COMFunctions_FRI<FSD_ByLine>> FileReaderInterpreter_Configuration_Priv<FSD_ByLine>::COMFunctions;


#endif // TCFILEREADERCONFIG_HPP
