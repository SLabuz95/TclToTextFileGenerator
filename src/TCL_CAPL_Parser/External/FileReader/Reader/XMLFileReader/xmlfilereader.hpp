#ifndef XMLFILEREADER_HPP
#define XMLFILEREADER_HPP

//#include"FileReader/Reader/filereader.hpp"
#include"FileReader/FilesSpecificData/XML/FRI_FSD_XML.hpp"
#include<QXmlStreamReader>
#include"FileReader/Reader/filereader.hpp"
template<>
struct FileReader_Configuration<FSD_XML::Reader::Stat>{

    QXmlStreamReader xml;

};

using FileReader_Xml_Config_Template = FileReader_Configuration<FSD_XML::Reader>;



using FileReader_Xml = FileReader<FSD_XML::Reader>;

template<>
template<>
bool FileReader_Xml::processingFunction<FSD_XML::Reader::Stat::CHECK_END_OF_FILE>();

template<>
template<>
bool FileReader_Xml::processingFunction<FSD_XML::Reader::Stat::READ_NEXT_TOKEN>();

template<>
template<>
bool FileReader_Xml::processingFunction<FSD_XML::Reader::Stat::CHECK_XML_ERRORS>();

template<>
QString FileReader_Xml::fileFilters();

template<>
bool FileReader_Xml::processing();

template<>
bool FileReader_Xml::initialize();

template<>
bool FileReader_Xml::deinitialize();

template<>
QVector<ProcessingFunctions_FR<FSD_XML::Reader>> FileReader<FSD_XML::Reader>::fileReader_processingFunctions ;

//template<>
//constexpr FSD_XML::Reader::Error CompileTimeErrorControl<FSD_XML::Reader::Error>::compileTimeError();

#endif // XMLFILEREADER_HPP
