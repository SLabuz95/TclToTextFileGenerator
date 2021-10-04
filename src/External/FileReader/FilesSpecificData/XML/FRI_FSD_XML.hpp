#ifndef FRI_FSD_XML_HPP
#define FRI_FSD_XML_HPP

#include"External/FileReader/FilesSpecificData/FSD_Config_Priv.hpp"

//#include<QXmlSimpleReader>
#include<QXmlStreamAttributes>

//#include"CANoe/Other/canoetestenvironment.hpp"
//#include"FTP_From_Xml/FTP_From_Xml.hpp"

// FSD Struct Section ----------------------------------------------------------------------------------
using FSD_XML = FSD<FSDTypes::XML>;

template<>
enum class FSD_XML::Reader::Stat{
    CHECK_XML_ERRORS,
    CHECK_END_OF_FILE,
    READ_NEXT_TOKEN,

    SIZE
};
/*
template<>
enum class FSD_XML::Reader::Error{
    NO_ERROR,
    ERROR,

    SIZE
};
*/
template <>
enum class FSD_XML::FileSpecificInterpreterStat{
    NO_TOKEN,
    INVALID,
    START_DOCUMENT,
    END_DOCUMENT,
    START_ELEMENT,
    END_ELEMENT,
    SIZE
};

template<>
enum class FSD_XML::COMCommand{
    SIZE
};
// Config_Virtuals Section -----------------------------------------------------------


// Config_Priv_Section --------------------------------------------------------------------------


template<>
struct FileReaderInterpreter_Configuration_Priv<FSD_XML>::Data{
    QXmlStreamReader* reader = nullptr;    // For StartElement
};

template<>
QVector<COMFunctions_FRI<FSD_XML>> FileReaderInterpreter_Configuration_Priv<FSD_XML>::COMFunctions;




#endif // FRI_FSD_XML_HPP
