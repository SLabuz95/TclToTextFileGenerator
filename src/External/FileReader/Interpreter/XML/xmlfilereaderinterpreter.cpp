#include"FileReader/Interpreter/XML/xmlfilereaderinterpreter.hpp"

template<>
bool FileReaderInterpreter<FSD_XML>::initialize(){
    return config.initialize();
}

template<>
bool FileReaderInterpreter<FSD_XML>::deinitialize(){
    return config.deinitialize();
}
