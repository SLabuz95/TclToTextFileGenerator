#include"FileReader/Interpreter/CANoeDBC/canoedbcfilereaderinterpreter.hpp"
template<>
bool FileReaderInterpreter<FSD_CANoeDBC>::initialize(){
    return config.initialize();
}

template<>
bool FileReaderInterpreter<FSD_CANoeDBC>::deinitialize(){
    return config.deinitialize();
}
