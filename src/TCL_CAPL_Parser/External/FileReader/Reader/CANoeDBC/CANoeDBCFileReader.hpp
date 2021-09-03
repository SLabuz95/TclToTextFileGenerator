#ifndef CUSTOMFILEREADER_HPP
#define CUSTOMFILEREADER_HPP

#include"FileReader/FilesSpecificData/CANoeDBC/FRI_FSD_CANoeDBC.hpp"
#include"FileReader/Reader/filereader.hpp"
//#include"FileReader/Reader/filereader.hpp"
template<>
struct FileReader_Configuration<FSD_CANoeDBC::Reader::Stat>{

};

using FileReader_CANoeDBC_Config_Template = FileReader_Configuration<FSD_CANoeDBC::Reader>;



using FileReader_CANoeDBC = FileReader<FSD_CANoeDBC::Reader>;

template<>
template<>
bool FileReader_CANoeDBC::processingFunction<FSD_CANoeDBC::Reader::Stat::CHECK_END_OF_FILE>();

template<>
template<>
bool FileReader_CANoeDBC::processingFunction<FSD_CANoeDBC::Reader::Stat::READ_NEXT_LINE>();

template<>
template<>
bool FileReader_CANoeDBC::processingFunction<FSD_CANoeDBC::Reader::Stat::CHECK_ERRORS>();


template<>
QString FileReader_CANoeDBC::fileFilters();

template<>
bool FileReader_CANoeDBC::processing();

template<>
bool FileReader_CANoeDBC::initialize();

template<>
bool FileReader_CANoeDBC::deinitialize();

template<>
QVector<ProcessingFunctions_FR<FSD_CANoeDBC::Reader>> FileReader<FSD_CANoeDBC::Reader>::fileReader_processingFunctions ;

#endif // CUSTOMFILEREADER_HPP
