#ifndef TCLTEXTINTERPRETER_HPP
#define TCLTEXTINTERPRETER_HPP

#include"TclInterpreter/tclinterpretercore.hpp"
#include<QList>

namespace Tcl::Interpreter{
    using namespace Core;
    class KeywordsController
    {
        // Concept -----------------------------------------------------
        using String = QString;
        using Char = QChar;
        using Chars = QVector<Char>;
        using CharPosition = QString::const_iterator;

        struct Keyword;
        using Keywords =  QVector<Keyword>;

        enum class ReadIgnoreResult : bool;
        enum class CheckingResult : uint;
        enum class CheckKeywordResult : bool;
        enum class Result : uint;

    public:
        using Result = Result;
        using CheckingResult = CheckingResult;
        using ReadIgnoreResult = ReadIgnoreResult;
        using KeywordsMap = QVector<Keywords>;
    private:
        // End of Concepts ||||||||||||||||||||||||||||||||||||||||||||
        // Concept Definition ------------------------------------------

        struct Keyword{
            enum class UnknownStringRule : bool{
              Forbidden,
              Permitted
            };
            const QString keyword;
            const Stat stat;
            const UnknownStringRule unknownStringRule = UnknownStringRule::Permitted;
        };

        enum class ReadIgnoreResult : bool{
            Satisfied = true,
            EndOfString = false
        };

        enum class CheckingResult : uint{
            StatFound,
            StatNotFound,

            EndOfString
        };

        enum class CheckKeywordResult : bool{
            Found,
            NotFound
        };

        enum class Result : uint{
           StatFound,
           //WaitForContinue,
           EndOfString,
           Error,
        };

        // End of Concept Definition ||||||||||||||||||||||||||||||||||
        // Objects ----------------------------------------------------
        Keywords::ConstIterator lastKeyword;
        const Keyword endOfStringKeyword = {"\n", {Stat::EndOfString}, Keyword::UnknownStringRule::Permitted};

        String savedStr; // Appended at end of String
        Chars ignoreReadUntilCondtions;
        CharPosition savedChar; // Used to save first character of substring which didnt match
        CharPosition savedFirstKeywordCharPos;
        CharPosition currentChar;
        CharPosition beginOfString;
        CharPosition endOfString;
        Result result = Result::StatFound;
        int lastKeywordLength = 0;
    public:
        static KeywordsMap keywordsMap;
    private:
        // End of Objects ||||||||||||||||||||||||||||||||||||||||||||
        // Functions --------------------------------------------------
        ReadIgnoreResult readIgnoreUntilPriv();
        ReadIgnoreResult readIgnoreIfPriv();
        CheckingResult checkingPriv();
        CheckingResult checkKeyword(int&);

        Result interpret();
        inline Char readCharacter(CharPosition chPos)const{return *chPos;}
        // End of Functions |||||||||||||||||||||||||||||||||||||||||||
        // Interface -------------------------------------------------
    public:
       ~KeywordsController(){}

        inline Stat lastKeywordStat()const{return lastKeyword->stat;}
        Error initialize(const QString&);
        Error deinitialize();

        inline Result runSearchingMode(){return interpret();}

        inline QString readLastKeyword(){return QString(savedFirstKeywordCharPos, currentChar - savedFirstKeywordCharPos);}
        inline bool isUnknownString(){return savedFirstKeywordCharPos - savedChar > 0;}
        inline QString readUnknownString(){return QString(savedChar, savedFirstKeywordCharPos - savedChar);}
        inline Error throwErrorIfUnknownStringForForbiddenRule(){
            return (lastKeyword->unknownStringRule == Keyword::UnknownStringRule::Forbidden
                    and isUnknownString())?  Error::Error : Error::NoError;
        }
        inline QString restOfString()const{return QString(currentChar, endOfString - currentChar);}
        inline QString readTclCommand()const{return QString(beginOfString, endOfString - beginOfString);}
        inline bool isCurrentChar(){return currentChar < endOfString;}
        inline CharPosition currentCharForSpecialSign(){return currentChar;}
        inline void incrementCurrentCharDueToSpecialSign(){currentChar++;}
        // End of Interface |||||||||||||||||||||||||||||||||||||||||||||

    };

};
#endif // TCLTEXTINTERPRETER_HPP
