#include"FileReader/Reader/filereader.hpp"


/*
template<class FSD>
bool FileReader<FSD>::exec(){
    if(readInfos.isEmpty())
        return false;
    if(readInfos.last()->getLastStat() < FileReader_Priv::numbOfReadStats())
        return FileReader_Priv::exec();
    else
    {
        if(readInfos.last()->getLastStat() < realStat(FSD_Stat::SIZE))
            return (this->*fileReader_processingFunctions.at(readInfos.last()->takeLastStat() - FileReader_Priv::numbOfReadStats()))();
        else
            return false;
    }
}
*/
//extern const char NS_FileReader_Null::FILE_READER_SIGNATURE[] = "Null";

