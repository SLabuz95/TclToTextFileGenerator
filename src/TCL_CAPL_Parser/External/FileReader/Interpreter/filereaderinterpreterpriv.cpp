#include"filereaderinterpreterpriv.hpp"

QVarLengthArray<ProcessingFunction_FRI_Priv, (int)FileReaderInterpreter_Priv_Stat::SIZE> FileReaderInterpreter_Priv::fileReaderInterpreterPriv_processingFunctions = {
        &FileReaderInterpreter_Priv::noStatReturned,
        &FileReaderInterpreter_Priv::processing,
        &FileReaderInterpreter_Priv::initialize,
        &FileReaderInterpreter_Priv::deinitialize
        };



bool FileReaderInterpreter_Priv::exec(){
    return (this->*fileReaderInterpreterPriv_processingFunctions.at(readInfo.takeLastStat()))();
}

/*bool FileReaderInterpreter_Priv::resultChecking(){

    if(errorCode)
        return false;
    else
        return true;
}*/

bool FileReaderInterpreter_Priv::process(){
    return (this->*COM_processingFunctions.at(static_cast<int>(FRI_COM_Manager::receive(COMChannel).command)))(FRI_COM_Manager::receive(COMChannel));
}

template<>
bool FileReaderInterpreter_Priv::COM_processingFunction<NS_FRI_COM_Manager::Commands::EXEC_STAT>(NS_FRI_COM_Manager::ChannelData& cmd){
    int newInterpreterStat = *static_cast<int*>(cmd.channelData);
    //FRI_COM_Manager::managerAccess()->last().commands.removeLast();
    readInfo.addStat(newInterpreterStat);
    while(exec());

    return true;
}

QVarLengthArray<COM_ProcessingFunctionPtr, (int)NS_FRI_COM_Manager::Commands::SIZE> FileReaderInterpreter_Priv::COM_processingFunctions = {
        &FileReaderInterpreter_Priv::COM_processingFunction<NS_FRI_COM_Manager::Commands::EXEC_STAT>
};
