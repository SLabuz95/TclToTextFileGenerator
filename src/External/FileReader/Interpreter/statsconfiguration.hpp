#ifndef STATSCONFIGURATION_HPP
#define STATSCONFIGURATION_HPP
#include<QStringList>
struct InterpreterStatInformation{
    QString paramName;
    int stat;
};

struct InterpreterStatsConfiguration{
    QVector<InterpreterStatInformation> statInfos;
    QVector<int> sizes;
    QVector<int> statInformationIndexForSizes;

    static const InterpreterStatsConfiguration initializeStatConfig(const QStringList paramList);
    bool findStatInfo(InterpreterStatInformation&, QString) const;
};


#endif // STATSCONFIGURATION_HPP
