#include <QApplication>
#include "FourierSeriesAnimation.h"

int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    FourierSeriesAnimation mainWin;
    mainWin.show();

    return app.exec();
}
