#ifndef XMLFILEREADERINTERPRETER_HPP
#define XMLFILEREADERINTERPRETER_HPP

//#include"External/FileReader/Interpreter/filereaderinterpreter.hpp"
//#include"External/FileReader/FilesSpecificData/XML/FRI_FSD_XML.hpp"
#include"External/FileReader/Reader/XMLFileReader/xmlfilereader.hpp"

template<>
bool FileReaderInterpreter<FSD_XML>::initialize();

template<>
bool FileReaderInterpreter<FSD_XML>::deinitialize();

#endif // XMLFILEREADERINTERPRETER_HPP
