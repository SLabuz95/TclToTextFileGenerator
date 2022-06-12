#ifndef CONTROLLERCONFIGMANAGER_HPP
#define CONTROLLERCONFIGMANAGER_HPP

#include<QList>
#include"controllerconfiginfo.hpp"

class CorruptedConfigInfoEvent;
class ControllerConfigManager{
    // Ver 2 VVVVVVVVVVVVVVVVVVV


    // Ver 1 VVVVVVVVVVVVVVVVVVV
    /*
public:
    using EventHandlerPtr = QObject*;
    using EventHandlerPtrs = QList<EventHandlerPtr>;
    using Info = ControllerConfigInfo;
    using InfoRef = Info::SelfRef;
    using InfoPtr = Info::SelfPtr;
    using InfosBySubsribers = QMap<InfoPtr, EventHandlerPtrs>;
    using SubsribersByInfos = QMap<EventHandlerPtr, InfoPtr>;
    using ErrorMsg = QString;
    using CorruptedInfoEvent = CorruptedConfigInfoEvent;

protected:
    InfosBySubsribers infosBySubsribers;
    SubsribersByInfos temporaryConfigInfos;

    ErrorMsg _lastErrorMsg;

    void processCorupptedInfo(InfosBySubsribers::key_value_iterator);
    bool readConfig(QFileInfo&, Info::ControllerConfigRef);

    InfoPtr registerConfig(EventHandlerPtr, Info::ConfigPath = QString());
    InfoPtr reregisterConfig(InfoPtr, Info::ConfigPath);
    bool unregisterConfig(InfoPtr, EventHandlerPtr);

public:
    InfoPtr loadConfig(EventHandlerPtr, Info::ConfigPath = QString());
    bool unloadConfig(InfoPtr, EventHandlerPtr);
    InfoPtr changeConfig(InfoPtr, Info::ConfigPath);

    bool saveConfigOnly(InfoPtr);
    InfoPtr saveConfigAs(InfoPtr, Info::ConfigPath&);
    InfoPtr saveConfig(InfoPtr, Info::ConfigPath);
    inline ErrorMsg lastErrorMessage(){return _lastErrorMsg;}
    inline bool isInfoExist(InfoPtr info){return infosBySubsribers.keys().contains(info);}
    //static QString readRawConfig(QFileInfo, Info::ControllerConfigRef);
    */

};

#endif // CONTROLLERCONFIGMANAGER_HPP
