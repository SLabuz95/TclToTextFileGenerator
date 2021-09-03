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
}

#endif // REGEXPRCORE_HPP
