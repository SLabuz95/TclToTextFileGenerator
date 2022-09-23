#ifndef CONTROLLERCONFIGMANAGER_HPP
#define CONTROLLERCONFIGMANAGER_HPP

#include<QList>
#include<QDateTime>

class ControllerConfigInfo;
class ControllerConfigManager{
    // Ver 2 VVVVVVVVVVVVVVVVVVV
    QString lastErrorMsg_;
public:
    class ConfigFile{
    public:
        using ConfigPath = QString;
        using RegisterFileModificationTime = QDateTime;

    protected:
        ConfigPath configPath_;
        RegisterFileModificationTime modificationTime_;

    public:
        ConfigFile(){}
        ConfigFile(ConfigPath path, RegisterFileModificationTime time)
            : configPath_(path)
        {

        };

        bool isConfigured()const{
            return not configPath_.isEmpty();
        }
        const QString& configPath()const{return configPath_;}
        void changeConfigPath(const QString& newPath){configPath_ = newPath;}
    };


public:
    bool loadConfig(ControllerConfigInfo&);
    bool saveConfig(ControllerConfigInfo&);
    bool saveConfigAs(ControllerConfigInfo&, QString newPath);
    const QString& errorMsg()const{return lastErrorMsg_;}

};

#endif // CONTROLLERCONFIGMANAGER_HPP
