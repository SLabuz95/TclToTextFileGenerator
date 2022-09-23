#include"app.hpp"
#include<QRegularExpression>

int main(int argc, char** argv){


    App a(argc, argv);
    //if(!a.startupErrorChecking<>()) //
      //  return 0;
    return a.exec();
}

