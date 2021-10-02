
#include"statsconfiguration.hpp"
const InterpreterStatsConfiguration InterpreterStatsConfiguration::initializeStatConfig(const QStringList paramList){
    InterpreterStatsConfiguration config;
    for(int i = 0; i < paramList.size(); i++){
        int curParamLen = paramList.at(i).length();
        int j = 0;
        for( ; j < config.sizes.size(); j++){
            if(curParamLen < config.sizes.at(j)){
                config.sizes.insert(j, curParamLen);
                config.statInformationIndexForSizes.insert(j, config.statInformationIndexForSizes.at(j));
                config.statInfos.insert(config.statInformationIndexForSizes.at(j), {paramList.at(i), i});
                break;
            }else{
                if(curParamLen == config.sizes.at(j)){
                    config.statInfos.insert((j == config.sizes.size() - 1)? i : config.statInformationIndexForSizes.at(j+1) - 1, {paramList.at(i), i});
                    break;
                }
            }
        }
        if(j == config.sizes.size()){   // Just end of loop
            config.sizes.append(curParamLen);
            config.statInformationIndexForSizes.append(i);
            config.statInfos.append({paramList.at(i), i});
        }else{
           for(j++ ; j < config.sizes.size(); j++)
               config.statInformationIndexForSizes[j]++;
        }
    }
    return config;
}

bool InterpreterStatsConfiguration::findStatInfo(InterpreterStatInformation& statInfo, QString param) const{
    int i = 0;
    for(; i < sizes.size(); i++){
        if(param.size() == sizes.at(i))
            break;
    }
    if(i == sizes.size())
        return false;
    int stopCondition = (i == sizes.size() - 1)? statInfos.size() : statInformationIndexForSizes.at(i + 1);
    for(i = statInformationIndexForSizes.at(i); i < stopCondition; i++){
        if(statInfos.at(i).paramName == param){
            statInfo = statInfos.at(i);
            return true;
        }
    }
    return false;
}
