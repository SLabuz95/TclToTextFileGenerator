#ifndef LISTSCONT_HPP
#define LISTSCONT_HPP

#include"TclInterpreter/tclinterpretercore.hpp"
#include<QList>

using namespace TclInterpreterCore;
class ListStatInfo{ // Special case for List stat (too avoid unnecessary virtualisations and dynamic memory management for every single stat)

    class ListInfo{
    public:
        ListInfo(QList<ListInfo> listInfos = {}, QString strData = QString())
            : listInfos(listInfos), strData(strData){}

        QList<ListInfo> listInfos;
        QString strData;

        QString toString(const QString&);
        QString toCaplListString();
    };

    enum class EndListResult : bool{
        NoEndOfList,
        EndOfList,
    };

public:
    using ListInfos = QVector<ListInfo>;
    using EndListResult = EndListResult;
protected:
    ListInfos listInfos;
    bool closeListEventOccured = false;

public:
    using ListInfo = ListInfo;

    ListStatInfo(){

    }

    void appendCommand(Command command);
    void newList(Command command);
    EndListResult endList(Command command);
    ListInfos::Iterator lastListInfo = nullptr;
    ListInfos& getListInfos(){return listInfos;}

    QString toString(const QString&);
    QString toCaplListString();
};


#endif // LISTSCONT_HPP
