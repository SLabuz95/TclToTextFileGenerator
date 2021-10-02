#include"FileReader/Interpreter/Custom/customfilereaderinterpreter.hpp"
template<>
bool FileReaderInterpreter<FSD_Custom>::initialize(){
    return config.initialize();
}

template<>
bool FileReaderInterpreter<FSD_Custom>::deinitialize(){
    return config.deinitialize();
}
