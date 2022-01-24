#ifndef XMLDEFINITION_HPP
#define XMLDEFINITION_HPP
#include<QString>
#include<QList>
#include<QXmlStreamReader>
#include<QBitArray>

/*struct XMLAttribute{
    const QString name;
};*/
/*
const XMLToken paramToken =
{
    .name = "param",
    .requiredTextContent = true
};

const XMLToken formatParamToken =
{
    .name = "formatParam",
    .requiredAttributes =
    {
        "type"
    }
};

const XMLToken conditionalActionToken =
{
    .name = "conditionalAction",
    .requiredAttributes =
    {
        "type"
    },
    .optionalTokens =
    {
        paramToken,
        formatParamToken
    }
};

const XMLToken executableActionToken =
{
    .name = "executableAction",
    .requiredAttributes =
    {
        "type"
    },
    .optionalTokens =
    {
        paramToken,
        formatParamToken
    }
};

const XMLToken ruleToken =
{
    .name = "rule",
    .optionalAttributes =
    {
        "controlFlag"
    },
    .optionalTokens =
    {
        conditionalActionToken,
        executableActionToken
    }
};

const XMLToken dynamicRulesToken =
{
    .name = "dynamicRules",
    .expectedNoDuplicates = true,
    .optionalTokens =
    {
        ruleToken
    }
};

const XMLToken rulesOnMoveToken =
{
    .name = "rulesOnMove",
    .expectedNoDuplicates = true,
    .optionalTokens =
    {
        ruleToken
    }
};

const XMLToken rulesForArgumentToken =
{
    .name = "rulesForArgument",
    .expectedNoDuplicates = true,
    .requieredAttributes =
    {
        "index"
    },
    .optionalTokens =
    {
        dynamicRulesToken,
        rulesOnMoveToken
    }
};

const XMLToken rulesForUnspecifiedArgumentToken =
{
    .name = "rulesForUnspecifiedArgument",
    .expectedNoDuplicates = true,
    .optionalTokens =
    {
        dynamicRulesToken,
        rulesOnMoveToken
    }
};

const XMLToken rulesOnEndOfCallToken =
{
    .name = "rulesOnEndOfCall",
    .optionalTokens =
    {
        ruleToken
    }
};

const XMLToken procedureToken =
{
    .name = "procedure",
    .requieredAttributes =
    {
        "name"
    },
    .optionalTokens =
    {
        rulesForArgumentToken,
        rulesForUnspecifiedArgumentToken,
        rulesOnEndOfCallToken
    }
};

const XMLToken defaultProcedureToken =
{
    .name = "defaultProcedure",
    .expectedNoDuplicates = true,
    .optionalTokens =
    {
        rulesForArgumentToken,
        rulesForUnspecifiedArgumentToken,
        rulesOnEndOfCallToken
    }
};

const XMLToken writeOnlyProcedureToken =
{
    .name = "procedure",
    .requiredTextContent = true
};

const XMLToken writeOnlyProceduresToken =
{
    .name = "writeOnlyProcedures",
    .expectedNoDuplicates = true,
    .optionalTokens =
    {
        writeOnlyProcedureToken
    }
};

const XMLToken settingsToken =
{
    .name = "settings",
    .expectedNoDuplicates = true,
    .optionalTokens =
    {
        writeOnlyProceduresToken
    }
};

const XMLToken userConfigToken =
{    
    .name = "userConfig",
    .expectedNoDuplicates = true,
    .requieredTokens =
    {

    },
    .optionalTokens =
    {
        settingsToken,
        procedureToken,
        defaultProcedureToken
    }
};

*/
#endif // XMLDEFINITION_HPP
