#ifndef SAVEDSTATCONT_HPP
#define SAVEDSTATCONT_HPP

#include"TclInterpreter/tclinterpretercore.hpp"

namespace Tcl::Interpreter {
    using namespace Tcl::Interpreter::Core;
    class SavedStat{
    protected:
        Stat pStat = Stat::None;
    //    ListStatInfo* info = nullptr;
        OutputCommand _command;

        void appendCommmand_listProcessing(OutputCommand command);
    public:
        SavedStat(){}
        SavedStat(Stat stat) : pStat(stat){}
        SavedStat(Stat stat, OutputCommand command) : pStat(stat), _command(command){}

        ~SavedStat(){

        }

        void setStat(Stat stat){pStat = stat;}
        Stat stat()const{return pStat;}
        Stat filteredStat()const{
            return (pStat > Stat::CommandSubbing)? Stat::CommandSubbing : pStat;
        }

        void clearMemory(){
    //        if(info){
    //            delete info;
    //            info = nullptr;
    //        }
        }

        void setCommand(OutputCommand command){
            _command = command;
        }
        void appendCommand(OutputCommand command){
            _command.append(command);
        }

        void hideInfoBeforeCopyConstructor(){/*info = nullptr;*/}
    //    inline ListStatInfo* advancedList()const{return info;}

        const OutputCommand& command()const{return _command;}

    //    void newListInfo(){
    //        if(!info)
    //            info = new ListStatInfo();
    //        /*if(not (command().isEmpty())){
    //            if((command().endsWith(" ") or command().endsWith("\t"))){
    //                info->newList(_command);
    //                _command.clear();
    //            }else{
    //                _command += "{";
    //            }
    //        }else{*/
    //            info->newList(_command);
    //            _command.clear();
    //        //}
    //    }

    //    inline ListStatInfo::EndListResult endList(){
    //        ListStatInfo::EndListResult result = ListStatInfo::EndListResult::EndOfList;
    //        if(info){
    //            switch((result = info->endList(command()))){
    //            case ListStatInfo::EndListResult::EndOfList:
    //                //_command = info->toCaplString(", ");
    //                _command.clear();
    //                break;
    //            default:
    //                _command.clear();
    //                break;
    //            }

    //        }
    //        return result;
    //    }

          QString listToCaplString();
    //    QString listToRawString(){return QString("{") + ((info)? info->toString("") : _command) + "}";}
        QString listToTclListString(){return /*((info)? info->toString("") :*/ _command;}
    //    inline bool isAdvancedList(){return info;}

    };
    inline namespace Types{
        using SavedStats = QVector<SavedStat>;
    }
};
#endif // SAVEDSTATCONT_HPP
