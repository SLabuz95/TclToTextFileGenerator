#ifndef FRI_FSD_CUSTOM_HPP
#define FRI_FSD_CUSTOM_HPP

#include"FileReader/FilesSpecificData/FSD_Config_Priv.hpp"

using FSD_CANoeDBC = FSD<FSDTypes::CANoeDBC>;

template <>
enum class FSD_CANoeDBC::Reader::Stat{
    CHECK_ERRORS,
    CHECK_END_OF_FILE,
    READ_NEXT_LINE,

    SIZE
};

template<>
enum class FSD_CANoeDBC::Reader::Error{
    NO_ERROR,
    ERROR,

    SIZE
};

template <>
enum class FSD_CANoeDBC::FileSpecificInterpreterStat{
    PROCESS_LINE,
    SIZE
};

template<>
enum class FSD_CANoeDBC::COMCommand{
    SIZE
};

template<>
struct FileReaderInterpreter_Configuration_Priv<FSD_CANoeDBC>::Data{
};


template<>
QVector<COMFunctions_FRI<FSD_CANoeDBC>> FileReaderInterpreter_Configuration_Priv<FSD_CANoeDBC>::COMFunctions;



#endif // FRI_FSD_CUSTOM_HPP
