#ifndef TCFILEREADER_HPP
#define TCFILEREADER_HPP

#include"tcfilereaderconfig.hpp"
#include"FileReader/Reader/filereader.hpp"
//#include"FileReader/Reader/filereader.hpp"
template<>
struct FileReader_Configuration<FSD_ByLine::Reader::Stat>{

};

using FileReader_ByLine_Config_Template = FileReader_Configuration<FSD_ByLine::Reader>;


using FileReader_ByLine = FileReader<FSD_ByLine::Reader>;

template<>
template<>
bool FileReader_ByLine::processingFunction<FSD_ByLine::Reader::Stat::CHECK_END_OF_FILE>();

template<>
template<>
bool FileReader_ByLine::processingFunction<FSD_ByLine::Reader::Stat::READ_NEXT_LINE>();




template<>
QString FileReader_ByLine::fileFilters();

template<>
bool FileReader_ByLine::processing();

template<>
bool FileReader_ByLine::initialize();

template<>
bool FileReader_ByLine::deinitialize();

template<>
QVector<ProcessingFunctions_FR<FSD_ByLine::Reader>> FileReader<FSD_ByLine::Reader>::fileReader_processingFunctions ;


#endif // TCFILEREADER_HPP
