#ifndef CUSTOMFILEREADER_HPP
#define CUSTOMFILEREADER_HPP

#include"FileReader/FilesSpecificData/Custom/FRI_FSD_Custom.hpp"
#include"FileReader/Reader/filereader.hpp"
//#include"FileReader/Reader/filereader.hpp"
template<>
struct FileReader_Configuration<FSD_Custom::Reader::Stat>{

};

using FileReader_Custom_Config_Template = FileReader_Configuration<FSD_Custom::Reader>;



using FileReader_Custom = FileReader<FSD_Custom::Reader>;

template<>
template<>
bool FileReader_Custom::processingFunction<FSD_Custom::Reader::Stat::CHECK_END_OF_FILE>();

template<>
template<>
bool FileReader_Custom::processingFunction<FSD_Custom::Reader::Stat::READ_NEXT_LINE>();

template<>
template<>
bool FileReader_Custom::processingFunction<FSD_Custom::Reader::Stat::CHECK_ERRORS>();


template<>
QString FileReader_Custom::fileFilters();

template<>
bool FileReader_Custom::processing();

template<>
bool FileReader_Custom::initialize();

template<>
bool FileReader_Custom::deinitialize();

template<>
QVector<ProcessingFunctions_FR<FSD_Custom::Reader>> FileReader<FSD_Custom::Reader>::fileReader_processingFunctions ;

#endif // CUSTOMFILEREADER_HPP
