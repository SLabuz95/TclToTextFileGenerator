#ifndef FRI_COM_MANAGER_HPP
#define FRI_COM_MANAGER_HPP

#include<QVector>

namespace NS_FRI_COM_Manager {

    enum class CommandTo  : bool{
        READER,
        INTERPRETER
    };

    enum class CommandStat : bool{
        REQUEST,
        RESPONSE
    };

    enum class Commands{
        EXEC_STAT,
        DATA,

        SIZE
    };

    struct ChannelData{
        Commands command;
        void* channelData = nullptr;
        bool error = false;
    };
    using ChannelDatas = QVector<ChannelData>;
    using ChannelDatasIter = ChannelDatas::Iterator;

}
using namespace NS_FRI_COM_Manager;

class FRI_COM_Manager{

    struct Channel{
        QVector<ChannelData>* data = nullptr;
        ChannelDatasIter iter;
    };

public:
    using Channel = Channel;
    using COMChannel = Channel *const;
    using COMChannelRef = COMChannel&;


    static inline COMChannel activate(){
        return new Channel;
    }
    static void deactivate(Channel *const channel){
        delete channel;
    }
    static inline Commands nonPrivCommand(int cmd){
        return static_cast<Commands>(cmd + static_cast<int>(NS_FRI_COM_Manager::Commands::SIZE));
    }
    static void send(Channel *const channel, QVector<ChannelData>& cmd){
        channel->data = &cmd;
        channel->iter = cmd.begin();
    }
    static ChannelData& receive(Channel *const channel){
        return *(channel->iter);
    }
    static bool next(Channel *const channel){
        channel->iter++;
        return channel->iter < channel->data->end();
    }
};

#endif // FRI_COM_MANAGER_HPP
