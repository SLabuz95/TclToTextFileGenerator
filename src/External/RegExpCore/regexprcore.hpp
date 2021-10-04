#ifndef REGEXPRCORE_HPP
#define REGEXPRCORE_HPP
#include<QString>
#include<QRegularExpression>
namespace RegExpCore {
const QString regExpForUint = "^[\\s+]?\\d+[\\s+]?$";
const QString regExpForInt = "^[\\s+]?[-+]?\\d+[\\s+]?$";
const QString regExpForFloats = "^[\\s+]?[-+]?(\\d+([.,]\\d*)?|[.,]\\d+)([eE][-+]?\\d+)?[\\s+]?$";
const QString regExpForFloatsNoComma = "^[\\s+]?[-+]?(\\d+([.]\\d*)?|[.]\\d+)([eE][-+]?\\d+)?[\\s+]?$";
const QString regExpForHexString = "^[\\s+]?[A-Fa-f0-9]+[\\s+]?$";
const QString regExprForNestedCurvedBrackets = "({(?:{??{^{}*?}))";

// Specific (-99 - 99)
const QString regExprForIntRange_MinusNinetyNine2NinetyNine = "^(0|-?[1-9][0-9]?)$";

}

#endif // REGEXPRCORE_HPP
