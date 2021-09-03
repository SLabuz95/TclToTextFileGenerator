#include"FileReader/Interpreter/CANoe/canoefilereaderinterpreter.hpp"

template<>
bool FileReaderInterpreter<FSD_CANoe>::initialize(){
    return config.initialize();
}

template<>
bool FileReaderInterpreter<FSD_CANoe>::deinitialize(){
    return config.deinitialize();
}
