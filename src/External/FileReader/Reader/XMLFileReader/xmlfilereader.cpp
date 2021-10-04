#include"xmlfilereader.hpp"
//#include"External/FileReader/Reader/filereaderprivreadinfo.hpp"
//#include"External/FileReader/Interpreter/filereaderinterpreterpriv.hpp"
using XML_Reader = FSD_XML::Reader;

template<>
QString FileReader_Xml::fileFilters(){
    return QString("*.xml");
}

template<>
bool FileReader_Xml::processing(){
 /*
  * 1) End of File Check
  * 2) Next Token
  * 3) Check For Errors
  */
    readInfo.addStats({FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::PROCESSING), realReadStat(XML_Reader::Stat::CHECK_XML_ERRORS), realReadStat(XML_Reader::Stat::READ_NEXT_TOKEN), realReadStat(XML_Reader::Stat::CHECK_END_OF_FILE)});
    return true;
}

template<>
bool FileReader_Xml::initialize(){
    config = new FileReader_Configuration<FSD_Stat>;
    config->xml.setDevice(&readInfo.getFile());
    return FileReader_Priv::initialize();
}

template<>
bool FileReader_Xml::deinitialize(){
    return FileReader_Priv::deinitialize();
}

template<>
template<>
bool FileReader_Xml::processingFunction<XML_Reader::Stat::CHECK_END_OF_FILE>(){
    if(config->xml.atEnd()){    // Clear all stats and set CLOSE_FILE and DEINIT Stats
        readInfo.clearStats();
        readInfo.addStats({FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::DEINITIALIZE), FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::CLOSE_FILE),});
    }
    return true;
}

template<>
template<>
bool FileReader_Xml::processingFunction<XML_Reader::Stat::READ_NEXT_TOKEN>(){
    QXmlStreamReader::TokenType token = config->xml.readNext();
    int interpreterStat = static_cast<int>(FileReaderInterpreter_Priv_Stat::SIZE) + static_cast<int>(token);
    if(token < (int)FSD_XML::FileSpecificInterpreterStat::SIZE){
        QXmlStreamReader& reader = config->xml;
        NS_FRI_COM_Manager::ChannelDatas cmds = {
            {NS_FRI_COM_Manager::Commands::EXEC_STAT, &interpreterStat},
            {NS_FRI_COM_Manager::Commands::DATA, &reader}
        };
        return processCOMMessage(cmds);
    }
    return true;
}

template<>
template<>
bool FileReader_Xml::processingFunction<XML_Reader::Stat::CHECK_XML_ERRORS>(){
    if(config->xml.hasError() )
    {
        errorMsg.setErrorMsg(QString("XML Internal Error: " + config->xml.errorString()));
    }
    return not config->xml.hasError();
}

template<>
QVector<ProcessingFunctions_FR<XML_Reader>> FileReader<XML_Reader>::fileReader_processingFunctions = {
    &FileReader_Xml::processingFunction<XML_Reader::Stat::CHECK_XML_ERRORS>,
    &FileReader_Xml::processingFunction<XML_Reader::Stat::CHECK_END_OF_FILE>,
    &FileReader_Xml::processingFunction<XML_Reader::Stat::READ_NEXT_TOKEN>
};

//template<>
//constexpr FSD_XML::Reader::Error CompileTimeErrorControl<FSD_XML::Reader::Error>::compileTimeError(){
//    return FSD_XML::Reader::Error::NO_ERROR;
//}
