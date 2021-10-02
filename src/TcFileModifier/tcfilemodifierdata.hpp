#ifndef TCFILEMODIFIERDATA_HPP
#define TCFILEMODIFIERDATA_HPP

#include"External/FileReader/FilesSpecificData/FSDTemplate.hpp"
#include<QFile>
#include<QDir>
/*
class TcFileModifierData : public FSD_DataModelBase<TcFileModifierData>{
public:
    using FormatCount = QMap<QString, uint>;
protected:
    enum class _InitializeStatus{
        INITIALIZE_SUCCESS,
        INITIALIZE_FAIL,
        ALREADY_INITIALIZED
    };

    QFile _file;
    QFile& _reportFile;
    QDir _dir;
    inline QFile& file(){return _file;}
    //FormatCount _formatCount;
    QStringList _ifInstructions;
public:
    using InitializeStatus = _InitializeStatus;
    TcFileModifierData(QDir outputDir, QFile& reportFile) : _dir(outputDir), _reportFile(reportFile){}
    ~TcFileModifierData(){
        file().close();
    }
    //FormatCount& formatCount(){return _formatCount;}
    QStringList& ifInstructions(){return _ifInstructions;}
    inline QDir& dir(){return _dir;}
    InitializeStatus initialize();
    inline bool write(QString str){return file().write(str.toUtf8()) == str.toUtf8().size();};
    inline QFile& reportFile(){return _reportFile;}
    QString currentTCLFileName;
};
*/
#endif // TCFILEMODIFIERDATA_HPP
