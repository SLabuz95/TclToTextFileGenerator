#include"External/FileReader/Reader/filereaderprivreadinfo.hpp"
#include"External/FileReader/Reader/filereaderpriv.hpp"

FileReader_Priv_ReadInfo::FileReader_Priv_ReadInfo(){
    stat.append({FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::PROCESSING), FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::INITIALIZE), FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::OPEN_FILE)});
}

