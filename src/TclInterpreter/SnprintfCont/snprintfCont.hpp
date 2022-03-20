#ifndef SNPRINTFCONT_HPP
#define SNPRINTFCONT_HPP

#include<QString>

namespace Tcl::Interpreter{

    class SnprintfController{
        //QStringList snprintfDefinitions;
        uint numbOfDefinitions = 1;
    public:
        QString getNextSnprintfVarName(){
            QString snVarName = QString("_snStr") + QString::number(numbOfDefinitions++);
            return snVarName;}
        void clear(){numbOfDefinitions = 1;}
    };

};
#endif // SNPRINTFCONT_HPP
