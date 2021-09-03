#include"app.hpp"
#include<QRegularExpression>
#include"External/RegExpCore/regexprcore.hpp"
//#include"tclToCAPL.hpp"
#include<QLibrary>

int main(int argc, char** argv){

    //qDebug() << QStringList({"abc:cc"}).replaceInStrings(QRegularExpression(str), "\\1");
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
