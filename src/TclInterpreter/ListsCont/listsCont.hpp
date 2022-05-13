#ifndef LISTSCONT_HPP
#define LISTSCONT_HPP

#include"TclInterpreter/tclinterpretercore.hpp"
#include<QList>

namespace Tcl{
    class TCLInterpreter;
};

namespace Tcl::Interpreter{
    using namespace Core;
    /*
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
    };*/
    class ListController{
            using ListLevel = int;
        protected:
            TCLInterpreter& tclInterpreter;
            // Level 1+ - List exists
            static constexpr ListLevel LIST_EMPTY = 0;
            // Level 0 - No list
            static constexpr ListLevel DEACTIVATED_LIST_CONTROL = -1;
            // Level -1 - Deactivated list level control
            ListLevel currentListLevel = LIST_EMPTY;

            Error throwError();
            Error throwError(const QString str);
        public:
            ListController(TCLInterpreter& tclInterpreter);

            inline void deactivateListLevelControl(){currentListLevel = DEACTIVATED_LIST_CONTROL;}  // Theoretically: Shall be possible only in no list level
            inline void activateListLevelControl(){currentListLevel = LIST_EMPTY;}
            inline Error incrementListLevel(){
                if(currentListLevel == DEACTIVATED_LIST_CONTROL)
                    return Error::NoError;
                    //return throwError("ListController::incrementListLevel: List level control is inactive. Internal error.");
                currentListLevel++;
                return Error::NoError;
            }
            inline Error decrementListLevel(){
                if(currentListLevel == DEACTIVATED_LIST_CONTROL)
                    return Error::NoError;
                    //return throwError("ListController::decrementListLevel: List level control is inactive. Internal error.");
                if(currentListLevel == LIST_EMPTY)
                    return throwError("ListController::decrementListLevel: List doesnt exist. Internal error.");

                currentListLevel--;
                return Error::NoError;
            }
            inline bool isListDeactivated(){
                return currentListLevel == DEACTIVATED_LIST_CONTROL;
            }
            inline bool isListClosed(){
                return currentListLevel == DEACTIVATED_LIST_CONTROL or
                        currentListLevel == LIST_EMPTY;
            }

    };
};
#endif // LISTSCONT_HPP
