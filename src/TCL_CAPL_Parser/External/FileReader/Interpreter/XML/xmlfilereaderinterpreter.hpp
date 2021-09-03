#ifndef XMLFILEREADERINTERPRETER_HPP
#define XMLFILEREADERINTERPRETER_HPP

//#include"FileReader/Interpreter/filereaderinterpreter.hpp"
//#include"FileReader/FilesSpecificData/XML/FRI_FSD_XML.hpp"
#include"FileReader/Reader/XMLFileReader/xmlfilereader.hpp"

template<>
bool FileReaderInterpreter<FSD_XML>::initialize();

template<>
bool FileReaderInterpreter<FSD_XML>::deinitialize();

#endif // XMLFILEREADERINTERPRETER_HPP
