#include"app.hpp"
#include<QRegularExpression>
#include"External/RegExpCore/regexprcore.hpp"
//#include"tclToCAPL.hpp"
#include<QLibrary>

struct A : public QPair<QString, int>{

public:
    using QPair<QString, int>::QPair;
    friend bool operator<(const A& lhs, const A& rhs){
        return (lhs.second < rhs.second and rhs.first == lhs.first)
                or lhs.first < rhs.first;
    }
};
// Note: as detailed above, the container must use std::less<> (or other
//   transparent Comparator) to access these overloads.
// This includes standard overloads, such as between std::string and std::string_view.

int main(int argc, char** argv){
/*
    // transparent comparison demo
    using Map =QMap<A, int>;
    Map map;

    map.insert({"set", -2}, 0);
    map.insert({"set", -1}, 1);
    map.insert({"set", 0}, 1);
    map.insert({"puts", -2}, 3);
    map.insert({"puts", -1}, 5);
    map.insert({"puts", 0}, 5);
    map.insert({"puts", 2}, 6);
*/
    /*qDebug() << "Test 1";
    {
        auto temp = (map.equal_range({"set", 2}));
        if(temp.first == map.end()){
            qDebug() <<  "No Key";
        }else{
            qDebug() << temp.first.key();
        }
        if(temp.second == map.end()){
            qDebug() <<  "No Key";
        }else{
            qDebug() << temp.second.key();
        }
    }
    qDebug() << "Test 2";
    {
        auto temp = (map.equal_range({"set", 0}));
        if(temp.first == map.end()){
            qDebug() <<  "No Key";
        }else{
            qDebug() << temp.first.key();
        }
        if(temp.second == map.end()){
            qDebug() <<  "No Key";
        }else{
            qDebug() << temp.second.key();
        }
    }
    qDebug() << "Test 3";
    {
        auto temp = (map.equal_range({"puts", 1}));
        if(temp.first == map.end()){
            qDebug() <<  "No Key";
        }else{
            qDebug() << temp.first.key();
        }
        if(temp.second == map.end()){
            qDebug() <<  "No Key";
        }else{
            qDebug() << temp.second.key();
        }
    }*/

    App a(argc, argv);
    //if(!a.startupErrorChecking<>()) //
      //  return 0;
    return a.exec();
}


// TODO: Sprawdzic przelot po plikach do generowania
// TODO: Progress Panel
// TODO: Generownie na podstawie plik
// TODO: Poprawa pliku konfiguracyjnego
// TODO: Dodanie QuickRules
// TODO: LAST: Panel do ustawania pliku konfiguracyjnego
// TODO: Predefinicje do pliku oraz dla testow
// TODO: Blokowanie dodawania komend w trybie predefinitions (ze wzgledow performance)
// TODO: Brak outputu = pierwszy input
