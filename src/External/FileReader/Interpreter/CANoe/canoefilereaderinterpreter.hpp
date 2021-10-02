#ifndef CANOEFILEREADERINTERPRETER_HPP
#define CANOEFILEREADERINTERPRETER_HPP
//#include"FileReader/Interpreter/filereaderinterpreter.hpp"
//#include"FileReader/FilesSpecificData/CANoeFilesFormat/FRI_FSD_CANoe.hpp"
#include"FileReader/Reader/CANoe/CANoeFileReader.hpp"
template<>
bool FileReaderInterpreter<FSD_CANoe>::initialize();

template<>
bool FileReaderInterpreter<FSD_CANoe>::deinitialize();

#endif // CANOEFILEREADERINTERPRETER_HPP
