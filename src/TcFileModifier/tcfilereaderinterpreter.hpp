#ifndef TCFILEREADERINTERPRETER_HPP
#define TCFILEREADERINTERPRETER_HPP

#include"tcfilereaderconfig.hpp"
template<>
bool FileReaderInterpreter<FSD_ByLine>::initialize();

template<>
bool FileReaderInterpreter<FSD_ByLine>::deinitialize();


#endif // TCFILEREADERINTERPRETER_HPP
