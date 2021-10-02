#ifndef FILEREADERPRIVREADINFO_HPP
#define FILEREADERPRIVREADINFO_HPP

#include<QFile>
class FileReader_Priv_ReadInfo{
public:
    FileReader_Priv_ReadInfo();
private:
    QFile file;
    QVector<int> stat;
public:
    inline void setFileName(QString fileName){file.setFileName(fileName);}
    inline void addStat(int newStat){stat.append(newStat);}
    inline void addStats(std::initializer_list<int> newStats){stat.append(newStats);}
    inline void removeLastStat(){stat.removeLast();}
    inline void removePreLastStat(){if(stat.size() > 1) stat.removeAt(stat.size() - 2);}
    inline void removeStat(int index){if(index > 0 && index < stat.size()) stat.remove(index);}
    inline void clearStats(){stat.clear();}
    inline int getLastStat(){if(stat.size() > 0) return stat.last(); else return 0;}
    inline int takeLastStat(){if(stat.size() > 0) return stat.takeLast(); else return 0;}
    inline int getPreLastStat(){if(stat.size() > 1) return stat.at(stat.size() - 2); else return 0;}
    inline int getStat(int index){if(index > 0 && index < stat.size()) return stat.at(index); else return 0;}

    // File Control
     bool openFile(){return file.open(QIODevice::Text | QIODevice::ReadOnly);}
     void closeFile(){file.close();}
     QFile& getFile(){return file;}
};

#endif // FILEREADERPRIVREADINFO_HPP
