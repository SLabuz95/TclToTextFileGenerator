#ifndef CANOEFILEREADER_HPP
#define CANOEFILEREADER_HPP

#include"FileReader/FilesSpecificData/CANoeFilesFormat/FRI_FSD_CANoe.hpp"
#include"FileReader/Reader/filereader.hpp"
//#include"FileReader/Reader/filereader.hpp"
template<>
struct FileReader_Configuration<FSD_CANoe::Reader::Stat>{

};

using FileReader_CANoe_Config_Template = FileReader_Configuration<FSD_CANoe::Reader>;



using FileReader_CANoe = FileReader<FSD_CANoe::Reader>;

template<>
template<>
bool FileReader_CANoe::processingFunction<FSD_CANoe::Reader::Stat::CHECK_END_OF_FILE>();

template<>
template<>
bool FileReader_CANoe::processingFunction<FSD_CANoe::Reader::Stat::READ_NEXT_LINE>();

template<>
template<>
bool FileReader_CANoe::processingFunction<FSD_CANoe::Reader::Stat::CHECK_ERRORS>();


template<>
QString FileReader_CANoe::fileFilters();

template<>
bool FileReader_CANoe::processing();

template<>
bool FileReader_CANoe::initialize();

template<>
bool FileReader_CANoe::deinitialize();

template<>
QVector<ProcessingFunctions_FR<FSD_CANoe::Reader>> FileReader<FSD_CANoe::Reader>::fileReader_processingFunctions ;

//template<>
//constexpr FSD_CANoe::Reader::Error CompileTimeErrorControl<FSD_CANoe::Reader::Error>::compileTimeError();

#endif // CANOEFILEREADER_HPP
