#include"tcfilereaderinterpreter.hpp"

template<>
bool FileReaderInterpreter<FSD_ByLine>::initialize(){
    return config.initialize();
}

template<>
bool FileReaderInterpreter<FSD_ByLine>::deinitialize(){
    return config.deinitialize();
}
