#ifndef CUSTOMFILEREADERINTERPRETER_HPP
#define CUSTOMFILEREADERINTERPRETER_HPP

#include"FileReader/Reader/CANoeDBC/CANoeDBCFileReader.hpp"
template<>
bool FileReaderInterpreter<FSD_CANoeDBC>::initialize();

template<>
bool FileReaderInterpreter<FSD_CANoeDBC>::deinitialize();


#endif // CUSTOMFILEREADERINTERPRETER_HPP
