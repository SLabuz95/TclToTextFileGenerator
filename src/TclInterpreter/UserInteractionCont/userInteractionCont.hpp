#ifndef USERINTERACTIONCONT_HPP
#define USERINTERACTIONCONT_HPP

#include<QString>
#include<QList>

class UserInteraction{
    class Info{
        using Position = QString::size_type;
        Position startIndex;
        Position endIndex;
    };
    using Infos = QList<Info>;
    Infos infos;
public:
    enum Status : quint8{None, NotRequired, Required};
    static constexpr Status defaultStatus(){return Status::NotRequired;}
    inline Infos::size_type nextUserInteractionNumber(){return infos.size() + 1;}
    inline void addNewInfo(){infos.append(Info());}
    inline void clearInfos(){infos.clear() ;}
};


#endif // USERINTERACTIONCONT_HPP
