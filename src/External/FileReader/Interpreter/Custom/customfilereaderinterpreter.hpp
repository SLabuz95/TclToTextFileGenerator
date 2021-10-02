#ifndef CUSTOMFILEREADERINTERPRETER_HPP
#define CUSTOMFILEREADERINTERPRETER_HPP

#include"FileReader/Reader/Custom/CustomFileReader.hpp"
template<>
bool FileReaderInterpreter<FSD_Custom>::initialize();

template<>
bool FileReaderInterpreter<FSD_Custom>::deinitialize();


#endif // CUSTOMFILEREADERINTERPRETER_HPP
